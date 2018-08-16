//
//  Interpreter.cpp
//  pseudo
//
//  Created by Javax on 10/08/2018.
//  Copyright © 2018 Javax Inc. All rights reserved.
//

#include "Interpreter.hpp"

bool containsChar (vector<char> v, char c) {
	for (char cc : v) {
		if (cc == c) {
			return true;
		}
	}
	return false;
}

bool containsString (vector<string> v, string c) {
	for (string cc : v) {
		if (cc == c) {
			return true;
		}
	}
	return false;
}

int intFrom (string s) {
	int output = 0;
	int tenMultiplier = 1;
	for (int i = int(s.size()) - 1; i > -1; i--) {
		char character = s.at (i);
		int digit = 0;
		char ints[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
		while (ints[digit] != character) {
			digit++;
		}
		output += (tenMultiplier * digit);
		tenMultiplier *= 10;
	}
	return output;
}

Interpreter::Interpreter (string input) {
	directInput = input;
	isPrepared = false;
}

void Interpreter::prepare () {
	preparedInput = "";
	for (char c : directInput) {
		if (c == '\n') {
			preparedInput += ' ';
		} else {
			preparedInput += c;
		}
	}
	isPrepared = true;
}

void Interpreter::interpret () {
	if (isPrepared) {
		vector<string> lines;
		int lineLoc = 0;
		lines.push_back("");
		for (char c : preparedInput) {
			if (c == ';') {
				lineLoc++;
				lines.push_back("");
			} else {
				lines.at (lineLoc) += c;
			}
		}
	}
}

void Interpreter::interpretLine (string line) {
	vector<string> tokensString;
	int tokenLoc = 0;
	tokensString.push_back("");
	for (char c : line) {
		if (c != ' ') {
			tokenLoc++;
			tokensString.at(tokenLoc) += c;
		} else {
			tokensString.push_back("");
		}
	}
	
	vector<Token> tokens;
	for (string sToken : tokensString) {
		Token t = makeToken(sToken);
		if (t.type != TokenType::undefined) {
			tokens.push_back(t);
		}
	}
	
	// Interpret and execute
}

Token Interpreter::makeToken (string tokenInput) {
	if (tokenInput.at(0) == '\"') {                  // String literal
		string s = "";
		bool b = false;
		for (char c : tokenInput) {
			if (c != '\"') {
				s += c;
			} else {
				if (b) {
					break;
				}
				b = true;
			}
		}
		return Token (s, TokenType::stringLiteral);
	} else if (tokenInput.at (0) == '[') {           // Memory reference
		string s = "";
		bool b = false;
		for (char c : tokenInput) {
			if (c != ']') {
				s += c;
			} else {
				if (b) {
					break;
				}
				b = true;
			}
		}
		int i = intFrom(s);
		return Token ("", 0, false, i, "", TokenType::memoryReference);
	} else if (tokenInput.at (0) == '_') {           // Variable reference
		string s = "";
		bool b = false;
		for (char c : tokenInput) {
			if (c != '_') {
				s += c;
			} else {
				if (b) {
					break;
				}
				b = true;
			}
		}
		return Token (s, TokenType::variableReference);
	} else if (tokenInput.at (0) == '!') {           // Boolean literal
		string s = "";
		bool b = false;
		for (char c : tokenInput) {
			if (c != '!') {
				s += c;
			} else {
				if (b) {
					break;
				}
				b = true;
			}
		}
		if (s == "true") {
			return Token ("", 0, true, TokenType::boolLiteral);
		} else if (s == "false") {
			return Token ("", 0, false, TokenType::boolLiteral);
		}
		throw exception();
	} else if (containsChar (ints, tokenInput.at (0))) { // Integer literal
		int i = intFrom(tokenInput);
		return Token ("", i, false, 0, "", TokenType::intLiteral);
	} else if (containsString(keywords, tokenInput)) {   // Keyword
		
	} // Undefined, syntax error.
	throw exception();
	return Token();
}
