
#include <iostream>

#include "FrameDB.h"

#include "opencv/cv.h"
#include "opencv/highgui.h"


#include "plotter.h"
#include "cvsl.h"

using std::cerr;
using std::endl;

/*
 * Helper function to copy from an image point to a destination matrix.
 * Templated to allow different destination types
 */
template<typename MatType>
void CopyMat( Pointer img, MatType dst ) {
    if( dst.isContinuous() )
        memcpy( dst.datastart, img, dst.dataend - dst.datastart );
    else
        cerr << "Cannot copy image - destination is not continuous" << endl;
}

/*
 * Helper function to create a cv::Mat from the provided image data
 */
cv::Mat makeMat( Pointer img, int width, int height, int type ) {
    cv::Mat dst = cv::Mat::zeros( height, width, type );
    CopyMat( img, dst );
    return dst;
}

/*
 * Helper function to create a cv::Mat_<T> from the provided image data
 */
template<typename T>
cv::Mat_<T> makeMat( Pointer img, int width, int height ) {
    cv::Mat_<T> dst = cv::Mat_<T>::zeros( height, width );
    CopyMat( img, dst );
    return dst;
}

/*
 * Display an image with cv image type of 'type'. If wait is true, wait
 * for a key-press.
 */
void showImageC( Pointer img, int width, int height, int type, Bool wait ) {
    cv::imshow( "Image", makeMat( img, width, height, type ) );
    if( wait ) cv::waitKey();
    else cv::waitKey( 34 );
}

/*
 * Save an image with cv image type of 'type' to file fname
 */
void saveImageC( Pointer img, int width, int height, int type, char *fname, int fnameLen) {
    cv::imwrite( std::string( fname, fnameLen ), makeMat( img, width, height, type ),
                 std::vector<int>() );
}

/*
 * a video writer - only one video may be written at a time with this
 * implementation. Not a problem with mlton, but you'll want to fix this before
 * multi-threading
 */
static cv::VideoWriter *out = 0;

/*
 * Start a video-save session to file fname
 */
void videoSaveBeginC( int width, int height, int /*fourcc*/, double fps, Bool color,
                      Pointer fname, int fnameLen ) {
    if( out ) {
        cerr << "Output writer not properly cleaned up" << endl;
    }
    out = new cv::VideoWriter( std::string( (char*)fname, fnameLen ),
                               /*CV_FOURCC('P','I','M','1') */ -1, fps,
                               cv::Size( height, width ), color );
}

/*
 * Save a frame of video. videoSaveBeginC() must be called first
 */
void saveVideoC( Pointer img, int width, int height, int type ) {
    (*out) << makeMat( img, width, height, type );
}

/*
 * Finalize saving a video. videoSaveBeginC() must be called first, followed
 * by saveVideoC() calls for any frames
 */
void videoSaveEndC() {
    if( !out ) {
        cerr << "Output writer not created" << endl;
    }
    delete out;
    out = 0;
}

/*
 * Display a histogram. histImage is a normalized-histogram array
 */
void showNormalHistogramC( Pointer histImg, int width, int height ) {
    cv::Mat_<double> hist = makeMat<double>( histImg, width, height );
    plotter pl;
    pl.toPlot( hist );
    pl.plot();
}

