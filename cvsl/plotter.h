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


#ifndef PLOTTER_H__
#define PLOTTER_H__

#include <string>
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
