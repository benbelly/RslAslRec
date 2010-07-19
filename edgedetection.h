
#ifndef EDGEDETECTION_H__
#define EDGEDETECTION_H__

#include "frame.h"

FrameSet getEdges( FrameSet &frames );
FrameSet getDilatedEdges( FrameSet &frames );
FrameSet negateAndMask( FrameSet &SDs, FrameSet &masks );

FrameSet removeSmallConnectedComponents( FrameSet &fs );
#endif
