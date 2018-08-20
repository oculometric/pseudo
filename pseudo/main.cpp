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
	// Load code from file 'code.pso'
	string code = "increment [0]; // This increments memory value 0.\n output [0]; // This prints the result.";
	string code2 = "increment [0]; // We make two values in memory;increment [0];increment [0];decrement [0];decrement [1]; // Here is the other;decrement [1];decrement [1];increment [1];output [0]; // Now we send them to the screen;output [1];";
	
	// Interpret the code we loaded.
	Interpreter inter = Interpreter(code2);
	inter.prepare();
	inter.interpret();
	
	return 0;
}
