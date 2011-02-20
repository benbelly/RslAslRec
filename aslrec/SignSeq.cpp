
#include "SignSeq.h"
#include "SignSeqScores.h"
#include "Databases.h"
#include "consts.h"
#include "boost/bind.hpp"
#include "boost/mem_fn.hpp"
#include <limits>

SignSeq::SignSeq() {
}

SignSeq::~SignSeq() {
}

int SignSeq::AddHands( cv::Point tl, cv::Point br,
                        const cv::Mat &dom, const cv::Mat &weak ) {
    boost::shared_ptr<FeatureFrame> framePtr( new FeatureFrame( tl, br, dom, weak ) );
    frames.push_back( boost::shared_ptr<FeatureFrame>( framePtr ) );
    return TDB->AddHandToList( framePtr );
}

void SignSeq::AppendHistograms( std::list<Histogram> &hists ) const {
    FrameList::const_iterator begin = frames.begin(), end = frames.end();
    while( begin != end ) {
        const FramePtr frame = *begin;
        hists.push_back( frame->domHist );
        if( frame->weakHist.get() )
            hists.push_back( *(frame->weakHist.get()) );
        ++begin;
    }
}

double SignSeq::Distance( std::pair<int, int> interval, const cv::PCA &pca,
                          const cv::Mat &covar ) {
    SignSeqScores scores( frames.size(), interval.second - interval.first, FDB->maxHands() );
    GenerateScoresForModelFrames( scores, interval, pca, covar );
    return GetBestScoreForEnd( scores, interval.second );
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
    for( int j = interval.first; j <= interval.second; ++j )
        GenerateScoresForTestFrame( scores, modelIndex, j, pca, covar );
}

/* Helpers for GenerateScoresForTestFrame */
bool validPredecessor( SignSeqScores::Index pre, SignSeqScores::Index cur );
bool bestPredecessor( SignSeqScores &scores,
                      SignSeqScores::Index l, SignSeqScores::Index r );

void SignSeq::GenerateScoresForTestFrame( SignSeqScores &scores,
                                          int modelIndex, int testIndex,
                                          const cv::PCA &pca, const cv::Mat &covar ) {
    HistogramSet handCands = FDB->histograms( testIndex );
    std::vector<HandPair> pairs = makePairs( handCands );
    for( unsigned int k = 0; k < handCands.size(); ++k ) {
        HandPair handPair = pairs[k];
        double distance = DistanceForPair( frames[modelIndex], handPair, pca, covar );
        std::vector<SignSeqScores::Index> predecessors = scores.legalPredecessors(
                modelIndex, testIndex, k, std::ptr_fun( validPredecessor ) );
        SignSeqScores::Index bestPred = *( std::min_element( predecessors.begin(),
                    predecessors.end(), boost::bind( bestPredecessor, scores, _1, _2 ) ) );
        scores.setDistance( modelIndex, testIndex, (int)k, bestPred, distance );
    }
}

double SignSeq::DistanceForPair( SignSeq::FramePtr frame, SignSeq::HandPair &handpair,
                                 const cv::PCA &pca, const cv::Mat &covar ) {
    if( handpair.second.get() )
        return frame->distance( handpair.first, *(handpair.second.get()), pca, covar );
    else
        return frame->distance( handpair.first, pca, covar );
}

double SignSeq::GetBestScoreForEnd( SignSeqScores &scores, int end ) {
    return scores.bestScoreForEndFrame( end );
}

std::vector<SignSeq::HandPair> SignSeq::makePairs( HistogramSet &hands ) {
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

bool validPredecessor( SignSeqScores::Index pre, SignSeqScores::Index cur ) {
    if( cur.test <= 0 ) return false; // no predecessors for first test frame
    if( pre.model <= 0 || pre.test <= 0 ) return false; // no illegal predecessors
    CenterPoint curCen = FDB->handCenters( cur.test )[cur.hand],
                preCen = FDB->handCenters( pre.test )[pre.hand];
    int xD = curCen.x - preCen.x,
        yD = curCen.y - preCen.y;
    unsigned int xSqr = xD * xD,
                 ySqr = yD * yD;
    return (xSqr + ySqr) < T0_SQR;
}

bool bestPredecessor( SignSeqScores &scores,
                      SignSeqScores::Index l, SignSeqScores::Index r ) {
    if( l.model == -1 || l.test == -1 ) return false;
    if( r.model == -1 || r.test == -1 ) return true;
    return scores.getDistance( l ) < scores.getDistance( r );
}

