
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

