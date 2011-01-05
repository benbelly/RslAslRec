
#include "LevelArrays.h"
#include <iostream>


inline void ASSERT( bool valid, std::string msg ) {
    if( valid ) return;
    std::cerr << msg << std::endl;
    exit( 1 );
}

// Client has selected distances and a predecessor - insert them
void LevelArrays::setDistance( int level, int frame, int sign,
                               LevelArrays::FrameDistance &ds,
                               const Index &pred ) {
    ASSERT( frame + ds.size() < (unsigned int)numFrames,
            "Too many distances passed to setDistance" );
    double predDist = a[pred.level][pred.frame][pred.sign];
    for( unsigned int f = 0; f < ds.size(); ++f ) {
        a[level][frame + f][sign] = ds[f] + predDist;
        v[level][frame + f][sign] = pred;
    }
}

