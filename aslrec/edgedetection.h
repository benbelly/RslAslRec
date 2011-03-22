
#ifndef EDGEDETECTION_H__
#define EDGEDETECTION_H__

#include "frame.h"

Frame getEdge( Frame f, bool dilate );
FrameSet getEdges( FrameSet &frames );
FrameSet getDilatedEdges( FrameSet &frames );
Frame negateAndMaskFrame( Frame SD, Frame mask );
FrameSet negateAndMask( FrameSet &SDs, FrameSet &masks );

Frame removeComponentsFromFrame( Frame f );
FrameSet removeSmallConnectedComponents( FrameSet &fs );

typedef std::vector< std::pair<Frame, ContourSet> > FrameHandSet;
std::pair<Frame, ContourSet> getBoundaryImage( Frame f );
FrameHandSet getBoundaryImages( FrameSet &fs );

ContourSet getHands( const cv::Mat &img );
Contour getBoundary( const cv::Mat &img );

CenterPoint center( Contour );
CenterSet centers( ContourSet & );
#endif
