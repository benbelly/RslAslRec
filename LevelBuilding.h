#include "Databases.h"
#include "LevelArrays.h"

typedef cv::PCA SignFeatureVec;
struct ModelSign {
    bool isME;
    std::string gloss;
    SignFeatureVec features;
};
typedef std::vector<ModelSign> ModelBase;


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
        LevelArrays::FrameDistance DistancesForSign( ModelSign sign, int start );
        std::vector<LevelArrays::FrameDistance> DistancesForAllSigns( int level );
        LevelArrays::Index findBestPrevious( int level, int frame );
        void DistancesForAllSigns( int level, int start );
        LevelArrays::Index prevFor( int level, int frame, int sign );
};
