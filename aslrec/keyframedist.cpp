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

#include <vector>
#include <numeric>
#include "frame.h"
#include "keyframedist.h"
//#include "logging.h"
#include "consts.h"

#include "Databases.h"

unsigned char zeroOrMax( unsigned long in ) {
    return in ? 255 : 0;
}

Frame avgDist2( const FrameSet &keyFrames, const Frame f ) {
    const Frame src( FDB->skin( f.id ) );
    unsigned int size = src.mat.rows * src.mat.cols;
    unsigned long *diffSum = new unsigned long[ size ];

    FrameSet::const_iterator end = keyFrames.end();
    for( FrameSet::const_iterator i = keyFrames.begin(); i != end; ++i ) {
        Frame kf( FDB->skin( i->id ) );
        unsigned char *fi = src.mat.datastart, *ki = kf.mat.datastart;
        for(unsigned long *j = diffSum; j != diffSum + size; ++j, ++fi, ++ki ) {
            *j += ( *fi - *ki );
        }
    }

    for( unsigned long *j = diffSum; j != diffSum + size; ++j ) {
        *j = *j / ( keyFrames.size() - 1 );
    }

    Frame dst( src.id, src.size(), src.type() );
    std::transform( diffSum, diffSum + size, dst.mat.datastart,
                    std::ptr_fun( zeroOrMax ) );

    delete[] diffSum;
    return dst;
}

