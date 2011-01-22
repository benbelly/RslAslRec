
#ifndef FEATUREFRAME_H__
#define FEATUREFRAME_H__
#include "histograms.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"

struct FeatureFrame {
    FeatureFrame( cv::Point tl, cv::Point br, cv::Mat dom, cv::Mat weak );
    cv::Point faceTopLeft, faceBottomRight;
    cv::Mat dom, weak;
    Histogram domHist, weakHist;
    cv::PCA domPCA, weakPCA;

    double distance( Histogram testDom, Histogram testWeak, const cv::Mat &icovar );
    Histogram GetDominantHistogram() { return domHist; }
    cv::Mat GetDominantPcaEigenValues() { return domPCA.eigenvalues; }
};

#endif
