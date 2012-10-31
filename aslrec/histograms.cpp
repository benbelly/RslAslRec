/**
    Copyright 2011 Ben Holm

    This file is part of RslAslRec.

    RslAslRec is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RslAslRec is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RslAslRec.  If not, see <http://www.gnu.org/licenses/>.
**/


#include <algorithm>
#include <functional>
#include <stdlib.h>
#include <boost/bind.hpp>
#include "FrameDB.h"
#include "histograms.h"
#include "consts.h"

inline void NormalizeHistogram( Histogram &hist, double total );

static const int histSize = 32;

HistogramSet generateHandHistograms( const ContourSet cs ) {
    HistogramSet hs; hs.reserve( cs.size() );
    std::transform( cs.begin(), cs.end(), std::back_inserter( hs ),
                    boost::bind( generateHandHistogram, _1, (Contour *)0 ) );
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
 *          hist[i] = sqrt( hist[i] / total )
 *
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

void CountPoints( Contour::const_iterator end, Histogram &hist, double &total,
                  int xSize, int ySize, Contour::const_iterator point ) {
    std::for_each( point, end, CountPoint( *point, hist, total, xSize, ySize ) );
}

Histogram generateHandHistogram( const Contour hand1, const Contour *hand2 ) {
    Histogram hist = Histogram::zeros( histSize, histSize );
    int w = WIDTH, h = HEIGHT;
    double total = 0.0;
    Contour::const_iterator cbegin = hand1.begin(), cend = hand1.end();
    while( cbegin != cend )
        CountPoints( cend, hist, total, w, h, cbegin++ );
    if( hand2 ) {
        cbegin = hand2->begin(), cend = hand2->end();
        while( cbegin != cend )
            CountPoints( cend, hist, total, w, h, cbegin++ );
    }
    NormalizeHistogram( hist, total );
    return hist;
}

inline void NormalizeHistogram( Histogram &hist, double total ) {
    Histogram::iterator hbegin = hist.begin(), hend = hist.end();
    while( hbegin != hend ) {
        *hbegin = total > 0 ? sqrt( *hbegin / total ) : 0.0;
        ++hbegin;
    }
}

void addImages( Histogram &dst, Histogram src ) {
    Histogram::iterator db = dst.begin(), de = dst.end(),
                        sb = src.begin(), se = src.end();
    while( db != de ) *db++ += *sb++;
}

Histogram flattenHistogram( const Histogram &h ) {
    Histogram flat = Histogram::zeros( 1, h.rows * h.cols );
    memcpy( flat.data, h.data, h.rows * h.cols * sizeof( double ) );
    return flat;
}

cv::Mat h2i( Frame, HistogramSet &hs ) {
    unsigned int width = hs[0].cols, height = hs[0].rows;
    Histogram histImg = Histogram::zeros( height, width );
    std::for_each( hs.begin(), hs.end(),
                   std::bind1st( std::ptr_fun( addImages ), histImg ) );
    return histImg;
}
