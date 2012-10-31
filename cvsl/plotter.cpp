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

#include <vector>
#include "plotter.h"

const std::string plotter::XWIN( "xcairo" );

plotter::plotter() : just( 2 ), style( -2 ), zm( 0.0 ), zM( 0.1 ),
                     alt( 35.0 ), az( 30.0 ) {
}

void plotter::toPlot( cv::Mat_<double> h ) {
    hist = h;
    xm2d = -(hist.cols / 2); xM2d = hist.cols / 2;
    ym2d = -(hist.rows / 2); yM2d = hist.rows / 2;
    xm = 0; xM = hist.cols;
    ym = 0; yM = hist.rows;
    basex = 15; basey = 15; height = 25;
    PL().col0( 1 );
    PL().env( xm2d, xM2d, ym2d, yM2d, just, style );
    PL().w3d( basex, basey, height, xm, xM, ym, yM, zm, zM, alt, az );
    PL().box3( "bnt", "X", 0, 0,
               "bnt", "Y", 0, 0,
               "bnst", "Z", 1.0, 2.0 );
}

void plotter::plot() {
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

plstream &plotter::PL() {
    static plstream *pl = 0;
    if( !pl ) {
        pl = new plstream();
        pl->sdev( XWIN.c_str() );
        pl->init();
    }
    return *pl;
}
