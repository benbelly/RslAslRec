
#ifndef SIGNSEQSCORES_H__
#define SIGNSEQSCORES_H__

#include "boost/multi_array.hpp"
#include<vector>

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

        void setDistance( int model, int test, int hand, Index last, double distance );
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
            for( int i = 0; i < maxnumberHands; ++i ) {
                i1.hand = i; i2.hand = i; i3.hand = i;
                if( valid( i1, current ) ) indices.push_back( i1 );
                if( valid( i2, current ) ) indices.push_back( i2 );
                if( valid( i3, current ) ) indices.push_back( i3 );
            }
            return indices;
        }


    private:
        const int trainSize, testSize, maxnumberHands;
        Scores scores;
        Predecessors preds;
};

#endif
