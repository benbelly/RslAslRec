
#ifndef FEATUREFRAME_H__
#define FEATUREFRAME_H__
#include "histograms.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"

struct FeatureFrame {
    cv::Point faceTopLeft, faceBottomRight;
    cv::Mat dom, weak;
    Histogram domHist, weakHist;
};

#endif
