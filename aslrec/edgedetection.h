/**
    Copyright 2011 Ben Holm

    This file is part of RslAslRec.

    RslAslRec is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RslAslRec is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RslAslRec.  If not, see <http://www.gnu.org/licenses/>.
**/


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
