
#include "differenceImageFunctions.h"
#include "keyframedist.h"

FrameSet generateInitialSDs( const FrameSet &grayScales,
                             const FrameSet &keyframes ) {
    FrameSet SDs; SDs.reserve( grayScales.size() );
    std::transform( grayScales.begin(), grayScales.end(), std::back_inserter( SDs ),
                    std::bind1st( std::ptr_fun( avgDist2 ), keyframes ) );
    return SDs;
}
