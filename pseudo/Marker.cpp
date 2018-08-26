//
//  Marker.cpp
//  pseudo
//
//  Created by Javax on 24/08/2018.
//  Copyright © 2018 Javax Inc. All rights reserved.
//

#include "Marker.hpp"


Marker::Marker(std::string id, int lnR) { 
    identifier = id;
	lineReference = lnR;
	
	isFunction = false;
}

Marker::Marker(std::string id, int lnR, int eLnR) {
	identifier = id;
	lineReference = lnR;
	
	isFunction = true;
	endLineReference = eLnR;
}
