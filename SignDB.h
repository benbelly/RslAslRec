#ifndef _SIGNDB_H__
#define _SIGNDB_H__

#include <map>
#include <string>

#include "frame.h"

class FrameDB;

class SignDB {
    public:
        SignDB &Instance();

        struct srec {
            FrameDB *frameDB;
            ContourSet left, right;
            cv::PCA pcaLeft, pcaRight;
            srec( FrameDB *f ) : frameDB( f ) {}
            srec() : frameDB( 0 ) { } // for map - do not use
        };

        typedef std::map<std::string, srec> SignSet;
        SignSet signs;

        FrameDB *AddVideo( std::string sign, std::string videoFile );
    private:
        SignDB();
};


#endif
