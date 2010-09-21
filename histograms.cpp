
#include <algorithm>
#include <functional>
#include <stdlib.h>
#include "FrameDB.h"
#include "histograms.h"

std::vector<cv::Mat> generateHandHistograms( Frame f, ContourSet cs ) {
    std::vector<cv::Mat> hs; hs.reserve( cs.size() );
    std::transform( cs.begin(), cs.end(), std::back_inserter( hs ),
                    std::bind1st( std::ptr_fun( generateHandHistogram ), f ) );
    return hs;
}

static const unsigned int Tvert = 7, Thorz = 7;
bool inTh( cv::Point center, cv::Point bin, cv::Point p );
bool inTv( cv::Point center, cv::Point bin, cv::Point p );

inline bool pointLess( cv::Point l, cv::Point r ) {
    if( l.y < r.y ) return true;
    if( l.y > r.y ) return false;
    if( l.x < r.x ) return true;
    return false;
}

/*
 * for b:bins
 *  for p:points
 *      if( dv( c, p ) < Tv[b] and
 *          dh( c, p ) < Th[b] )
 *              ++hist[b]
 */
cv::Mat generateHandHistogram( Frame f, Contour c ) {
    cv::Mat hist = cv::Mat::zeros( f.size(), CV_64FC1 );
    Contour::iterator cbegin = c.begin(), cend = c.end();
    cv::Point center( hist.cols / 2, hist.rows / 2 );
    double total = 0.0;
    for( Contour::iterator p = cbegin; p != cend; ++p ) {
        for( int y = std::max( (int)(p->y - (Tvert + 1)), 0 ) ;
             y < std::min( (int)(p->y + (Tvert + 1)), hist.cols ); ++y ) {
            for( int x = std::max( (int)(p->x - (Thorz + 1)), 0 );
                 x < std::min( (int)(p->x + (Thorz + 1)), hist.rows ); ++x ) {
                cv::Point bin( x, y );
                if( inTv( center, bin, *p ) && inTh( center, bin, *p ) ) {
                    ++hist.at<double>( bin );
                    ++total;
                }
            }
        }
    }
    double *histBegin = hist.ptr<double>();
    double *histEnd = histBegin + ( hist.rows * hist.cols );
    for( double *i = histBegin; i != histEnd; ++i ) {
        *i = *i / total;
    }
    return hist;
}

int vd( cv::Point a, cv::Point b ) {
    return ( a.y - b.y );
}

int hd( cv::Point a, cv::Point b ) {
    return ( a.x - b.x );
}


bool inTv( cv::Point center, cv::Point bin, cv::Point p ) {
    int bDist = vd( center, bin ),
        pDist = vd( center, p );
    int min = bDist - Tvert, max = bDist + Tvert;
    return min < pDist &&
           pDist < max;
}

bool inTh( cv::Point center, cv::Point bin, cv::Point p ) {
    int bDist = hd( center, bin ),
        pDist = hd( center, p );
    int min = bDist - Thorz, max = bDist + Thorz;
    return min < pDist &&
           pDist < max;
}

cv::Mat h2i( Frame f, HistogramSet &hs ) {
    cv::Mat dst = cv::Mat::zeros( f.size(), image_types::gray );
    for( HistogramSet::iterator i = hs.begin(); i != hs.end(); ++i ) {
        double max = *(std::max_element( i->begin<double>(), i->end<double>() ) );
        double scale = 255 / max;
        double *iBegin = i->ptr<double>(),
                        *iEnd = iBegin + i->rows * i->cols;
        unsigned char *d = dst.data;
        while( iBegin != iEnd ) {
            *d = (unsigned char)(*iBegin * scale);
            ++iBegin; ++d;
        }
    }
    return dst;
}
