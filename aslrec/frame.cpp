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


#include "frame.h"
#include <map>
#include <algorithm>
#include <functional>
#include <boost/bind.hpp>

FrameSet loadFromFiles( std::vector<std::pair<std::string, int> > &filesAndFrames ) {
    FrameSet frames; frames.reserve( filesAndFrames.size() );
    std::vector<std::pair<std::string, int> >::iterator begin = filesAndFrames.begin(),
        end = filesAndFrames.end();
    while( begin != end ) {
        cv::Mat mat = cv::imread( begin->first );
        frames.push_back( Frame( begin->second, mat ) );
        ++begin;
    }
    return frames;
}

FrameSet loadFromVideo( std::string videoFile ) {
    cv::VideoCapture cap( videoFile );
    int numFrames = cap.get( CV_CAP_PROP_FRAME_COUNT );
    FrameSet frames; frames.reserve( numFrames );

    int i = 0;
    cv::Mat img;
    while( cap.grab() ) {
        cap.retrieve( img );
        cv::Mat copy( img.size(), img.type() );
        img.copyTo( copy );
        frames.push_back( Frame( i++, copy ) );
    }
    return frames;
}

FrameSet loadFromByteArray( int *ids, char *arr, int count,
                            int width, int height, int type ) {
    FrameSet frames; frames.reserve( count );
    for( int i = 0; i < count; ++i ) {
        int id = ids[i];
        cv::Mat img = cv::Mat::zeros( height, width, type );
        int bytes = height * width * img.elemSize1();
        memcpy( img.data, arr + ( i * bytes ), bytes );
        frames.push_back( Frame( id, img ) );
    }
    return frames;
}

Frame convertTo16bit( const Frame &src ) {
    Frame dest( src.id, src.size(), image_types::big_gray );
    src.mat.convertTo( dest.mat, image_types::big_gray );
    return dest;
}

FrameSet gray8bitTogray16bit( const FrameSet &src ) {
    FrameSet bigs; bigs.reserve( src.size() );
    std::transform( src.begin(), src.end(), std::back_inserter( bigs ),
                    boost::bind( convertTo16bit, _1 ) );
    return bigs;
}

int Frame::Id() const { return id; }
cv::Size Frame::size() const { return mat.size(); }
int Frame::type() const { return mat.type(); }
int Frame::depth() const { return mat.depth(); }
int Frame::channels() const { return mat.channels(); }
