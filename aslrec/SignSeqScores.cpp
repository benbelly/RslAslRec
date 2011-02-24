
#include "SignSeqScores.h"
#include<limits>

SignSeqScores::SignSeqScores( int trainCount, int testCount, int maxHands) :
    trainSize( trainCount ),
    testSize( testCount ),
    maxnumberHands( maxHands ),
    scores( boost::extents[trainSize][testSize][maxnumberHands] ),
    preds( boost::extents[trainSize][testSize][maxnumberHands] )
{

    double initScore = std::numeric_limits<double>::max();
    Index initIndex;
    //std::fill( scores.begin(), scores.end(), initScore ); // won't compile. Sheesh.
    //std::fill( preds.begin(), preds.end(), initIndex );
    for( int train = 0; train < trainSize; ++train )
        for( int test = 0; test < testSize; ++test )
            for( int hand = 0; hand < maxnumberHands; ++hand ) {
                scores[train][test][hand] = initScore;
                preds[train][test][hand] = initIndex;
            }
}

SignSeqScores::~SignSeqScores() {
}

void SignSeqScores::setDistance( int model, int test, int hand, double distance ) {
    scores[model][test][hand] = distance;
}

void SignSeqScores::setDistance( int model, int test, int hand, Index last,
                                 double distance ) {
    double predDist = scores[last.model][last.test][last.hand];
    if( last.model != -1 && last.test != -1 )
        scores[model][test][hand] = distance + predDist;
    preds[model][test][hand] = last;
}

double SignSeqScores::bestScoreForEndFrame( int end ) {
    double best = std::numeric_limits<double>::max();
    for( int i = 0; i < trainSize; ++i ) {
        // j = end!
        for( int k = 0; k < maxnumberHands; ++k ) {
            double thisScore = scores[i][end][k];
            if( thisScore < best ) best = thisScore;
        }
    }
    return best;
}
