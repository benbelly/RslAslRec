
#include <algorithm>
#include <functional>
#include <stdlib.h>
#include "FrameDB.h"
#include "histograms.h"
#include "logging.h"


cv::Mat generateHandHistogram( Frame f, Contour c );
inline void NormalizeHistogram( Histogram &hist, double total );

static const int histSize = 32;

HistogramSet generateHandHistograms( Frame f, ContourSet cs ) {
    std::vector<cv::Mat> hs; hs.reserve( cs.size() );
    std::transform( cs.begin(), cs.end(), std::back_inserter( hs ),
                    std::bind1st( std::ptr_fun( generateHandHistogram ), f ) );
    return hs;
}

/*
 * Histograms in the paper are a bit of a mystery.
 *  The hand is a 200x200 pixel image.
 *  The histogram is a 32x32 pixel image
 *  The algorithm goes:
 *      hist[rows, cols] <= 0
 *      total = 0
 *      foreach( Point p in IMG )
 *          foreach( Point q in IMG from p )
 *              xD = |( p.x - q.x )| * 32 * 2 / 200   ?? Why "* 2"
 *              yD = |( p.y - q.y )| * 32 / 200
 *              xD = min( xD, 32 - 1 )
 *              yD = min( yD, 32 - 1 )
 *              ++hist[yD, xD]
 *              ++total
 *      foreach( float i in hist )
 *          hist[i] = sqrt( hist[i] / total )       ?? Why "sqrt"
 *
 * This file does not currently reflect that algorithm
 */
struct CountPoint {
    CountPoint( cv::Point pnt, Histogram &h, double &t,
                int width, int height ) :
        p( pnt ), hist( h ), total( t ), w( width ), h( height ) { }
    cv::Point p;
    Histogram &hist;
    double &total;
    int w, h;
    void operator()( cv::Point q ) {
        int hSize = hist.size().width; // assume a square
        int xD = abs( ( p.x - q.x ) ) * hSize * 2 / w,
            yD = abs( ( p.y - q.y ) ) * hSize / h;
        xD = std::min( xD, hSize - 1 );
        yD = std::min( yD, hSize - 1 );
        ++hist( yD, xD ); ++total;
    }
};

void CountPoints( Contour::iterator end, Histogram &hist, double &total,
                  int xSize, int ySize, Contour::iterator point ) {
    std::for_each( point, end, CountPoint( *point, hist, total, xSize, ySize ) );
}

inline cv::Mat generateHandHistogram( Frame f, Contour c ) {
    Histogram hist = Histogram::zeros( histSize, histSize );
    int w = f.size().width, h = f.size().height;
    double total = 0.0;
    Contour::iterator cbegin = c.begin(), cend = c.end();
    while( cbegin != cend )
        CountPoints( cend, hist, total, w, h, cbegin++ );
    NormalizeHistogram( hist, total );
    return hist;
}

inline void NormalizeHistogram( Histogram &hist, double total ) {
    Histogram::iterator hbegin = hist.begin(), hend = hist.end();
    while( hbegin != hend ) {
        *hbegin = sqrt( *hbegin / total );
        ++hbegin;
    }
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
