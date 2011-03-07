
#include "TrainDB.h"
#include "Databases.h"
#include "TrainingData.h"

#include<iostream>
#include<memory>

using boost::shared_ptr;
using std::cerr;
using std::endl;

TrainDB::TrainDB() {
    TDB = this;
}

TrainDB::~TrainDB() {
}

double TrainDB::Distance( std::string word, int start, int end ) {
    static std::auto_ptr<TrainingData> data( new TrainingData( GlossPtrs() ) );
    return trainedSigns[word]->Distance( data->GetPCA(), data->GetCovariance(),
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
