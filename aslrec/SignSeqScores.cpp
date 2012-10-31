/**
    Copyright 2011 Ben Holm

    This file is part of RslAslRec.

    RslAslRec is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RslAslRec is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RslAslRec.  If not, see <http://www.gnu.org/licenses/>.
**/


#include "SignSeqScores.h"

SignSeqScores::SignSeqScores( int trainCount, int testCount, int maxHands) :
    trainSize( trainCount ),
    testSize( testCount ),
    maxnumberHands( maxHands ),
    scores( boost::extents[trainSize][testSize][maxnumberHands] ),
    preds( boost::extents[trainSize][testSize][maxnumberHands] )
{

    double initScore = std::numeric_limits<double>::max();
    Index initIndex;
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
                                 double thisDistance ) {
    double predDist = scores[last.model][last.test][last.hand];
    double distance = predDist == std::numeric_limits<double>::max() ? predDist
                                                                     : predDist + thisDistance;
    scores[model][test][hand] = distance;
    preds[model][test][hand] = last;
}

double SignSeqScores::bestScoreForEndFrame( int end ) {
    double best = std::numeric_limits<double>::max();
    // Set i to be the last mode frame:
    int i = trainSize - 1;
    //for( int i = 0; i < trainSize; ++i ) {
        // j = end!
        for( int k = 0; k < maxnumberHands; ++k ) {
            double thisScore = scores[i][end][k];
            if( thisScore < best ) best = thisScore;
        }
    //}
    return best;
}
