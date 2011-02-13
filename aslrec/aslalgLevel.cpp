
#include "Databases.h"
#include "boost/shared_ptr.hpp"
#include "boost/mem_fn.hpp"
#include<utility>
#include<string>
#include<limits>
#include<iostream>
#include "aslalgLevel.h" // This must go last - ml-types.h defines macros that cause trouble

using std::cout;
using std::endl;

int numberOfSequencesC( Pointer glossPtr, int glossLen ) {
    std::string gloss( (char *)glossPtr, glossLen );
    std::vector<boost::shared_ptr<SignSeq> > seqShared = TDB->Sequences( gloss );
    return seqShared.size();
}

void getSequencesC( Pointer glossPtr, int glossLen, Pointer sequencePtrs ) {
    std::string gloss( (char *)glossPtr, glossLen );
    std::vector<boost::shared_ptr<SignSeq> > seqShared = TDB->Sequences( gloss );
    std::transform( seqShared.begin(), seqShared.end(),
                    (SignSeq **)sequencePtrs,
                    boost::mem_fn( &boost::shared_ptr<SignSeq>::get ) );
}

double getMaxScoreC() { return std::numeric_limits<double>::max(); }

int getNumberOfSignsC() { return TDB->NumGlosses(); }

int getSignLengthC( int i ) { return TDB->GetGloss( i ).size(); }

void getSignC( int i, Pointer dst ) {
    std::string sign( TDB->GetGloss( i ) );
    const char *cs = sign.c_str();
    memcpy( dst, cs, sign.size() ) ;
}

double distanceC( Pointer word, int wordLen, int start, int end ) {
    std::string gloss( (char *)word, wordLen );
    return TDB->Distance( gloss, start, end );
}
