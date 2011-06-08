
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
