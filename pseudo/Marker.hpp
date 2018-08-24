//
//  Marker.hpp
//  pseudo
//
//  Created by Javax on 24/08/2018.
//  Copyright © 2018 Javax Inc. All rights reserved.
//

#ifndef Marker_hpp
#define Marker_hpp

#include <string>

using namespace std;

class Marker {
public:
	string identifier;
	int lineReference;
	
	Marker (string id, int lnR);
};
#endif /* Marker_hpp */
