
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

class SignSeqScores;

class SignSeq {
    public:
        SignSeq();
        virtual ~SignSeq();

        int AddHands( cv::Point tl, cv::Point br,
                      const cv::Mat &dom, const cv::Mat &weak );

        double Distance( std::pair<int, int> interval );

    private:
        std::vector<std::shared_ptr<FeatureFrame> > hands;


        void GenerateScoresForModelFrames( SignSeqScores &scores,
                                           std::pair<int, int> interval );
        void GeneratorScoresForModel( SignSeqScores &scores,
                                      std::pair<int, int> interval,
                                      int modelIndex );
        void GenerateScoresForTestFrame( SignSeqScores &scores,
                                         int modelIndex, int testIndex );
        double GetBestScoreForEnd( SignSeqScores &scores, int end );

        std::vector<std::pair<int, int> > makePairs( HistogramSet &hands );
};

#endif
