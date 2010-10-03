
#include <algorithm>
#include <functional>
#include <stdlib.h>
#include <fstream>
#include "FrameDB.h"
#include "histograms.h"
#include "logging.h"

cv::Mat generateHandHistogram( Frame f, Contour c );

HistogramSet generateHandHistograms( Frame f, ContourSet cs ) {
    std::vector<cv::Mat> hs; hs.reserve( cs.size() );
    std::transform( cs.begin(), cs.end(), std::back_inserter( hs ),
                    std::bind1st( std::ptr_fun( generateHandHistogram ), f ) );
    return hs;
}

typedef cv::Mat_<double> Histogram;

struct CountPoint {
    CountPoint( cv::Point c, Histogram &h, double &t ) : center( c ), hist( h ), total( t ) { }
    cv::Point center;
    Histogram &hist;
    double &total;
    void operator()( cv::Point p ) {
        ++hist( vD( p ), hD( p ) );
        ++total;
    }
    int D( int l, int r ) { return abs( l - r ); }
    int vD( cv::Point p ) { return D( center.y, p.y ); }
    int hD( cv::Point p ) { return D( center.x, p.x ); }
};

inline cv::Mat generateHandHistogram( Frame f, Contour c ) {
    cv::Point center( f.size().width / 2, f.size().height / 2 );
    Histogram hist = Histogram::zeros( center.y + 1, center.x + 1 );
    double total = 0.0;
    std::for_each( c.begin(), c.end(), CountPoint( center, hist, total ) );
    // Normalize
    Histogram::iterator hbegin = hist.begin(), hend = hist.end();
    while( hbegin != hend ) {
        *hbegin = *hbegin / total;
        ++hbegin;
    }
    return hist;
}

void addImages( Histogram &dst, Histogram src ) {
    Histogram::iterator db = dst.begin(), de = dst.end(),
                        sb = src.begin(), se = src.end();
    while( db != de ) *db++ += *sb++;
}

cv::Mat h2i( Frame f, HistogramSet &hs ) {
    unsigned int width = hs[0].cols, height = hs[0].rows;
    Histogram histImg = Histogram::zeros( height, width );
    std::for_each( hs.begin(), hs.end(),
                   std::bind1st( std::ptr_fun( addImages ), histImg ) );
    return histImg;
}
