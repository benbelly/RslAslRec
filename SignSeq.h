
#ifndef SIGNSEQ_H__
#define SIGNSEQ_H__

#include "frame.h"
#include "edgedetection.h"
#include "histograms.h"
#include<utility>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include<vector>
#include<utility>

class SignSeq {
    public:
        SignSeq();
        virtual ~SignSeq();

        typedef cv::Mat Hand;
        struct FeatureFrame {
            cv::Point faceTopLeft, faceBottomRight;
            Hand dom, weak;
            Histogram domHist, weakHist;
        };

        void AddHands( cv::Point tl, cv::Point br,
                       const cv::Mat &dom, const cv::Mat &weak );

    private:
        std::vector<FeatureFrame> hands;
        
};

#endif
