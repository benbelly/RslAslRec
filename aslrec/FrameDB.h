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

#ifndef FRAMEDB_H__
#define FRAMEDB_H__

#include "histograms.h"
#include "frame.h"
#include "HandPairCollection.h"

#include <map>
#include <iterator>
#include <functional>
#include <string>
#include <boost/shared_ptr.hpp>
#include "opencv/cv.h"
#include "opencv/highgui.h"

class FrameDB {
    public:
        FrameDB( FrameSet &frames );
        virtual ~FrameDB();

        std::vector<double> findKeyframes( const unsigned int T1 );

        void setSingleSD( Frame sd );
        void setAllSDs( FrameSet &sds );
        void setBoundary( Frame b );
        void initialSDs();
        void skinmaskSDs();
        void edgeAndMask();
        void removeSmalls();

        void findHands();

        struct FrameData {
            int id;
            Frame original,
                  skinMask,
                  gray,
                  SD,
                  boundary;
            ContourSet hands;
            boost::shared_ptr<HandPairCollection> handPairs;
            CenterSet handCenters;
            HistogramSet histograms;
            ProjectionSet projections;
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
        int lastId() const;
        std::vector<int> ids() const;

        // Get individual frames
        Frame original( int i )          { return db[i].original; }
        Frame gray( int i )              { return db[i].gray; }
        Frame skin( int i )              { return db[i].skinMask; }
        Frame sd( int i )                { return db[i].SD; }
        Frame boundary( int i )          { return db[i].boundary; }
        HistogramSet histograms( int i ) { return db[i].histograms; }
        ProjectionSet projections( int i, const cv::PCA &pca );
        Frame histogramImg( int i );
        ContourSet hands( int i )        { return db[i].hands; }
        CenterSet handCenters( int i )   { return db[i].handCenters; }
        HandPairCollection handPairs( int i, const cv::PCA &pca );


        int size() { return db.size(); }
        // maximum number of hand candidates in all frames
        int maxHands();

        typedef std::map<int, FrameDB::FrameData> DBType;
        typedef DBType::value_type RowType;

    private:
        DBType db;

        int fourcc;
        double fps;

        FrameSet keyframes;
        const std::string videoFile;

        void findKeyframes( const unsigned int T1, std::vector<double> &diffs );
        void makeSDs();

        FrameSet maskedSDs( FrameSet );

        typedef Frame (*Accessor)( RowType );
        FrameSet getItem( Accessor ) const;

        typedef void (*Setter)( RowType &, const Frame & );
        void setItem( Setter, FrameSet vals );
};

#endif
