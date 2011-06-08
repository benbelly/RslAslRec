
#include "SignSeq.h"
#include "SignSeqScores.h"
#include "Databases.h"
#include "consts.h"
#include "boost/bind.hpp"
#include "boost/mem_fn.hpp"
#include "boost/shared_ptr.hpp"
#include <limits>

extern HandPairCollection handPairs( int i, const cv::PCA &pca );
HandPairCollection GetHandPairs( int index, const cv::PCA &pca );
bool validPredecessor( std::pair<int, int> interval,
                       const cv::PCA &pca,
                       SignSeqScores::Index &pre,
                       SignSeqScores::Index &cur );
bool bestPredecessor( SignSeqScores &scores,
                      SignSeqScores::Index l, SignSeqScores::Index r );

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
                        const Contour &dom, boost::shared_ptr<Contour> weak ) {
    boost::shared_ptr<FeatureFrame> framePtr( new FeatureFrame( tl, br, dom, weak ) );
    frames.push_back( boost::shared_ptr<FeatureFrame>( framePtr ) );
    return TDB->AddHandToList( framePtr );
}

void SignSeq::AppendHistograms( std::list<Histogram> *hists ) const {
    FrameList::const_iterator begin = frames.begin(), end = frames.end();
    while( begin != end ) {
        const FramePtr frame = *begin;
        hists->push_back( frame->domHist );
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

inline void SignSeq::GeneratorScoresForModel( SignSeqScores &scores,
                                       std::pair<int, int> interval,
                                       int modelIndex, const cv::PCA &pca,
                                       const cv::Mat &covar ) {
    for( int j = interval.first; j <= interval.second; ++j ) {
        GenerateScoresForTestFrame( scores, interval, modelIndex,
                                    j - interval.first, pca, covar );
    }
}

inline void SignSeq::GenerateScoresForTestFrame( SignSeqScores &scores,
                                          std::pair<int, int> interval,
                                          int modelIndex, int testIndex,
                                          const cv::PCA &pca, const cv::Mat &covar ) {

    HandPairCollection pairs = handPairs( interval.first + testIndex, pca );

    for( int k = 0; k < pairs.size(); ++k ) {
        HandPairCollection::HandPair handPair = pairs.pair( k );
        Cost( scores, interval, modelIndex, testIndex, k,
              handPair, pca, covar );
    }
}

void SignSeq::Cost( SignSeqScores &scores,
                    std::pair<int, int> interval,
                    int modelIndex, int testIndex, int handIndex,
                    const HandPairCollection::HandPair &handPair,
                    const cv::PCA &pca, const cv::Mat &covar ) {

    // Short-circuit long intervals
    if( testIndex - interval.first > MAX_SIGN_LEN ) {
        scores.setDistance( modelIndex, testIndex, handIndex,
                            scores.maxDistance() );
        return;
    }

    if( modelIndex == 0 && testIndex == 0 ) {
        double distance = DistanceForPair( frames[modelIndex], handPair, pca, covar );
        scores.setDistance( modelIndex, testIndex, handIndex, distance );
    }

    else {
        std::vector<SignSeqScores::Index> predecessors = scores.legalPredecessors(
                    modelIndex, testIndex, handIndex,
                    boost::bind( validPredecessor, interval, pca, _1, _2 ) );

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

inline double SignSeq::DistanceForPair( SignSeq::FramePtr frame,
                                        const HandPairCollection::HandPair &handpair,
                                        const cv::PCA &pca, const cv::Mat &covar ) {
    double score = frame->distance( handpair, pca, covar );
    return score;
}

double SignSeq::GetBestScoreForEnd( SignSeqScores &scores, int end ) {
    return scores.bestScoreForEndFrame( end );
}

inline HandPairCollection GetHandPairs( int index, const cv::PCA &pca ) {
    return handPairs( index, pca );
}

bool validPredecessor( std::pair<int, int> /*interval*/, const cv::PCA &/*pca*/,
                       SignSeqScores::Index &pre, SignSeqScores::Index &cur ) {
    if( cur.test < 0 || pre.model < 0 || pre.test < 0 ) return false; // no illegal predecessors
    // The following code does not work - the algorithm was not described, so I made it up.
    // There may be a logic error, or it may be a bad algorithm. Since this is just a
    // performance step, and not a correctness step, short-circuit to check results.
    // Complete, correct, . . .
    return true;
    /*
     *int firstFrame = interval.first;
     *HandPairCollection curPair = GetHandPairs( firstFrame + cur.test, pca ),
     *                   prePair = GetHandPairs( firstFrame + pre.test, pca );
     *if( curPair.size() <= cur.hand || prePair.size() <= pre.hand ) return false;
     *HandPairCollection::CenterPair
     *            curCen = curPair.center( cur.hand ),
     *            preCen = prePair.center( pre.hand );
     * // First Hand
     *int xD = curCen.first.x - preCen.first.x,
     *    yD = curCen.first.y - preCen.first.y;
     *unsigned int xSqr = xD * xD,
     *             ySqr = yD * yD;
     *bool firstHandClose = (xSqr + ySqr) < T0_SQR;
     * // Second Hand
     *boost::shared_ptr<CenterPoint> curSec = curCen.second,
     *                               preSec = preCen.second;
     *bool secondMatch = ( curSec.get() && preSec.get() ) || ( !curSec.get() && !preSec.get() );
     *bool secondClose = secondMatch;
     *if( secondMatch && curSec.get() ) {
     *    xD = curSec->x - preSec->x;
     *    yD = curSec->y - preSec->y;
     *    xSqr = xD * xD; ySqr = yD * yD;
     *    secondClose = (xSqr + ySqr) < T0_SQR;
     *}
     *return firstHandClose && secondClose;
     */
}

bool bestPredecessor( SignSeqScores &scores,
                      SignSeqScores::Index l, SignSeqScores::Index r ) {
    return scores.getDistance( l ) < scores.getDistance( r );
}

