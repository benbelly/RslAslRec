#ifndef LOGGING_H__
#define LOGGING_H__

#include <string>
#include <sstream>
#include <iostream>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "frame.h"

void showNwait( char *name, Frame img );

#ifdef TRACE

using std::cerr;
using std::endl;
using std::cout;

#define TRACEDIFFS( diffs ) {\
    std::ostringstream os;\
    os << "output/out" << diff.f1.id << ".png";\
    cv::imwrite( os.str(), diff.diff );\
}

#define TRACECOUNT( item, count ) std::cerr << item << " = " << count << std::endl;

#define TRACEAREA( a ) TRACECOUNT( "area", a )

#else

#define TRACEDIFFS( diffs )
#define TRACECOUNT( item, count )
#define TRACEAREA( a )

#endif // if trace

#endif
