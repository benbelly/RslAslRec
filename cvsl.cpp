
#include <iostream>

#include "FrameDB.h"

#include "opencv/cv.h"
#include "opencv/highgui.h"

#include <plplot/plplot.h>
#include <plplot/plstream.h>

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

static const std::string XWIN( "xwin" );
struct plotter {
    plstream &PL() {
        static plstream *pl = 0;
        if( !pl ) {
            pl = new plstream();
            pl->sdev( XWIN.c_str() );
            pl->init();
        }
        return *pl;
    }

    cv::Mat_<double> hist;
    // env values
    PLFLT xm2d, xM2d, ym2d, yM2d;
    const PLINT just, style;
    // axis
    PLFLT xm, xM, ym, yM;
    const PLFLT zm, zM;
    // w3d vals
    PLINT basex, basey, height;
    const PLFLT alt, az;

    plotter() : just( 2 ), style( -2 ), zm( 0.0 ), zM( 1.0 ),
                alt( 30.0 ), az( 30.0 ) {
    }
    void toPlot( cv::Mat_<double> h ) {
        hist = h;
        xm2d = -(hist.cols / 2); xM2d = hist.cols / 2;
        ym2d = -(hist.rows / 2); yM2d = hist.rows / 2;
        xm = 0; xM = hist.cols;
        ym = 0; yM = hist.rows;
        basex = 100; basey = 100; height = 50;
        PL().col0( 1 );
        PL().env( xm2d, xM2d, ym2d, yM2d, just, style );
        PL().w3d( basex, basey, height, xm, xM, ym, yM, zm, zM, alt, az );
        PL().box3( "bnt", "X", 0, 0,
                 "bnt", "Y", 0, 0,
                 "bnt", "Z", 0, 0 );
    }
    void plot() {
        std::vector<PLFLT> xs, ys, zs;
        unsigned int size = hist.rows * hist.cols;
        xs.reserve( size ); ys.reserve( size ); zs.reserve( size );
        for( int y = 0; y < hist.rows; ++y )
            for( int x = 0; x < hist.cols; ++x )
                if( hist( y, x ) > 0.0 ) {
                    xs.push_back( (PLFLT)x ); ys.push_back( (PLFLT)y );
                    zs.push_back( (PLFLT)hist( y, x ) );
                }
        PL().col0( 15 ); // white
        PL().poin3( ys.size(), &xs[0], &ys[0], &zs[0], 1 );
    }
};

void showNormalHistogram( Pointer histImg, int width, int height ) {
    cv::Mat_<double> hist = makeMat<double>( histImg, width, height );
    plotter pl;
    pl.toPlot( hist );
    pl.plot();
}

