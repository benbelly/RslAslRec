
#include "TrainDB.h"
#include "Databases.h"

#include<iostream>
using std::cerr;
using std::endl;

TrainDB::TrainDB() {
    TDB = this;
}

TrainDB::~TrainDB() {
}

SignSeq *TrainDB::NextSequenceForGloss( std::string g ) {
    //cerr << "Getting next sequence for " << g << endl;
    SignMap::iterator glossI = GlossIter( g );
    return (glossI->second)->NextSeq();
}

TrainDB::SignMap::iterator TrainDB::GlossIter( std::string g ) {
    SignMap::iterator forGloss = trainedSigns.find( g ),
                      end = trainedSigns.end();
    if( forGloss == end )
        forGloss = trainedSigns.insert( trainedSigns.begin(),
                                        std::make_pair( g, SignPtr( new Gloss( g ) ) ) );
    return forGloss;
}

int TrainDB::AddHandToList( std::shared_ptr<FeatureFrame> ff ) {
    static int index = 0;
    features[index] = ff;
    return index++;
}

int getId( std::pair<int, std::shared_ptr<FeatureFrame> > p ) { return p.first; }
std::vector<int> TrainDB::ids() {
    std::vector<int> is; is.reserve( features.size() );
    std::transform( features.begin(), features.end(), std::back_inserter( is ),
                    std::ptr_fun( getId ) );
    return is;
}
