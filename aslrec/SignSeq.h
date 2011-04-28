
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
                      const cv::Mat &dom, boost::shared_ptr<cv::Mat> weak );

        void AppendHistograms( std::list<Histogram> *hists ) const;

        double Distance( std::pair<int, int> interval, const cv::PCA &pca,
                         const cv::Mat &covar );

    private:
        typedef std::map<int, boost::shared_ptr<SignSeqScores> > MemoMap;
        MemoMap scoreMemos;
        typedef boost::shared_ptr<FeatureFrame> FramePtr;
        typedef std::vector<FramePtr> FrameList;
        FrameList frames;

        void Distance( SignSeqScores &scores, std::pair<int, int> interval,
                         const cv::PCA &pca, const cv::Mat &covar );
        void GenerateScoresForModelFrames( SignSeqScores &scores,
                                           std::pair<int, int> interval,
                                           const cv::PCA &pca, const cv::Mat &covar );
        void GeneratorScoresForModel( SignSeqScores &scores,
                                      std::pair<int, int> interval,
                                      int modelIndex, const cv::PCA &pca,
                                      const cv::Mat &covar );
        void GenerateScoresForTestFrame( SignSeqScores &scores,
                                         std::pair<int, int> interval,
                                         int modelIndex, int testIndex, const cv::PCA &pca,
                                         const cv::Mat &covar );

        typedef std::pair<cv::Mat, boost::shared_ptr<cv::Mat> > HandPair;
        void Cost( SignSeqScores &scores, std::pair<int, int> interval,
                   int modelIndex, int testIndex, int handIndex,
                   const HandPair &handPair,
                   const cv::PCA &pca, const cv::Mat &covar );

        double GetBestScoreForEnd( SignSeqScores &scores, int end );

        std::vector<HandPair> &GetHandPairs( int index, const cv::PCA &pca );
        std::vector<HandPair> makePairs( ProjectionSet &hands );
        double DistanceForPair( int modelIndex, int testIndex, int handIndex,
                                FramePtr, const HandPair &pair,
                                const cv::PCA &pca, const cv::Mat &covar );

};

#endif
