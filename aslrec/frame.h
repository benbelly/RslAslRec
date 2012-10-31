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

#ifndef FRAME_H__
#define FRAME_H__

#include <vector>
#include <memory>
#include <string>
#include <utility>
#include "opencv/cv.h"
#include "opencv/highgui.h"

namespace image_types {
    enum {
        gray = CV_8UC1,
        big_gray = CV_16UC1
    };
}

struct Frame {
    Frame( int i ) : id( i ) {}
    Frame( int i, cv::Size size, int type ) : id( i ), mat( cv::Mat::zeros( size, type ) ) {}
    Frame( int i, cv::Mat &m ) : id( i ), mat( m ) {}
    Frame() : id( -1 ), mat() { }

    // Common accessors on cv::Mat
    cv::Size size() const;
    int type() const;
    int depth() const;
    int channels() const;
    int Id() const;

    int id; // frame index

    cv::Mat mat;
};
typedef std::vector<Frame> FrameSet;

typedef std::vector<cv::Point> Contour;
typedef std::vector<Contour> ContourSet;

typedef cv::Point2f CenterPoint;
typedef std::vector<CenterPoint> CenterSet;

//
// Frame loading functions
//
FrameSet loadFromFiles( std::vector<std::pair<std::string, int> > &files);
FrameSet loadFromVideo( std::string filename );

FrameSet loadFromByteArray( int *ids, char *arr, int count,
                            int width, int height, int type );

///
// Utility functions that should probably go somewhere else
///

FrameSet gray8bitTogray16bit( const FrameSet &source );

#endif

