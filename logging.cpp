
#include "logging.h"

#ifdef TRACE
    void showNwait( char *name, Frame img ) {
        cv::imshow( name, img.mat );
        cv::waitKey();
    }
#else
    void showNwait( char *, Frame ) { }
#endif
