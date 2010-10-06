
#include <iostream>

#include "FrameDB.h"

#include "opencv/cv.h"
#include "opencv/highgui.h"


#include "plotter.h"
#include "cvsl.h"

using std::cerr;
using std::endl;

template<typename MatType>
void CopyMat( Pointer img, MatType dst ) {
    if( dst.isContinuous() )
        memcpy( dst.datastart, img, dst.dataend - dst.datastart );
    else
        cerr << "Cannot copy image" << endl;
}

cv::Mat makeMat( Pointer img, int width, int height, int type ) {
    cv::Mat dst = cv::Mat::zeros( height, width, type );
    CopyMat( img, dst );
    return dst;
}

template<typename T>
cv::Mat_<T> makeMat( Pointer img, int width, int height ) {
    cv::Mat_<T> dst = cv::Mat_<T>::zeros( height, width );
    CopyMat( img, dst );
    return dst;
}

void showImageC( Pointer img, int width, int height, int type, Bool wait ) {
    cv::imshow( "Image", makeMat( img, width, height, type ) );
    if( wait ) cv::waitKey();
    else cv::waitKey( 34 );
}

void saveImageC( Pointer img, int width, int height, int type, char *fname, int fnameLen) {
    cv::imwrite( std::string( fname, fnameLen ), makeMat( img, width, height, type ),
                 std::vector<int>() );
}


static cv::VideoWriter *out = 0;
void videoSaveBeginC( int width, int height, int fourcc, double fps, Bool color,
                      Pointer fname, int fnameLen ) {
    if( out ) {
        cerr << "Output writer not properly cleaned up" << endl;
    }
    out = new cv::VideoWriter( std::string( (char*)fname, fnameLen ),
                               /*CV_FOURCC('P','I','M','1') */ -1, fps,
                               cv::Size( height, width ), color );
}

void saveVideoC( Pointer img, int width, int height, int type ) {
    (*out) << makeMat( img, width, height, type );
}

void videoSaveEndC() {
    if( !out ) {
        cerr << "Output writer not created" << endl;
    }
    delete out;
    out = 0;
}

void showNormalHistogram( Pointer histImg, int width, int height ) {
    cv::Mat_<double> hist = makeMat<double>( histImg, width, height );
    plotter pl;
    pl.toPlot( hist );
    pl.plot();
}

