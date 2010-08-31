
#include "frame.h"
#include <map>
#include <algorithm>

Frame convertTo16bit( const Frame src ) {
    Frame dst( src.id, src.size(), image_types::big_gray );
    src.mat.convertTo( dst.mat, image_types::big_gray );
    return dst;
}

FrameSet gray8bitTogray16bit( const FrameSet &src ) {
    FrameSet bigs; bigs.reserve( src.size() );
    std::transform( src.begin(), src.end(), std::back_inserter( bigs ),
                    std::ptr_fun( convertTo16bit ) );
    return bigs;
}

int Frame::Id() const { return id; }
cv::Size Frame::size() const { return mat.size(); }
int Frame::type() const { return mat.type(); }
int Frame::depth() const { return mat.depth(); }
int Frame::channels() const { return mat.channels(); }
