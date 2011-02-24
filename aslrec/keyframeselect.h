#ifndef KEYFRAMESELECT_H__
#define KEYFRAMESELECT_H__

#include "frame.h"

// Functor to find keyframes
// This requires grayscale images
struct AccumKeyframes {
    AccumKeyframes( std::vector<double> &ds,
                    FrameSet &kfs ) : keyframes( kfs ),
                                      diffs( ds ) {}
    FrameSet &keyframes;
    std::vector<double> &diffs;
    Frame operator() ( Frame lastKey, Frame next );
};
#endif
