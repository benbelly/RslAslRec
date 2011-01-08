
#ifndef GLOSS_H__
#define GLOSS_H__

#include "SignSeq.h"
#include "boost/shared_ptr.hpp"
#include<string>
#include<vector>

class Gloss {
    public:
        Gloss( std::string g );
        virtual ~Gloss();

        // Add another sequence to this gloss
        SignSeq *NextSeq();

        // Get the sequences for this gloss
        std::vector<boost::shared_ptr<SignSeq> > Sequences() const { return trainedSeqs; }

        // Find the distance from this gloss to the frame interval
        double Distance( int start, int end );

    private:
        std::string gloss;

        // Use shared_ptr because they get passed to the database and even
        // back to the SML client
        typedef std::vector<boost::shared_ptr<SignSeq> > SeqList;
        SeqList trainedSeqs;

        std::vector<double> GetScores( std::pair<int, int> interval );

        const cv::Mat &GetICovarMatrix();
        void CalcICovarianceMatrix();
        cv::Mat icovarianceMatrix;
};

#endif
