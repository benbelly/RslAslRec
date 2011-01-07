
#ifndef EDGEDETECTION_H__
#define EDGEDETECTION_H__

#include "frame.h"

FrameSet getEdges( FrameSet &frames );
FrameSet getDilatedEdges( FrameSet &frames );
FrameSet negateAndMask( FrameSet &SDs, FrameSet &masks );

FrameSet removeSmallConnectedComponents( FrameSet &fs );
Frame removeComponentsFromFrame( Frame f );

typedef std::vector< std::pair<Frame, ContourSet> > FrameHandSet;
FrameHandSet getBoundaryImages( FrameSet &fs );

Contour getBoundary( const cv::Mat &img );

CenterPoint center( Contour );
CenterSet centers( ContourSet & );
#endif
