
#include "Databases.h"
#include<utility>
#include<string>
#include<limits>
#include "aslalgLevel.h" // This must go last - ml-types.h defines macros that cause trouble

int numberOfSequencesC( Pointer glossPtr, int glossLen ) {
    std::string gloss( (char *)glossPtr, glossLen );
    std::vector<std::shared_ptr<SignSeq> > seqShared = TDB->Sequences( gloss );
    return seqShared.size();
}

void getSequencesC( Pointer glossPtr, int glossLen, Pointer sequencePtrs ) {
    std::string gloss( (char *)glossPtr, glossLen );
    std::vector<std::shared_ptr<SignSeq> > seqShared = TDB->Sequences( gloss );
    std::transform( seqShared.begin(), seqShared.end(),
                    (SignSeq **)sequencePtrs,
                    std::mem_fun_ref( &std::shared_ptr<SignSeq>::get ) );
}

double distanceC( Pointer seqPtr, int start, int end ) {
    SignSeq *sequence = (SignSeq *)seqPtr;
    return sequence->Distance( std::make_pair( start, end ) );
}

double getMaxScoreC() { return std::numeric_limits<double>::max(); }
