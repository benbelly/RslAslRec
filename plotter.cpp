#include "plotter.h"

const std::string plotter::XWIN( "xwin" );

plotter::plotter() : just( 2 ), style( -2 ), zm( 0.0 ), zM( 1.0 ),
                     alt( 30.0 ), az( 30.0 ) {
}

void plotter::toPlot( cv::Mat_<double> h ) {
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
