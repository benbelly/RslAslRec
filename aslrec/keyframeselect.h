#ifndef KEYFRAMESELECT_H__
#define KEYFRAMESELECT_H__

#include "frame.h"

// Functor to find keyframes
// This requires grayscale images
struct AccumKeyframes {
    AccumKeyframes( const unsigned int t1,
                    std::vector<double> &ds,
                    FrameSet &kfs ) : T1( t1 ),
                                      keyframes( kfs ),
                                      diffs( ds ) {}
    const unsigned int T1;
    FrameSet &keyframes;
    std::vector<double> &diffs;
    Frame operator() ( Frame lastKey, Frame next );
};
#endif
