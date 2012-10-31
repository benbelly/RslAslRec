
#ifndef LEVENSHTEIN_H__
#define LEVENSHTEIN_H__

/* Levenshtein distance algorithm made available without license
 * at http://www.merriampark.com/ldcpp.htm */
#include <vector>

// MLton stuff
#include "export.h"
#include "ml-types.h"

extern "C" {
    int levenshteinC( Pointer source, int sourceLen,
                      Pointer target, int targetLen );
};

int levenshtein( const std::vector<char> &source, const std::vector<char> &target );

#endif
