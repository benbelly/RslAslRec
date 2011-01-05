
#ifndef LEVELARRAYS_H__
#define LEVELARRAYS_H__

#include"boost/multi_array.hpp"
#include"frame.h"

class LevelArrays {
    public:
        LevelArrays( int levels, int frames, int signs ) :
          numLevels( levels ), numFrames( frames ), numSigns( signs ),
          a( boost::extents[numLevels][numFrames][numSigns] ),
          v( boost::extents[numLevels][numFrames][numSigns] ) {
              makeA();
              makeV();
        }

        typedef double Distance;
        typedef boost::multi_array<Distance, 3> A;
        struct Index {
            Index( int l, int f, int s ) : level(l), frame(f), sign(s) { }
            Index() : level(-1), frame(-1), sign(-1) {}
            int level, frame, sign;
        };
        typedef boost::multi_array<Index, 3> V;

        typedef std::vector<Distance> FrameDistance; // Distance from a sign for each frame

        void setDistance( int level, int frame, int sign,
                          FrameDistance &ds, const Index &predK );
        Distance distanceFor( Index i ) { return a[i.level][i.frame][i.sign]; }

    private:
        // Order of construction is assumed. Do not rearrange
        const int numLevels, numFrames, numSigns;
        A a;
        V v;

        // Is this really how to initialize a multi_array
        template<typename ArrType>
        void initArr( ArrType &arr, typename ArrType::element init ) {
            for( int l = 0; l < numLevels; ++l )
                for( int f = 0; f < numFrames; ++f )
                    for( int s = 0; s < numSigns; ++s )
                        arr[l][f][s] = init;
        }
        void makeA() { initArr( a, (A::element)0.0 ); }
        // Make a predecessor collection for a given A
        void makeV() { initArr( v, (V::element)Index() ); }
};

#endif
