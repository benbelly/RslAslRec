
#include<utility>
#include<algorithm>
#include<functional>
#include<limits>
#include "Gloss.h"

Gloss::Gloss( std::string g ) : gloss( g ) {
}

Gloss::~Gloss() {}

SignSeq *Gloss::NextSeq() {
    std::shared_ptr<SignSeq> ssp( new SignSeq() );
    trainedSeqs.push_back( ssp );
    return ssp.get();
}

std::vector<double> Gloss::GetScores( std::pair<int, int> interval ) {
    // Silly C++ - turn the shared_ptr into regular pointers for use 
    // with the std algorithms
    // TODO: I'm using C++0x, so maybe std::bind will work
    std::vector<SignSeq *> seqPtrs; seqPtrs.reserve( trainedSeqs.size() );
    std::transform( trainedSeqs.begin(), trainedSeqs.end(),
                    std::back_inserter( seqPtrs ),
                    std::mem_fun_ref( &std::shared_ptr<SignSeq>::get ) );
    std::vector<double> distances; distances.reserve( seqPtrs.size() );
    std::transform( seqPtrs.begin(), seqPtrs.end(),
                    std::back_inserter( distances ),
                    std::bind2nd( std::mem_fun( &SignSeq::Distance ), interval ) );
    return distances;
}

double Gloss::Distance( int start, int end ) {
    std::vector<double> distances = GetScores( std::make_pair( start, end ) );
    std::vector<double>::iterator min = std::min_element( distances.begin(), distances.end() );
    if( min == distances.end() )
        return std::numeric_limits<double>::max();
    return *min;
}
