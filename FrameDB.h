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
        void findHands();

        struct FrameData {
            int id;
            Frame original,
                  skinMask,
                  gray,
                  SD;
            FrameData( int i, const cv::Mat &img );
            // Don't use. Provide for std::map
            FrameData() {}
        };

        // Access frames of various types
        FrameSet originals() const;
        FrameSet grays() const;
        FrameSet skins() const;
        FrameSet sds() const;
        FrameSet keys() const;
        std::vector<int> ids() const;

        // Get individual frames
        Frame original( int i ) { return db[i].original; }
        Frame gray( int i )     { return db[i].gray; }
        Frame skin( int i )     { return db[i].skinMask; }
        Frame sd( int i )       { return db[i].SD; }
        Frame key( int i )      { return keyframes[i]; }

        int size() { return db.size(); }

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
        void makeSDs();

        FrameSet generateInitialSDs();
        FrameSet maskedSDs( FrameSet );

        typedef Frame (*Accessor)( RowType );
        FrameSet getItem( Accessor ) const;

        typedef void (*Setter)( RowType &, const Frame & );
        void setItem( Setter, FrameSet vals );
};

extern FrameDB *FDB;

#endif
