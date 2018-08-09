//
//  main.cpp
//  pseudo
//
//  Created by Javax on 09/08/2018.
//  Copyright © 2018 Javax Inc. All rights reserved.
//

#include <iostream>
#include "Variable.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
	// insert code here...
	//cout << "Hello, World!\n";
	Variable v = *new Variable ("Testy", Primitive::pstring, "Hello, World!");
	cout << "Variable is called '" << v.name << "', is of type '" << v.type << "', and has value '" << v.stringValue << "'.\n";
	return 0;
}
