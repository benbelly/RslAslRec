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


#ifndef GLOSS_H__
#define GLOSS_H__

#include "SignSeq.h"
#include "boost/shared_ptr.hpp"
#include<string>
#include<vector>
#include<list>

class Gloss {
    public:
        Gloss( std::string g );
        virtual ~Gloss();

        // Add another sequence to this gloss
        SignSeq *NextSeq();

        // Get the sequences for this gloss
        std::vector<boost::shared_ptr<SignSeq> > Sequences() const { return trainedSeqs; }

        void AppendHistograms( std::list<Histogram> *hists ) const;

        // Find the distance from this gloss to the frame interval
        double Distance( const cv::PCA &pca, const cv::Mat &covar,
                         int start, int end );

        const std::string gloss;
    private:

        // Use shared_ptr because they get passed to the database and even
        // back to the SML client
        typedef std::vector<boost::shared_ptr<SignSeq> > SeqList;
        SeqList trainedSeqs;

        std::vector<double> GetScores( const cv::PCA &pca, const cv::Mat &covar,
                                       std::pair<int, int> interval );

};

#endif
