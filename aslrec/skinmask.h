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


#ifndef SKINMASK_H__
#define SKINMASK_H__

#include <vector>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "frame.h"

Frame generateSkinMask( Frame frame );
FrameSet generateSkinMasks( FrameSet &frames );

Frame maskFrame( Frame &f, Frame &sk );
#endif
