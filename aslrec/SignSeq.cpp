
#include "SignSeq.h"
#include "SignSeqScores.h"
#include "Databases.h"
#include "consts.h"
#include "boost/bind.hpp"
#include "boost/mem_fn.hpp"
#include "boost/shared_ptr.hpp"
#include <limits>

int factorial( int n ) {
    return (n == 0 || n == 1) ? 1 : factorial( n - 1 ) * n;
}

int maxHandCombos( int h ) {
  // The number of 2 combinations of h hands
  // h! / 2! * (h-2)!
  // + the number hand candidates for single hand signs
  int hbang = factorial( h ), h2bang = factorial( std::max( h - 2, 1 ) );
  return ( ( hbang / h2bang ) + 1 ) + h;
}

SignSeq::SignSeq() {
}

SignSeq::~SignSeq() {
}

int SignSeq::AddHands( cv::Point tl, cv::Point br,
                        const cv::Mat &dom, boost::shared_ptr<cv::Mat> weak ) {
    boost::shared_ptr<FeatureFrame> framePtr( new FeatureFrame( tl, br, dom, weak ) );
    frames.push_back( boost::shared_ptr<FeatureFrame>( framePtr ) );
    return TDB->AddHandToList( framePtr );
}

void SignSeq::AppendHistograms( std::list<Histogram> *hists ) const {
    FrameList::const_iterator begin = frames.begin(), end = frames.end();
    while( begin != end ) {
        const FramePtr frame = *begin;
        hists->push_back( frame->domHist );
        if( frame->weakHist.get() )
            hists->push_back( *(frame->weakHist.get()) );
        ++begin;
    }
}

double SignSeq::Distance( std::pair<int, int> interval, const cv::PCA &pca,
                          const cv::Mat &covar ) {
    // Each frame interval is checked independently. E.g. 1-2, 1-3, 1-4, 1-5, etc.
    // However, the interval from first-last will calculate all the intervening
    // values. So, no matter what interval is asked for, calculate from first-last
    // and memoize the results based on start frame (because each start frame has
    // a different result)
    MemoMap::iterator memo = scoreMemos.find( interval.first );
    if( memo == scoreMemos.end() ) { // Haven't seen this start yet
        int startFrame = interval.first, lastFrame = FDB->lastId();
        int size = ( lastFrame - startFrame ) + 1;
        int combinations = maxHandCombos( FDB->maxHands() );
        boost::shared_ptr<SignSeqScores> scores(
                new SignSeqScores( frames.size(), size, combinations ));
        Distance( *(scores.get()), std::make_pair( startFrame, lastFrame ),
                   pca, covar );
        std::pair<MemoMap::iterator, bool> inserted = scoreMemos.insert(
            std::make_pair( startFrame, scores ) );
        memo = inserted.first;
    }
    int idx = interval.second - interval.first;
    return GetBestScoreForEnd( *(memo->second.get()), idx );
}

void SignSeq::Distance( SignSeqScores &scores, std::pair<int, int> interval,
                          const cv::PCA &pca, const cv::Mat &covar ) {
    GenerateScoresForModelFrames( scores, interval, pca, covar );
}

void SignSeq::GenerateScoresForModelFrames( SignSeqScores &scores,
                                            std::pair<int, int> interval,
                                            const cv::PCA &pca, const cv::Mat &covar ) {
    unsigned int numFrames = frames.size();
    for( unsigned int i = 0; i < numFrames; ++i )
        GeneratorScoresForModel( scores, interval, i, pca, covar );
}

void SignSeq::GeneratorScoresForModel( SignSeqScores &scores,
                                       std::pair<int, int> interval,
                                       int modelIndex, const cv::PCA &pca,
                                       const cv::Mat &covar ) {
    for( int j = interval.first; j <= interval.second; ++j ) {
        GenerateScoresForTestFrame( scores, interval, modelIndex,
                                    j - interval.first, pca, covar );
    }
}

