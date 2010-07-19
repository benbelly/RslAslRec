#ifndef FRAME_H__
#define FRAME_H__

#include <vector>
#include <memory>
#include "opencv/cv.h"
#include "opencv/highgui.h"

struct Frame {
    Frame( int i, cv::Size size, int type ) : id( i ), mat( size, type ) {}
    Frame( int i, cv::Mat &m ) : id( i ), mat( m ) {}
    Frame() : id( -1 ), mat() { }
    //Frame( int i, Frame::MatPtr m ) : id( i ), mat( m ) {}

    // Common accessors on cv::Mat
    cv::Size size() const { return mat.size(); }
    int type() const { return mat.type(); }
    int depth() const { return mat.depth(); }
    int channels() const { return mat.channels(); }

    int id; // frame index

    cv::Mat mat;
};
typedef std::vector<Frame> FrameSet;

Frame getFrame( int id );
int newFrame( Frame &f );
std::vector<int> framesToIds( FrameSet &fs );
FrameSet idsToFrames( std::vector<int> &ids );

struct CsAndGs {
    FrameSet colors;
    FrameSet grays;
};

typedef std::vector<cv::Point> Contour;
typedef std::vector<Contour> ContourSet;

///
// Utility functions that should probably go somewhere else
///

FrameSet gray8bitTogray16bit( const FrameSet &source );

template<typename ForwardIteratorA, typename ForwardIteratorB,
         typename Inserter, typename BinaryOp>
void Zip( ForwardIteratorA aBegin, ForwardIteratorA aEnd,
          ForwardIteratorB bBegin, ForwardIteratorB bEnd,
          Inserter ins, BinaryOp op ) {
    for( ; ( aBegin != aEnd ) && ( bBegin != bEnd ); ++aBegin, ++bBegin ) {
        *ins = op( *aBegin, *bBegin );
        ++ins;
    }
}

//Copy_if was dropped from the standard library by accident.
template<typename In, typename Out, typename Pred>
Out Copy_if(In first, In last, Out res, Pred Pr)
{
  while (first != last)
  {
    if (Pr(*first))
      *res++ = *first;
    ++first;
  }
  return res;
}
#endif

