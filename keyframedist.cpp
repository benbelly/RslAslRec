#include <vector>
#include <numeric>
#include "frame.h"
#include "keyframedist.h"
//#include "logging.h"
#include "consts.h"

#include "FrameDB.h"

unsigned char zeroOrMax( unsigned long in ) {
    return in ? 255 : 0;
}

Frame avgDist2( const FrameSet &keyFrames, const Frame f ) {
    const Frame src( FDB->skin( f.id ) );
    unsigned int size = src.mat.dataend - src.mat.datastart;
    unsigned long *diffSum = new unsigned long[ size ];

    FrameSet::const_iterator end = keyFrames.end();
    for( FrameSet::const_iterator i = keyFrames.begin(); i != end; ++i ) {
        Frame kf( FDB->skin( i->id ) );
        unsigned char *fi = src.mat.datastart, *ki = kf.mat.datastart;
        for(unsigned long *j = diffSum; j != diffSum + size; ++j, ++fi, ++ki ) {
            *j += ( *fi - *ki );
        }
    }

    for( unsigned long *j = diffSum; j != diffSum + size; ++j ) {
        *j = *j / ( keyFrames.size() - 1 );
    }

    Frame dst( src.id, src.size(), src.type() );
    //std::copy( diffSum, diffSum + size, dst.mat.datastart );
    std::transform( diffSum, diffSum + size, dst.mat.datastart,
                    std::ptr_fun( zeroOrMax ) );

    delete[] diffSum;
    return dst;
}

