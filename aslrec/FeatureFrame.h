
#ifndef FEATUREFRAME_H__
#define FEATUREFRAME_H__
#include "histograms.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <memory>
#include <boost/shared_ptr.hpp>

struct FeatureFrame {
        FeatureFrame( cv::Point tl, cv::Point br, cv::Mat dom, cv::Mat weak );
        cv::Point faceTopLeft, faceBottomRight;
        cv::Mat dom, weak;
        Histogram domHist;
        std::auto_ptr<Histogram> weakHist;


        double distance( const Histogram testDom,
                         const cv::PCA &pca, const cv::Mat &covar );
        double distance( const Histogram testDom, Histogram testWeak,
                         const cv::PCA &pca, const cv::Mat &covar );

    private:
        const cv::Mat &GetDomProjection( const cv::PCA & );
        const cv::Mat &GetWeakProjection( const cv::PCA & );
        // Cache the histogram projections after first use
        std::auto_ptr<cv::Mat> domProject, weakProject;
};

#endif
