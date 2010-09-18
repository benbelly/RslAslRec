
#ifndef EDGEDETECTION_H__
#define EDGEDETECTION_H__

#include "frame.h"

FrameSet getEdges( FrameSet &frames );
FrameSet getDilatedEdges( FrameSet &frames );
FrameSet negateAndMask( FrameSet &SDs, FrameSet &masks );

FrameSet removeSmallConnectedComponents( FrameSet &fs );

typedef std::vector< std::pair<Frame, ContourSet> > FrameHandSet;
FrameHandSet getBoundaryImages( FrameSet &fs );
#endif
