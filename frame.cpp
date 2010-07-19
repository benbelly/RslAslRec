
#include "frame.h"
#include <map>
#include <algorithm>

Frame convertTo16bit( const Frame src ) {
    Frame dst( src.id, src.size(), CV_16U );
    src.mat.convertTo( dst.mat, CV_16U );
    return dst;
}

FrameSet gray8bitTogray16bit( const FrameSet &src ) {
    FrameSet bigs; bigs.reserve( src.size() );
    std::transform( src.begin(), src.end(), std::back_inserter( bigs ),
                    std::ptr_fun( convertTo16bit ) );
    return bigs;
}

static std::map<int, Frame> frames;
static int nextFrame = 0;

Frame getFrame( int id ) {
    return frames[nextFrame];
}

int newFrame( Frame &f ) {
    frames[++nextFrame] = f;
    return nextFrame;
}

std::vector<int> framesToIds( FrameSet &fs ) {
    return std::vector<int>();
}

FrameSet idsToFrames( std::vector<int> &ids ) {
    return FrameSet();
}
