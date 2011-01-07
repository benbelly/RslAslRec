#ifndef FRAMEDB_H__
#define FRAMEDB_H__

#include <map>
#include <iterator>
#include <functional>
#include <string>
#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "histograms.h"
#include "frame.h"

class FrameDB {
    public:
        FrameDB( FrameSet &frames );
        void findHands();

        struct FrameData {
            int id;
            Frame original,
                  skinMask,
                  gray,
                  SD,
                  boundary;
            ContourSet hands;
            CenterSet handCenters;
            HistogramSet histograms;
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
        FrameSet boundaries() const;
        int firstId() const;
        std::vector<int> ids() const;

        // Get individual frames
        Frame original( int i )          { return db[i].original; }
        Frame gray( int i )              { return db[i].gray; }
        Frame skin( int i )              { return db[i].skinMask; }
        Frame sd( int i )                { return db[i].SD; }
        Frame boundary( int i )          { return db[i].boundary; }
        HistogramSet histograms( int i ) { return db[i].histograms; }
        Frame histogramImg( int i );
        ContourSet hands( int i )        { return db[i].hands; }
        CenterSet handCenters( int i )       { return db[i].handCenters; }


        int size() { return db.size(); }
        // maximum number of hand candidates in all frames
        int maxHands();

        typedef std::map<int, FrameDB::FrameData> DBType;
        typedef DBType::value_type RowType;

    private:
        DBType db;

        int fourcc;
        double fps;
        double numFrames;

        FrameSet keyframes;
        const std::string videoFile;

        void findKeyframes();
        void makeSDs();

        FrameSet generateInitialSDs();
        FrameSet maskedSDs( FrameSet );

        typedef Frame (*Accessor)( RowType );
        FrameSet getItem( Accessor ) const;

        typedef void (*Setter)( RowType &, const Frame & );
        void setItem( Setter, FrameSet vals );
};

#endif
