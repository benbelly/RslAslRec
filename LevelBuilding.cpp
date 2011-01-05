
#include<vector>
#include<list>
#include<utility>
#include<limits>
#include<algorithm>
#include<functional>
#include<string>
#include"LevelBuilding.h"

double LevelBuilder::distance( ModelSign sign, int start, int end ) {
    // For each FrameScore, find the minimal distance to all possible
    // subsequences of the model sign.
    return 0.0;
}


LevelArrays::FrameDistance LevelBuilder::FrameDistance( ModelSign sign, int start ) {
    unsigned int endFrame = FDB->size(),
                 numFrames = endFrame - start;
    LevelArrays::FrameDistance fds( numFrames, maxScore );
    for( unsigned int m = start; m < endFrame; ++m ) {
        unsigned int loc = m - start;
        fds[loc] = distance( sign, start, m );
    }
    return fds;
}

std::vector<LevelArrays::FrameDistance> LevelBuilder::FrameDistance( int startFrame ) {
    std::vector<LevelArrays::FrameDistance> sds; sds.reserve( signDB.size() );
    ModelBase::iterator begin = signDB.begin(), end = signDB.end();
    while( begin != end )
        sds.push_back( DistancesForSign( *begin++, startFrame ) );
    return sds;
}

LevelArrays::Index LevelBuilder::findBestPrevious( int level, int frame ) {
    LevelArrays::FrameDistance best = std::numeric_limits<double>::max();
    int bestSign = -1, pl = level - 1, pf = frame - 1;
    for( unsigned int sign = 0; sign < signDB.size(); ++sign ) {
        double dist = levels.distanceFor( pl, pf, sign );
        if( dist < best ) {
            best = dist; bestSign = sign;
        }
    }
    return LevelArrays::Index( pl, pf, bestSign );
}

void LevelBuilder::DistancesForAllSigns( int level, int start ) {
    std::vector<LevelArrays::FrameDistance> dists = DistancesForAllSigns( start );
    LevelArrays::Index pred = findBestPrevious( level, start );
    for( unsigned int i = 0; i < dists.size(); ++i )
        levels.setDistance( level, start, i, dists[i], pred );
}

void LevelBuilder::BuildLevels() {
    int start = 0;
    // Prepopulate the first level (0) with startSign
    for( int l = 1; i < numLevels; ++i )
        DistancesForAllSigns( l, start++ );
}
