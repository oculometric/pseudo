//
//  main.cpp
//  pseudo
//
//  Created by Javax on 09/08/2018.
//  Copyright © 2018 Javax Inc. All rights reserved.
//

#include <iostream>
#include <chrono>

using namespace std;

#include "Interpreter.hpp"

using namespace std;
using namespace chrono;

int main(int argc, const char * argv[]) {
	// Interpret the code we loaded.
	MemoryManager mm = *new MemoryManager();
	vector<Variable> variables;
	vector<Marker> markers;
#ifdef DEBUG
	string codeFile = "/Repository of Things/Coding/AIs/pseudo/pseudo/demo.pso";
	Interpreter inter = Interpreter(readEntireTextFile(codeFile), &mm, &variables, &markers);
#else
	Interpreter inter = Interpreter(readEntireTextFile(argv[1]), &mm, &variables, &markers);
#endif
	auto prepStart = high_resolution_clock::now();
	inter.prepare();
	auto prepEnd = high_resolution_clock::now();
	auto prepTimeMicroseconds = duration_cast<microseconds>(prepEnd-prepStart);
	
	auto interpretStart = high_resolution_clock::now();
	inter.interpret();
	auto interpretEnd = high_resolution_clock::now();
	auto interpretTimeMicroseconds = duration_cast<microseconds>(interpretEnd-interpretStart);
	
	cout << "Program exited without error." << endl;
	cout << "Preparation time was " << prepTimeMicroseconds.count() << "µs." << endl;
	cout << "Interpretation time was " << interpretTimeMicroseconds.count() << "µs, or about " << interpretTimeMicroseconds.count()/1000000 << "s." << endl;
	
	return 0;
}