void SignSeq::GenerateScoresForTestFrame( SignSeqScores &scores,
                                          std::pair<int, int> interval,
                                          int modelIndex, int testIndex,
                                          const cv::PCA &pca, const cv::Mat &covar ) {

    ProjectionSet handCands = FDB->projections( interval.first + testIndex, pca );
    std::vector<HandPair> pairs = makePairs( handCands );

    for( unsigned int k = 0; k < pairs.size(); ++k ) {
        HandPair handPair = pairs[k];
        Cost( scores, interval, modelIndex, testIndex, (int)k,
              handPair, pca, covar );
    }
}

/* Helpers for GenerateScoresForTestFrame */
bool validPredecessor( std::pair<int, int> interval, SignSeqScores::Index &pre,
                       SignSeqScores::Index &cur );
bool bestPredecessor( SignSeqScores &scores,
                      SignSeqScores::Index l, SignSeqScores::Index r );

void SignSeq::Cost( SignSeqScores &scores,
                    std::pair<int, int> interval,
                    int modelIndex, int testIndex, int handIndex,
                    const SignSeq::HandPair &handPair,
                    const cv::PCA &pca, const cv::Mat &covar ) {

    // Short-circuit long intervals
    if( testIndex - interval.first > MAX_SIGN_LEN ) {
        scores.setDistance( modelIndex, testIndex, (int)handIndex,
                            std::numeric_limits<double>::max() );
        return;
    }

    if( modelIndex == 0 && testIndex == 0 ) {
        double distance = DistanceForPair( frames[modelIndex], handPair, pca, covar );
        scores.setDistance( modelIndex, testIndex, (int)handIndex, distance );
    }

    else {
        std::vector<SignSeqScores::Index> predecessors = scores.legalPredecessors(
                    modelIndex, testIndex, handIndex,
                    boost::bind( validPredecessor, interval, _1, _2 ) );

        if( predecessors.empty() == false ) {
            SignSeqScores::Index bestPred = *( std::min_element( predecessors.begin(),
                        predecessors.end(), boost::bind( bestPredecessor, scores, _1, _2 ) ) );
            if( scores.getDistance( bestPred ) < scores.maxDistance() ) {
                double distance = DistanceForPair( frames[modelIndex],
                                                   handPair, pca, covar );
                scores.setDistance( modelIndex, testIndex, handIndex,
                                    bestPred, distance );
            }
        }
    }
}

double SignSeq::DistanceForPair( SignSeq::FramePtr frame, const SignSeq::HandPair &handpair,
                                 const cv::PCA &pca, const cv::Mat &covar ) {
    if( handpair.second.get() )
        return frame->distance( handpair.first, *(handpair.second.get()), pca, covar );
    else
        return frame->distance( handpair.first, pca, covar );
}

double SignSeq::GetBestScoreForEnd( SignSeqScores &scores, int end ) {
    return scores.bestScoreForEndFrame( end );
}

std::vector<SignSeq::HandPair> SignSeq::makePairs( ProjectionSet &hands ) {
    int size = hands.size();
    std::vector<HandPair> pairs; pairs.reserve( size * size );
    for( int dom = 0; dom < size; ++dom )
        for( int weak = 0; weak < size; ++weak ) {
            if( weak != dom ) pairs.push_back( std::make_pair( hands[dom],
                                                               new Histogram( hands[weak] ) ) );
            else pairs.push_back( std::make_pair( hands[dom], (Histogram *)0 ) );
        }
    return pairs;
}

bool validPredecessor( std::pair<int, int> interval,
                       SignSeqScores::Index &pre, SignSeqScores::Index &cur ) {
    if( cur.test < 0 || pre.model < 0 || pre.test < 0 ) return false; // no illegal predecessors
    int firstFrame = interval.first;
    CenterSet curSet = FDB->handCenters( firstFrame + cur.test ),
              preSet = FDB->handCenters( firstFrame + pre.test );
    if( curSet.empty() || preSet.empty() ) return true;
    CenterPoint curCen = curSet[cur.hand], preCen = preSet[pre.hand];
    int xD = curCen.x - preCen.x,
        yD = curCen.y - preCen.y;
    unsigned int xSqr = xD * xD,
                 ySqr = yD * yD;
    return (xSqr + ySqr) < T0_SQR;
}

bool bestPredecessor( SignSeqScores &scores,
                      SignSeqScores::Index l, SignSeqScores::Index r ) {
    return scores.getDistance( l ) < scores.getDistance( r );
}

