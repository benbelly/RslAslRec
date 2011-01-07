
#include "SignSeq.h"
#include "SignSeqScores.h"
#include "Databases.h"
#include "consts.h"
#include "boost/bind.hpp"
#include <limits>

SignSeq::SignSeq() {
}

SignSeq::~SignSeq() {
}

int SignSeq::AddHands( cv::Point tl, cv::Point br,
                        const cv::Mat &dom, const cv::Mat &weak ) {
    std::shared_ptr<FeatureFrame> framePtr( new FeatureFrame( tl, br, dom, weak ) );
    hands.push_back( std::shared_ptr<FeatureFrame>( framePtr ) );
    return TDB->AddHandToList( framePtr );
}

double SignSeq::Distance( std::pair<int, int> interval ) {
    SignSeqScores scores( hands.size(), interval.second - interval.first, FDB->maxHands() );
    GenerateScoresForModelFrames( scores, interval );
    return GetBestScoreForEnd( scores, interval.second );
}

void SignSeq::GenerateScoresForModelFrames( SignSeqScores &scores,
                                            std::pair<int, int> interval ) {
    for( unsigned int i = 0; i < hands.size(); ++i )
        GeneratorScoresForModel( scores, interval, i );
}

void SignSeq::GeneratorScoresForModel( SignSeqScores &scores,
                                       std::pair<int, int> interval,
                                       int modelIndex ) {
    for( int j = interval.first; j <= interval.second; ++j )
        GenerateScoresForTestFrame( scores, modelIndex, j );
}

/* Helpers for GenerateScoresForTestFrame */
bool validPredecessor( SignSeqScores::Index pre, SignSeqScores::Index cur );
bool bestPredecessor( SignSeqScores &scores,
                      SignSeqScores::Index l, SignSeqScores::Index r );

void SignSeq::GenerateScoresForTestFrame( SignSeqScores &scores,
                                          int modelIndex, int testIndex ) {
    HistogramSet handCands = FDB->histograms( testIndex );
    std::vector<std::pair<int, int> > pairs = makePairs( handCands );
    for( unsigned int k = 0; k < handCands.size(); ++k ) {
        std::pair<int, int> handPair = pairs[k];
        double distance = hands[modelIndex]->distance( handCands[handPair.first],
                                                       handCands[handPair.second] );
        std::vector<SignSeqScores::Index> predecessors = scores.legalPredecessors(
                modelIndex, testIndex, k, std::ptr_fun( validPredecessor ) );
        SignSeqScores::Index bestPred = *( std::min_element( predecessors.begin(),
                    predecessors.end(), boost::bind( bestPredecessor, scores, _1, _2 ) ) );
        scores.setDistance( modelIndex, testIndex, (int)k, bestPred, distance );
    }
}

double SignSeq::GetBestScoreForEnd( SignSeqScores &scores, int end ) {
    return scores.bestScoreForEndFrame( end );
}

std::vector<std::pair<int, int> > SignSeq::makePairs( HistogramSet &hands ) {
    int size = hands.size(), pairCount = size * size - size;
    std::vector<std::pair<int, int> > pairs; pairs.reserve( pairCount );
    for( int dom = 0; dom < size; ++dom )
        for( int weak = 0; weak < size; ++weak )
            if( weak != dom ) pairs.push_back( std::make_pair( dom, weak ) );
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

