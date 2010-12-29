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

//
// Frame loading functions
//
FrameSet loadFromFiles( std::vector<std::pair<std::string, int>> &files);
FrameSet loadFromVideo( std::string filename );

///
// Utility functions that should probably go somewhere else
///

FrameSet gray8bitTogray16bit( const FrameSet &source );

#endif

