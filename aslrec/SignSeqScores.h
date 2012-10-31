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


#ifndef SIGNSEQSCORES_H__
#define SIGNSEQSCORES_H__

#include "boost/multi_array.hpp"
#include<vector>
#include<limits>

class SignSeqScores {
    public:
        SignSeqScores( int trainCount, int testCount, int maxHands );
        virtual ~SignSeqScores();

        typedef boost::multi_array<double, 3> Scores;

        struct Index {
            Index( int m, int t, int h ) : model( m ), test( t ), hand( h ) { }
            Index() : model( -1 ), test( -1 ), hand( -1 ) { }
            int model, test, hand;
        };
        typedef boost::multi_array<Index, 3> Predecessors;

        void setDistance( int model, int test, int hand, double distance );
        void setDistance( int model, int test, int hand, Index last, double distance );
        double maxDistance() { return std::numeric_limits<double>::max(); }
        double getDistance( Index i ) { return scores[i.model][i.test][i.hand]; }
        double bestScoreForEndFrame( int end );

        /*
         * Legal predecessors are defined in Section 3.3 of the LevelBuilding
         * paper. This method collects the indices of legal predecessors
         * and validates them with the provided predicate.
         * NOTE: The Predicate can receive negative values
         */
        template<typename Predicate>
        std::vector<Index> legalPredecessors( int model, int test, int hand,
                                              Predicate valid ) {
            Index i1( model, test - 1, hand ),
                  i2( model - 1, test - 1, hand ),
                  i3( model - 1, test, hand ),
                  current( model, test, hand );
            std::vector<Index> indices;

            if( valid( i3, current ) ) indices.push_back( i3 );

            for( int i = 0; i < maxnumberHands; ++i ) {
                i1.hand = i; i2.hand = i; i3.hand = i;
                if( valid( i1, current ) ) indices.push_back( i1 );
                if( valid( i2, current ) ) indices.push_back( i2 );
            }
            return indices;
        }


    private:
        const int trainSize, testSize, maxnumberHands;
        Scores scores;
        Predecessors preds;
};

#endif
