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

#ifndef ASLALGLEVELS_H__
#define ASLALGLEVELS_H__

#include "export.h"
#include "ml-types.h"

extern "C" {
    int numberOfSequencesC( Pointer glossPtr, int glossLen );
    void getSequencesC( Pointer glossPtr, int glossLen, Pointer sequencePtrs );
    double getMaxScoreC();
    int getNumberOfSignsC();
    int getSignLengthC( int i );
    void getSignC( int i, Pointer dst );

    void loadScoresC( Pointer fname, int fnameLen );
    double distanceC( Pointer word, int wordLen, int start, int end );
    void dumpScoresC( Pointer fname, int fnameLen );
}

#endif
