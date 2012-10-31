/**
    Copyright 2011 Ben Holm

    This file is part of RslAslRec.

    RslAslRec is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RslAslRec is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with RslAslRec.  If not, see <http://www.gnu.org/licenses/>.
**/


#ifndef SIGNSEQ_H__
#define SIGNSEQ_H__

#include "frame.h"
#include "edgedetection.h"
#include "FeatureFrame.h"
#include "histograms.h"
#include "HandPairCollection.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include<vector>
#include<list>
#include<utility>
#include "boost/shared_ptr.hpp"

class SignSeqScores;

// Memoization helpers
struct MemoIndex {
   int model, test, hand;
};
inline bool operator<( const MemoIndex &l, const MemoIndex &r ) {
    return l.model < r.model ||
           ( l.model == r.model && l.test < r.test ) ||
           ( l.model == r.model && l.test == r.test && l.hand < r.hand );
}
typedef std::map<MemoIndex, double> ScoreHistory;

class SignSeq {
    public:
        SignSeq();
        virtual ~SignSeq();

        int AddHands( cv::Point tl, cv::Point br,
                      const Contour &dom, boost::shared_ptr<Contour> weak );

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

        void Cost( SignSeqScores &scores, std::pair<int, int> interval,
                   int modelIndex, int testIndex, int handIndex,
                   const HandPairCollection::HandPair &handPair,
                   const cv::PCA &pca, const cv::Mat &covar );

        double GetBestScoreForEnd( SignSeqScores &scores, int end );

        double DistanceForPair( FramePtr, const HandPairCollection::HandPair &pair,
                                const cv::PCA &pca, const cv::Mat &covar );
        ScoreHistory scoreHistory;
};

#endif
