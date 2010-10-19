
#include "FrameDB.h"
#include "SignDB.h"

SignDB::SignDB() {
}

SignDB &SignDB::Instance() {
    static SignDB *sdb = 0;
    if( !sdb ) {
        sdb = new SignDB();
    }
    return *sdb;
}

FrameDB *SignDB::AddVideo( std::string sign, std::string videoFile ) {
    FrameSet vidFrames = loadFromVideo( videoFile );
    signs[sign] = srec( new FrameDB( vidFrames ) );
    return signs[sign].frameDB;
}
