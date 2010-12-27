
#include "TrainDB.h"

#include<iostream>
using std::cerr;
using std::endl;

TrainDB *TDB;

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
