
#include <algorithm>
#include <functional>
#include <stdlib.h>
#include "FrameDB.h"
#include "histograms.h"
#include "logging.h"

std::vector<cv::Mat> generateHandHistograms( Frame f, ContourSet cs ) {
    std::vector<cv::Mat> hs; hs.reserve( cs.size() );
    std::transform( cs.begin(), cs.end(), std::back_inserter( hs ),
                    std::bind1st( std::ptr_fun( generateHandHistogram ), f ) );
    return hs;
}

static const unsigned int Tvert = 7, Thorz = 7;

std::pair< cv::Range, cv::Range> bbox( cv::Point p, int hmax, int vmax ) {
    return std::pair<cv::Range, cv::Range>(
            cv::Range( std::max( 0, (int)(p.y - Tvert ) ),
                       std::min( vmax, (int)(p.y + Tvert ) ) ),
            cv::Range( std::max( 0, (int)(p.x - Thorz ) ),
                       std::min( hmax, (int)(p.x + Thorz ) ) ) );
                       
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
    cv::Point center( hist.cols / 2, hist.rows / 2 );
    double total = 0.0;
    Contour::iterator cbegin = c.begin(), cend = c.end();
    for( Contour::iterator p = cbegin; p != cend; ++p ) {
        std::pair<cv::Range, cv::Range> box = bbox( *p, hist.rows, hist.cols );
        // There should be some way to stick this in hist?
        //cv::Mat incedBox = hist( box.first, box.second ) + cv::Scalar( 1 );

        for( int y = box.first.start; y <= box.first.end; ++y ) {
            for( int x = box.second.start; x <= box.second.end; ++x ) {
                cv::Point bin( x, y );
                // no need to check that we're in bounds here - box is defined
                // to be in bounds
                ++hist.at<double>( bin );
                ++total;
            }
        }
    }
    double *histBegin = hist.ptr<double>();
    double *histEnd = histBegin + ( hist.rows * hist.cols );
    if( total > 0.0 ) { // in case nothing was found?
        for( double *i = histBegin; i != histEnd; ++i ) {
            *i = *i / total;
        }
    }
    else {
        cerr << "Failed to place any points in histogram" << endl;
    }
    return hist;
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
