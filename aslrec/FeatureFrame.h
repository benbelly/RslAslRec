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


#ifndef FEATUREFRAME_H__
#define FEATUREFRAME_H__
#include "histograms.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <memory>
#include <boost/shared_ptr.hpp>

struct FeatureFrame {
        FeatureFrame( cv::Point tl, cv::Point br, Contour dom,
                      boost::shared_ptr<Contour> weak );
        cv::Point faceTopLeft, faceBottomRight;
        Contour dom;
        boost::shared_ptr<Contour> weak;
        Histogram domHist;


        double distance( const cv::Mat &testDom,
                         const cv::PCA &pca, const cv::Mat &covar );

    private:
        const cv::Mat &GetDomProjection( const cv::PCA & );
        // Cache the histogram projections after first use
        std::auto_ptr<cv::Mat> projection;
};

#endif
