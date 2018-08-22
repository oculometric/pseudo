//
//  main.cpp
//  pseudo
//
//  Created by Javax on 09/08/2018.
//  Copyright © 2018 Javax Inc. All rights reserved.
//

#include <iostream>

using namespace std;

#include "Interpreter.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
	// Interpret the code we loaded.
	MemoryManager mm = *new MemoryManager();
	vector<Variable> variables;
#ifdef DEBUG
	string codeFile = "code5.pso";
	Interpreter inter = Interpreter(readEntireTextFile(codeFile), &mm, &variables);
#else
	Interpreter inter = Interpreter(readEntireTextFile(argv[1]), &mm, &variables);
#endif
	inter.prepare();
	inter.interpret();
	
	return 0;
}
