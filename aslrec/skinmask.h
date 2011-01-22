
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
