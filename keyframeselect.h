#ifndef KEYFRAMESELECT_H__
#define KEYFRAMESELECT_H__

#include "frame.h"

// Functor to find keyframes
// This requires grayscale images
struct AccumKeyframes {
    AccumKeyframes( FrameSet &kfs ) : keyframes( kfs ) {}
    FrameSet &keyframes;
    Frame operator() ( Frame lastKey, Frame next );
};
#endif
