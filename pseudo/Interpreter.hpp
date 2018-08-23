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
	MemoryManager *memoryManager;
	vector<Variable> *variables;
	int lineRef;
	vector<Token> currentLineTokens;
	vector<string> lines;
	
	const vector<void (*)(Interpreter* i)> keywordFuncPointer = {kincrement, kdecrement, krepeat, ktimes, kset, kto, kinput, koutput, kfrom, kwhile, kas};
	vector<char> ints = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	
	void interpretLine (string line);
	
	Token makeToken (string tokenInput);
	
	static void kincrement (Interpreter* i);
	static void kdecrement (Interpreter* i);
	static void krepeat (Interpreter* i);
	static void ktimes (Interpreter* i);
	static void kset (Interpreter* i);
	static void kto (Interpreter* i);
	static void kas (Interpreter* i);
	static void kinput (Interpreter* i);
	static void koutput (Interpreter* i);
	static void kfrom (Interpreter* i);
	static void kwhile (Interpreter* i);
	static void kcast (Interpreter* i);
	
	void outputInternal (vector<Token> toks, Interpreter *i, int start, int end, Token outputLocation);
	
	void assignMemoryValueInternal (Token assignee, Token newValue, Interpreter* i);
	void assignVariableValueInternal (string assigneeName, Token newValue, Interpreter* i);
	Variable* getVariable (string name);
public:
	static const vector<string> keywords () {
		return vector<string> ({"increment", "decrement", "repeat", "times", "set", "to", "input", "output", "from", "while", "kas"});
	}

	Interpreter (string input, MemoryManager *mm, vector<Variable> *vars);
	
	void prepare ();
	
	void interpret ();
};

string replaceAll (const string orig, const string search, const string replace);
string substring (string orig, int start, int end);
bool containsChar (vector<char> v, char c);
bool containsChar (string v, char c);
bool containsString (vector<string> v, string c);
int intFrom (string s);
int keywordIndex (Token t);
int findFirstKeywordOccurrence (Token t, vector<Token> vec);
vector<string> split (string input, char regex);
string readEntireTextFile (string path);

#endif /* Interpreter_hpp */
