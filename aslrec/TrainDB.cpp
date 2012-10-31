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


#include "TrainDB.h"
#include "Databases.h"
#include "TrainingData.h"

#include<iostream>
#include<memory>

using boost::shared_ptr;
using std::cerr;
using std::endl;

TrainDB::TrainDB() : data( (TrainingData *)0 ) {
    TDB = this;
}

TrainDB::~TrainDB() {
}

double TrainDB::Distance( std::string word, int start, int end ) {
    return trainedSigns[word]->Distance( Data().GetPCA(), Data().GetCovariance(),
                                         start, end );
}

SignSeq *TrainDB::NextSequenceForGloss( std::string g ) {
    //cerr << "Getting next sequence for " << g << endl;
    GlossMap::iterator glossI = GlossIter( g );
    return (glossI->second)->NextSeq();
}

TrainDB::GlossMap::iterator TrainDB::GlossIter( std::string g ) {
    GlossMap::iterator forGloss = trainedSigns.find( g ),
                       end = trainedSigns.end();
    if( forGloss == end )
        forGloss = trainedSigns.insert( trainedSigns.begin(),
                                        std::make_pair( g, GlossPtr( new Gloss( g ) ) ) );
    return forGloss;
}

int TrainDB::AddHandToList( shared_ptr<FeatureFrame> ff ) {
    static int index = 0;
    features[index] = ff;
    return index++;
}

int getId( std::pair<int, shared_ptr<FeatureFrame> > p ) { return p.first; }
std::string getTrainedId( std::pair<std::string, shared_ptr<Gloss> > p ) { return p.first; }
shared_ptr<Gloss> getGlossPtr( std::pair<std::string, shared_ptr<Gloss> > p ) { return p.second; }

std::vector<int> TrainDB::ids() {
    std::vector<int> is; is.reserve( features.size() );
    std::transform( features.begin(), features.end(), std::back_inserter( is ),
                    std::ptr_fun( getId ) );
    return is;
}

std::vector<std::string> TrainDB::glosses() const {
    if( glossesVec.empty() ) {
        glossesVec.reserve( trainedSigns.size() );
        std::transform( trainedSigns.begin(), trainedSigns.end(),
                        std::back_inserter( glossesVec ), std::ptr_fun( getTrainedId ) );
    }
    return glossesVec;
}

std::vector<boost::shared_ptr<Gloss> > TrainDB::GlossPtrs() const {
    std::vector<GlossPtr> ptrs; ptrs.reserve( trainedSigns.size() );
    std::transform( trainedSigns.begin(), trainedSigns.end(),
                    std::back_inserter( ptrs ), std::ptr_fun( getGlossPtr ) );
    return ptrs;
}
