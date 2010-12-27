
#ifndef SIGNSEQ_H__
#define SIGNSEQ_H__

#include "frame.h"
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
        typedef std::pair<Hand, Hand> HandPair;

        void AddHands( const cv::Mat &dom, const cv::Mat &weak );

    private:
        std::vector<HandPair> hands;
        
};

#endif
