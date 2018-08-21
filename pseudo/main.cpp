//
//  main.cpp
//  pseudo
//
//  Created by Javax on 09/08/2018.
//  Copyright © 2018 Javax Inc. All rights reserved.
//

#include <iostream>
#include <fstream>

using namespace std;

string readEntireTextFile (string path) {
	string output;
	string line;
	
	ifstream file;
	file.open(path);
	//if (file.is_open()) {
		while (getline(file, line))
			output += line + "\n";
		file.close();
	//}
	
	return output;
}

#include "Interpreter.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
	// Load code from file 'code.pso'
	string code = "increment [0]; // This increments memory value 0.\n output [0]; // This prints the result.";
	string code2 = "increment [0]; // We make two values in memory;increment [0];increment [0];decrement [0];decrement [1]; // Here is the other;decrement [1];decrement [1];increment [1];output [0]; // Now we send them to the screen;output [1];";
	string code3 = "set [0] to 5; set [2] to \"Hello, World!\"; set [1] to !true!; increment [0]; output [0] [1] [2]; output \"We will now clear all the values in memory, and try setting one to a different value type.\"; set [0] to NULL; set [0] to \"And for my next trick?\"; output [0] \"There we go!\";";
	
	string code4 = readEntireTextFile("/Users/javaxprogrammer/Desktop/pseudo/pseudo/code.pso");
	
	
	
	// Interpret the code we loaded.
	Interpreter inter = Interpreter(code4);
	inter.prepare();
	inter.interpret();
	
	return 0;
}
