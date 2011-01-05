#ifndef ASLALGLEVELS_H__
#define ASLALGLEVELS_H__

#include "export.h"
#include "ml-types.h"

extern "C" {
    int numberOfSequencesC( Pointer glossPtr, int glossLen );
    void getSequencesC( Pointer glossPtr, int glossLen, Pointer sequencePtrs );
    double distanceC( Pointer sequence, int start, int end );
    double getMaxScoreC();
}

#endif
