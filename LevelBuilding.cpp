
#include<vector>
#include<list>
#include<utility>
#include<limits>
#include<algorithm>
#include<functional>
#include<string>
#include<iostream>
#include"LevelBuilding.h"

inline void ASSERT( bool valid, std::string msg ) {
    if( valid ) return;
    std::cerr << msg << std::endl;
    exit( 1 );
}

// Client has selected distances and a predecessor - insert them
void LevelArrays::setDistance( int level, int frame, int sign,
                               LevelArrays::D &ds, const Index &pred ) {
    ASSERT( frame + ds.size() < (unsigned int)numFrames,
            "Too many distances passed to setDistance" );
    double predDist = a[pred.level][pred.frame][pred.sign];
    for( unsigned int f = 0; f < ds.size(); ++f ) {
        a[level][frame + f][sign] = ds[f] + predDist;
        v[level][frame + f][sign] = pred;
    }
}

double LevelBuilder::distance( ModelSign sign, int start, int end ) {
    // For each FrameScore, find the minimal distance to all possible
    // subsequences of the model sign.
    return 0.0;
}


LevelArrays::D LevelBuilder::DistancesForSign( ModelSign sign, int start ) {
    unsigned int numFrames = video.size() - start;
    LevelArrays::D fds( numFrames, maxScore );
    for( unsigned int m = start; m < video.size(); ++m ) {
        unsigned int loc = m - start;
        fds[loc] = distance( sign, start, m );
    }
    return fds;
}

std::vector<LevelArrays::D> LevelBuilder::DistancesForAllSigns( int startFrame ) {
    std::vector<LevelArrays::D> sds; sds.reserve( signDB.size() );
    ModelBase::iterator begin = signDB.begin(), end = signDB.end();
    while( begin != end )
        sds.push_back( DistancesForSign( *begin++, startFrame ) );
    return sds;
}

LevelArrays::Index LevelBuilder::findBestPrevious( int level, int frame ) {
    LevelArrays::Distance best = std::numeric_limits<double>::max();
    int bestSign = -1, pl = level - 1, pf = frame - 1;
    for( unsigned int sign = 0; sign < signDB.size(); ++sign ) {
        if( levels.distanceFor( pl, pf, sign ) < best ) {
            best = levels.distanceFor( pl, pf, sign );
            bestSign = sign;
        }
    }
    return LevelArrays::Index( pl, pf, bestSign );
}

void LevelBuilder::DistancesForAllSigns( int level, int start ) {
    std::vector<LevelArrays::D> dists = DistancesForAllSigns( start );
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
