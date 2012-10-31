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


#include "HandPairCollection.h"

HandPairCollection::HandPairCollection( const cv::PCA &pca,
                                        const ContourSet &singles ) {
    makePairs( pca, singles );
}

void HandPairCollection::makePairs( const cv::PCA &pca,
                                    const ContourSet &singles ) {
    int size = singles.size();
    hands.reserve( size * size + 2 ); centers.reserve( size * size + 2 );
    for( int dom = 0; dom < size; ++dom ) {
        for( int weak = 0; weak < size; ++weak ) {
            Projection proj;
            CenterPair centerp;
            if( weak != dom ) {
                proj = pca.project( flattenHistogram(
                            generateHandHistogram( singles[dom], &singles[weak]) ) );
                centerp = std::make_pair( ::center( singles[dom] ),
                                          new CenterPoint( ::center( singles[weak] ) ) );
            }
            else {
                proj = pca.project(
                        flattenHistogram( generateHandHistogram( singles[dom], (Contour *)0 ) ));
                centerp = std::make_pair( ::center( singles[dom] ), (CenterPoint *)0 );
            }
            hands.push_back( proj );
            centers.push_back( centerp );
        }
    }
}

