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

#ifndef KEYFRAMESELECT_H__
#define KEYFRAMESELECT_H__

#include "frame.h"

// Functor to find keyframes
// This requires grayscale images
struct AccumKeyframes {
    AccumKeyframes( const unsigned int t1,
                    std::vector<double> &ds,
                    FrameSet &kfs ) : T1( t1 ),
                                      keyframes( kfs ),
                                      diffs( ds ) {}
    const unsigned int T1;
    FrameSet &keyframes;
    std::vector<double> &diffs;
    Frame operator() ( Frame lastKey, Frame next );
};
#endif
