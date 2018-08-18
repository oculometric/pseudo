//
//  Interpreter.hpp
//  pseudo
//
//  Created by Javax on 10/08/2018.
//  Copyright © 2018 Javax Inc. All rights reserved.
//

#ifndef Interpreter_hpp
#define Interpreter_hpp

#include "Token.hpp"
#include "Variable.hpp"
#include "MemoryManager.hpp"

using namespace std;

class Interpreter {
private:
	string directInput;
	string preparedInput;
	bool isPrepared;
	MemoryManager memoryManager;
	vector<Variable> variables;
	
	vector<Token> currentLineTokens;
	
	const vector<void (*)(Interpreter* i)> keywordFuncPointer = {kincrement, kdecrement, krepeat, ktimes, kset, kto, kinput, koutput, kfrom, kwhile};
	vector<char> ints = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	
	void interpretLine (string line);
	
	Token makeToken (string tokenInput);
	
	static void kincrement (Interpreter* i);
	static void kdecrement (Interpreter* i);
	static void krepeat (Interpreter* i);
	static void ktimes (Interpreter* i);
	static void kset (Interpreter* i);
	static void kto (Interpreter* i);
	static void kinput (Interpreter* i);
	static void koutput (Interpreter* i);
	static void kfrom (Interpreter* i);
	static void kwhile (Interpreter* i);
	
	Variable* getVariable (string name);
public:
	static const vector<string> keywords () {
		return vector<string> ({"increment", "decrement", "repeat", "times", "set", "to", "input", "output", "from", "while"});
	}

	Interpreter (string input);
	
	void prepare ();
	
	void interpret ();
};

#endif /* Interpreter_hpp */
