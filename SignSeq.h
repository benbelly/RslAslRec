
#ifndef SIGNSEQ_H__
#define SIGNSEQ_H__

#include "frame.h"
#include "edgedetection.h"
#include "FeatureFrame.h"
#include "histograms.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include<vector>
#include<utility>
#include<memory>

class SignSeq {
    public:
        SignSeq();
        virtual ~SignSeq();

        int AddHands( cv::Point tl, cv::Point br,
                      const cv::Mat &dom, const cv::Mat &weak );

        double Distance( std::pair<int, int> interval );

    private:
        std::vector<std::shared_ptr<FeatureFrame> > hands;
        
};

#endif
