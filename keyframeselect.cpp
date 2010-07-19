

#include <numeric>
#include <algorithm>
#include <iterator>
#include <memory>

#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "keyframeselect.h"
#include "consts.h"
#include "logging.h"

double calculateDiff( const Frame &l, const Frame &r );
double sizeOfLargestConnectedComponent( cv::Mat &differenceImage );
double maxAreaOfContours( const ContourSet &contours );
double absArea( const Contour &mat );

Frame AccumKeyframes::operator() ( Frame lastKey, Frame next ) {
    double diff = calculateDiff( next, lastKey );
    if( diff > T1 ) {
        keyframes.push_back( next );
        return next;
    }
    return lastKey;
}

double calculateDiff( const Frame &l, const Frame &r ) {
    cv::Mat diff( l.size(), l.type() );
    cv::absdiff( l.mat, r.mat, diff );
    return sizeOfLargestConnectedComponent( diff );
}

// Get the pixel count of the largest connected component (blob)
double sizeOfLargestConnectedComponent( cv::Mat &differenceImage ) {
    ContourSet contours;
    cv::findContours( differenceImage, contours,
                      CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE );
    /*
     *TRACEAREA( maxAreaOfContours( contours ) )
     *#ifdef TRACE
     *cv::drawContours( *differenceImage, contours, -1, cv::Scalar( 255 ) );
     *cv::imshow( "contours", *differenceImage );
     *cv::waitKey( );
     *#endif
     */
    return maxAreaOfContours( contours );
}

double maxAreaOfContours( const ContourSet &contours ) {
    if( contours.empty() ) return 0.0;
    std::vector<double> areas; areas.reserve( contours.size() );
    std::transform( contours.begin(), contours.end(), std::back_inserter( areas ),
                    std::ptr_fun( absArea ) );
    return *std::max_element( areas.begin(), areas.end() );
}

double absArea( const Contour &mat ) {
    return std::abs( cv::contourArea( cv::Mat( mat ) ) );
}

