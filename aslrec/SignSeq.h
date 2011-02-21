
#ifndef SIGNSEQ_H__
#define SIGNSEQ_H__

#include "frame.h"
#include "edgedetection.h"
#include "FeatureFrame.h"
#include "histograms.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include<vector>
#include<list>
#include<utility>
#include "boost/shared_ptr.hpp"

class SignSeqScores;

class SignSeq {
    public:
        SignSeq();
        virtual ~SignSeq();

        int AddHands( cv::Point tl, cv::Point br,
                      const cv::Mat &dom, const cv::Mat &weak );

        void AppendHistograms( std::list<Histogram> *hists ) const;

        double Distance( std::pair<int, int> interval, const cv::PCA &pca,
                         const cv::Mat &covar );

    private:
        typedef boost::shared_ptr<FeatureFrame> FramePtr;
        typedef std::vector<FramePtr> FrameList;
        FrameList frames;

        void GenerateScoresForModelFrames( SignSeqScores &scores,
                                           std::pair<int, int> interval,
                                           const cv::PCA &pca, const cv::Mat &covar );
        void GeneratorScoresForModel( SignSeqScores &scores,
                                      std::pair<int, int> interval,
                                      int modelIndex, const cv::PCA &pca,
                                      const cv::Mat &covar );
        void GenerateScoresForTestFrame( SignSeqScores &scores,
                                         int modelIndex, int testIndex, const cv::PCA &pca,
                                         const cv::Mat &covar );
        double GetBestScoreForEnd( SignSeqScores &scores, int end );

        typedef std::pair<cv::Mat, boost::shared_ptr<cv::Mat> > HandPair;
        std::vector<HandPair> makePairs( ProjectionSet &hands );
        double DistanceForPair( FramePtr, HandPair &pair, const cv::PCA &pca, const cv::Mat &covar );
};

#endif
