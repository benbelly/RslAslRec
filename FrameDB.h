#ifndef FRAMEDB_H__
#define FRAMEDB_H__

#include <map>
#include <iterator>
#include <functional>
#include <string>
#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "frame.h"

using std::string;

class FrameDB {
    public:
        FrameDB( string videoFile );

        struct FrameData {
            int id;
            Frame original,
                  gray,
                  skinMask,
                  SD;
            FrameData( int i, Frame &o, Frame &g ) :
                id( i ), original( o ), gray( g ) { }
            // Don't use. Provide for std::map
            FrameData() {}
        };

        // Access frames of various types
        FrameSet originals() const;
        FrameSet grays() const;
        FrameSet skins() const;
        FrameSet sds() const;

        // Get individual frames
        Frame original( int i ) { return db[i].original; }
        Frame gray( int i )     { return db[i].gray; }
        Frame skin( int i )     { return db[i].skinMask; }
        Frame sd( int i )       { return db[i].SD; }

        // Mask a frame with the corresponding skinmask
        Frame skinmask( Frame &f );

        typedef std::map<int, FrameDB::FrameData> DBType;
        typedef DBType::value_type RowType;

    private:
        DBType db;
        FrameSet keyframes;
        const string videoFile;

        void loadVideo();
        void findKeyframes();

        typedef Frame (*Accessor)( RowType );
        FrameSet getItem( Accessor ) const;
};

#endif
