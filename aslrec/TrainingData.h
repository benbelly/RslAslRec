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


#ifndef TRAININGDATA_H__
#define TRAININGDATA_H__

#include "Gloss.h"
#include "frame.h"
#include "histograms.h"
#include <vector>
#include <list>
#include <string>
#include <boost/shared_ptr.hpp>

class TrainingData {
    public:
        TrainingData( const std::vector<boost::shared_ptr<Gloss> > &glosses );
         cv::PCA &GetPCA() ;
         cv::Mat &GetCovariance();
    private:
        std::vector<boost::shared_ptr<Gloss> > glosses;

        std::list<Histogram> allHists;
        std::list<Histogram> GetHists() ;

        std::map<std::string, std::list<Histogram> > wordHists;
        std::map<std::string, std::list<Histogram> > GetWordHists();

        cv::Mat MakeBigVector( const std::list<Histogram> &hists );

        cv::PCA pca;
        cv::PCA MakePCA() ;

        cv::Mat covar;
        cv::Mat MakeCovar() ;
};
#endif
