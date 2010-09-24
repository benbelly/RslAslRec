
#include <algorithm>
#include <functional>
#include <stdlib.h>
#include "FrameDB.h"
#include "histograms.h"
#include "logging.h"

static const unsigned int Tvert = 9, Thorz = 9;
typedef std::pair<cv::Range, cv::Range> RowColRange;


std::vector<cv::Mat> generateHandHistograms( Frame f, ContourSet cs ) {
    std::vector<cv::Mat> hs; hs.reserve( cs.size() );
    std::transform( cs.begin(), cs.end(), std::back_inserter( hs ),
                    std::bind1st( std::ptr_fun( generateHandHistogram ), f ) );
    return hs;
}

RowColRange bbox( cv::Point p, int hmax, int vmax ) {
    return RowColRange(
            cv::Range( std::max( 0, (int)(p.y - Tvert ) ),
                       std::min( vmax, (int)(p.y + Tvert ) ) ),
            cv::Range( std::max( 0, (int)(p.x - Thorz ) ),
                       std::min( hmax, (int)(p.x + Thorz ) ) ) );
}

double incrementBox( cv::Mat &hist, const RowColRange &box ) {
    // There should be some way to stick this in hist?
    //cv::Mat incedBox = hist( box.first, box.second ) + cv::Scalar( 1 );
    for( int y = box.first.start; y <= box.first.end; ++y ) {
        for( int x = box.second.start; x <= box.second.end; ++x ) {
            ++hist.at<double>( y, x );
        }
    }
    return ( box.first.end - box.first.start ) * ( box.second.end - box.second.start );
}

cv::Mat generateHandHistogram( Frame f, Contour c ) {
    cv::Mat hist = cv::Mat::zeros( f.size(), CV_64FC1 );
    cv::Point center( hist.cols / 2, hist.rows / 2 );
    double total = 0.0;
    Contour::iterator cbegin = c.begin(), cend = c.end();
    for( Contour::iterator p = cbegin; p != cend; ++p ) {
        total += incrementBox( hist, bbox( *p, hist.rows, hist.cols ) );
    }
    // Normalize to sum to 1
    double *histBegin = hist.ptr<double>(),
           *histEnd = histBegin + ( hist.rows * hist.cols );
    for( double *i = histBegin; i != histEnd; ++i ) *i = *i / total;
    return hist;
}

cv::Mat h2i( Frame f, HistogramSet &hs ) {
    cv::Mat dst = cv::Mat::zeros( f.size(), image_types::gray );
    for( HistogramSet::iterator i = hs.begin(); i != hs.end(); ++i ) {
        double max = *(std::max_element( i->begin<double>(), i->end<double>() ) );
        double scale = 255. / max;
        double *iBegin = i->ptr<double>(),
               *iEnd = iBegin + i->rows * i->cols;
        unsigned char *d = dst.data;
        while( iBegin != iEnd ) {
            if( *iBegin > 0.0 )
                *d = (unsigned char)(*iBegin * scale);
            ++iBegin; ++d;
        }
    }
    return dst;
}
