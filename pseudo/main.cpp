//
//  main.cpp
//  pseudo
//
//  Created by Javax on 09/08/2018.
//  Copyright © 2018 Javax Inc. All rights reserved.
//

#include <iostream>
#include "Interpreter.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
	// insert code here...
	//cout << "Hello, World!\n";
	string code = "increment [0];increment [0];increment [0];output [0];";
	
	Interpreter inter = Interpreter(code);
	inter.prepare();
	inter.interpret();
	
	return 0;
}
