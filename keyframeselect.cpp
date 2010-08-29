

#include <numeric>
#include <algorithm>
#include <iterator>
#include <memory>
#include <functional>

#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "keyframeselect.h"
#include "consts.h"
#include "logging.h"
#include "utility.h"

#include "FrameDB.h"

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

unsigned char bitOr( unsigned char a, unsigned char b ) { return a | b; }

cv::Mat orMasks( cv::Mat &a, cv::Mat &b ) {
    cv::Mat dst = cv::Mat::zeros( a.size(), a.type() );
    Zip( a.datastart, a.dataend, b.datastart, b.dataend,
         dst.datastart, std::ptr_fun( bitOr ) );
    return dst;
}

double calculateDiff( const Frame &l, const Frame &r ) {
    // Diff only the 'valid' pixels - skin pixels
    cv::Mat lvalid = cv::Mat::zeros( l.size(), l.type() ),
            rvalid = cv::Mat::zeros( r.size(), r.type() ),
            diff( l.size(), l.type() );
    l.mat.copyTo( lvalid, FDB->skin( l.id ).mat );
    r.mat.copyTo( rvalid, FDB->skin( r.id ).mat );
    cv::absdiff( lvalid, rvalid, diff );
    return sizeOfLargestConnectedComponent( diff );
}

// Get the pixel count of the largest connected component (blob)
double sizeOfLargestConnectedComponent( cv::Mat &differenceImage ) {
    ContourSet contours;
    cv::findContours( differenceImage, contours,
                      CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE );
    /*
     TRACEAREA( maxAreaOfContours( contours ) )
     *#ifdef TRACE
     *cv::Mat out = cv::Mat::zeros( differenceImage.size(), differenceImage.type() );
     *cv::drawContours( out, contours, -1, cv::Scalar( 255 ) );
     *cv::imshow( "contours", out );
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

