//
//  Variable.cpp
//  pseudo
//
//  Created by Javax on 09/08/2018.
//  Copyright © 2018 Javax Inc. All rights reserved.
//

#include "Variable.hpp"

Variable::Variable (string n, Primitive t, string sv, int iv, bool bv) {
	name = n;
	type = t;
	stringValue = sv;
	intValue = iv;
	boolValue = bv;
}

void Variable::displaySummary() {
	cout << "Variable named: " << name << ". Values: String - " << stringValue << ", Int - " << intValue << ", Bool - " << boolValue << ".\n";
}
