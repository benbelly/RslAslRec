
#ifndef PLOTTER_H__
#define PLOTTER_H__

#include <plplot/plplot.h>
#include <plplot/plstream.h>
#include <opencv/cv.h>

class plotter {

    public:
        plotter();
        void toPlot( cv::Mat_<double> h );
        void plot();

    private:

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

        plstream &PL();
        static const std::string XWIN;
};

#endif
