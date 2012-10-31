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


#include<utility>
#include<algorithm>
#include<functional>
#include<limits>
#include "Gloss.h"
#include "consts.h"
#include "boost/bind.hpp"
#include "boost/mem_fn.hpp"

Gloss::Gloss( std::string g ) : gloss( g ) {
}

Gloss::~Gloss() {}

SignSeq *Gloss::NextSeq() {
    boost::shared_ptr<SignSeq> ssp( new SignSeq() );
    trainedSeqs.push_back( ssp );
    return ssp.get();
}

void Gloss::AppendHistograms( std::list<Histogram> *hists ) const {
    std::for_each( trainedSeqs.begin(), trainedSeqs.end(),
                   boost::bind( &SignSeq::AppendHistograms, _1, hists ) );
}

double Gloss::Distance( const cv::PCA &pca, const cv::Mat &covar,
                        int start, int end ) {
    if( (end - start) > MAX_SIGN_LEN ) return std::numeric_limits<double>::max();
    std::vector<double> distances = GetScores( pca, covar, std::make_pair( start, end ) );
    std::vector<double>::iterator min = std::min_element( distances.begin(), distances.end() );
    if( min == distances.end() )
        return std::numeric_limits<double>::max();
    return *min;
}

std::vector<double> Gloss::GetScores( const cv::PCA &pca, const cv::Mat &mat,
                                      std::pair<int, int> interval ) {
    std::vector<double> distances; distances.reserve( trainedSeqs.size() );
    std::transform( trainedSeqs.begin(), trainedSeqs.end(),
                    std::back_inserter( distances ),
                    boost::bind( &SignSeq::Distance, _1, interval, pca, mat ) );
    return distances;
}


