#include"boost/multi_array.hpp"
#include"frame.h"

typedef cv::PCA SignFeatureVec;
struct ModelSign {
    bool isME;
    std::string gloss;
    SignFeatureVec features;
};
typedef std::vector<ModelSign> ModelBase;

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

        typedef std::vector<Distance> D; // Distance from a sign for each frame

        void setDistance( int level, int frame, int sign,
                          D &ds, const Index &predK );
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

class LevelBuilder {
    public:
        LevelBuilder( ModelBase sDB, FrameSet v ) : 
            numLevels( 20 ), maxScore( 1E9 ),
            signDB( sDB ), video( v ), levels( numLevels, v.size(), sDB.size() )
        {}
        const int numLevels, maxScore;
        ModelBase signDB;
        FrameSet video;
        LevelArrays levels;
    private:
        double distance( ModelSign sign, int start, int end );
        LevelArrays::D DistancesForSign( ModelSign sign, int start );
        std::vector<LevelArrays::D> DistancesForAllSigns( int level );
        LevelArrays::Index findBestPrevious( int level, int frame );
        void DistancesForAllSigns( int level, int start );
        LevelArrays::Index prevFor( int level, int frame, int sign );
};
