//
//  Interpreter.hpp
//  pseudo
//
//  Created by Javax on 10/08/2018.
//  Copyright © 2018 Javax Inc. All rights reserved.
//

#ifndef Interpreter_hpp
#define Interpreter_hpp

#include <string>
#include <vector>
#include "Token.hpp"

using namespace std;

class Interpreter {
private:
	string directInput;
	string preparedInput;
	bool isPrepared;
	
	vector<string> keywords = {"increment", "decrement", "repeat", "times", "set", "to", "input", "output", "from"};
	vector<char> ints = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	
	void interpretLine (string line);
	
	Token makeToken (string tokenInput);
	
public:
	Interpreter (string input);
	
	void prepare ();
	
	void interpret ();
};

#endif /* Interpreter_hpp */
