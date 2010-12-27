
#include "Gloss.h"

Gloss::Gloss( std::string g ) : gloss( g ) {
}

Gloss::~Gloss() {}

SignSeq *Gloss::NextSeq() {
    std::shared_ptr<SignSeq> ssp( new SignSeq() );
    trainedSeqs.push_back( ssp );
    return ssp.get();
}
