
#ifndef SIGNSEQ_H__
#define SIGNSEQ_H__

#include "frame.h"
#include "edgedetection.h"
#include "FeatureFrame.h"
#include "histograms.h"
#include "eigens.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include<vector>
#include<utility>
#include "boost/shared_ptr.hpp"

class SignSeqScores;

class SignSeq {
    public:
        SignSeq();
        virtual ~SignSeq();

        int AddHands( cv::Point tl, cv::Point br,
                      const cv::Mat &dom, const cv::Mat &weak );

        void CollectEigenValues( std::vector<cv::Mat> &samples );

        double Distance( std::pair<int, int> interval, const cv::Mat &icovar );

    private:
        std::vector<boost::shared_ptr<FeatureFrame> > hands;


        void GenerateScoresForModelFrames( SignSeqScores &scores,
                                           std::pair<int, int> interval,
                                           const cv::Mat &icovar );
        void GeneratorScoresForModel( SignSeqScores &scores,
                                      std::pair<int, int> interval,
                                      int modelIndex, const cv::Mat &icovar );
        void GenerateScoresForTestFrame( SignSeqScores &scores,
                                         int modelIndex, int testIndex, const cv::Mat &icovar );
        double GetBestScoreForEnd( SignSeqScores &scores, int end );

        std::vector<std::pair<int, int> > makePairs( HistogramSet &hands );
};

#endif
