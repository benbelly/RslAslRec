
#include "HandPairCollection.h"

HandPairCollection::HandPairCollection( const ProjectionSet &projections,
                                        const ContourSet &singles ) {
    makePairs( projections, singles );
}

void HandPairCollection::makePairs( const ProjectionSet &projections,
                                    const ContourSet &singles ) {
    int size = singles.size();
    hands.reserve( size * size + 2 ); centers.reserve( size * size + 2 );
    for( int dom = 0; dom < size; ++dom ) {
        for( int weak = 0; weak < size; ++weak ) {
            if( weak != dom ) {
                hands.push_back( std::make_pair( projections[dom],
                                                 new Histogram( projections[weak] ) ) );
                centers.push_back( std::make_pair( ::center( singles[dom] ),
                                                   new CenterPoint( ::center( singles[weak] ) ) ) );
            }
            else {
                hands.push_back( std::make_pair( projections[dom], (Histogram *)0 ) );
                centers.push_back( std::make_pair( ::center( singles[dom] ),
                                                   (CenterPoint *)0 ) );
            }
        }
    }
}

