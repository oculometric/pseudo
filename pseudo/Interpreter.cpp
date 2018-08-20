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

int keywordIndex (Token t) {
	int ind = 0;
	for (string s : Interpreter::keywords()) {
		if (t.stringValue == s) {
			return ind;
		}
		ind++;
	}
	throw runtime_error ("Invalid keyword: '" + t.stringValue + "'.");
}

int findFirstKeywordOccurrence (Token t, vector<Token> vec) {
	int loc = 0;
	for (Token tc : vec) {
		if (tc.type == TokenType::keyword && tc.stringValue == t.stringValue) {
			return loc;
		}
		loc++;
	}
	return -1;
}

Interpreter::Interpreter (string input) {
	directInput = input;
	memoryManager = *new MemoryManager();
	isPrepared = false;
}

void Interpreter::prepare () {
	preparedInput = "";
	for (char c : directInput) {
		if (c == '\n') {
			preparedInput += ';';
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
		for (string line : lines) {
			if (line.size() > 1)
				interpretLine(line);
		}
	}
}

void Interpreter::interpretLine (string line) {
	vector<string> tokensString;
	int tokenLoc = 0;
	tokensString.push_back("");
	
	int charLoc = 0;
	
	for (char c : line) {
		if (c != ' ') {
			tokensString.at(tokenLoc) += c;
		} else {
			tokensString.push_back("");
			tokenLoc++;
		}
		charLoc++;
	}
	
	
	
	vector<Token> tokens;
	for (string sToken : tokensString) {
		if (sToken.size() > 1) {
			if (!(sToken.at (0) == '/' && sToken.at(1) == '/')) {
				Token t = makeToken(sToken);
				if (t.type != TokenType::undefined) {
					tokens.push_back(t);
				}
			} else {
				break;
			}
		}
	}
	
	this->currentLineTokens = tokens;
	if (tokens.size() > 0) {
		if (tokens.at(0).type == TokenType::keyword) {
			int loc = keywordIndex (tokens.at(0));
			keywordFuncPointer.at (loc) (this);
		} else {
			throw runtime_error("Expected keyword token, got '" + tokens.at(0).stringValue + "'.");
		}
	}
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
		return *new Token (s, 0, false, 0, "", TokenType::stringLiteral);
	} else if (tokenInput.at (0) == '[') {           // Memory reference
		string s = "";
		bool b = false;
		for (char c : tokenInput) {
			if (c != ']' && c != '[') {
				s += c;
			} else {
				if (b) {
					break;
				}
				b = true;
			}
		}
		int i = intFrom(s);
		return *new Token (tokenInput, 0, false, i, "", TokenType::memoryReference);
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
		return *new Token (tokenInput, 0, true, 0, s, TokenType::variableReference);
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
			return *new Token (tokenInput, 0, true, 0, "", TokenType::boolLiteral);
		} else if (s == "false") {
			return *new Token (tokenInput, 0, false, 0, "", TokenType::boolLiteral);
		}
		throw exception();
	} else if (containsChar (ints, tokenInput.at (0))) { // Integer literal
		int i = intFrom(tokenInput);
		return *new Token (tokenInput, i, false, 0, "", TokenType::intLiteral);
	} else if (containsString(keywords(), tokenInput)) {   // Keyword
		return *new Token (tokenInput, 0, false, 0, "", TokenType::keyword);
	} // Undefined, syntax error.
	throw runtime_error ("Invalid keyword: '" + tokenInput + "'.");
}

void Interpreter::kincrement(Interpreter* i) {
	Token nextToken = i->currentLineTokens.at (1);
	if (nextToken.type == TokenType::memoryReference) {
		Token toInc = i->memoryManager.getMemoryValue(nextToken.memoryReference);
		if (toInc.type == TokenType::undefined) {
			i->memoryManager.setMemoryValue(Token ("", 1, false, 0, "", TokenType::intLiteral), nextToken.memoryReference);
		} else if (toInc.type == TokenType::intLiteral) {
			i->memoryManager.setMemoryValue(Token ("", toInc.intValue + 1, false, 0, "", TokenType::intLiteral), nextToken.memoryReference);
		}
	} else if (nextToken.type == TokenType::variableReference) {
		Variable* v = i->getVariable(nextToken.variableReference);
		if (v != NULL) {
			if (v->type == Primitive::pint) {
				v->intValue += 1;
			}
		} else {
			throw runtime_error ("Reference to undeclared variable after 'increment' keyword.");
		}
	} else {
		throw runtime_error ("Invalid token after 'increment' keyword.");
	}
}

void Interpreter::kdecrement(Interpreter* i) {
	Token nextToken = i->currentLineTokens.at (1);
	if (nextToken.type == TokenType::memoryReference) {
		Token toInc = i->memoryManager.getMemoryValue(nextToken.memoryReference);
		if (toInc.type == TokenType::undefined) {
			i->memoryManager.setMemoryValue(Token ("", -1, false, 0, "", TokenType::intLiteral), nextToken.memoryReference);
		} else if (toInc.type == TokenType::intLiteral) {
			i->memoryManager.setMemoryValue(Token ("", toInc.intValue - 1, false, 0, "", TokenType::intLiteral), nextToken.memoryReference);
		}
	} else if (nextToken.type == TokenType::variableReference) {
		Variable* v = i->getVariable(nextToken.variableReference);
		if (v != NULL) {
			if (v->type == Primitive::pint) {
				v->intValue -= 1;
			}
		} else {
			throw runtime_error ("Reference to undeclared variable after 'decrement' keyword.");
		}
	} else {
		throw runtime_error ("Invalid token after 'decrement' keyword.");
	}
}

void Interpreter::kset(Interpreter* i) {
	int toLoc = findFirstKeywordOccurrence(*new Token ("to"), i->currentLineTokens);
	if (toLoc == -1) {
		throw runtime_error ("Could not locate 'to' keyword to assign new value.");
	}
	if (toLoc < 2) {
		throw runtime_error ("Keyword 'to' found instead of required reference value.");
	}
	if (toLoc == i->currentLineTokens.size()-1) {
		throw runtime_error ("No new assignment value found after 'to' token.");
	}
	vector<Token> assignees;
	for (int tmp = 1; tmp < toLoc; tmp++) {
		assignees.push_back(i->currentLineTokens.at(tmp));
	}
	Token newValue = i->currentLineTokens.at (toLoc + 1);
	
	// Assign new value to all assignees
	for (Token assignee : assignees) {
		
	}
}

void Interpreter::krepeat(Interpreter* i) {}
void Interpreter::kto(Interpreter* i) {}
void Interpreter::kwhile(Interpreter* i) {}
void Interpreter::kfrom(Interpreter* i) {}
void Interpreter::kinput(Interpreter* i) {}
void Interpreter::koutput(Interpreter* i) {
	// Handle any type of token.
	if (i->currentLineTokens.size() <= 1)
		throw runtime_error ("Expected data token after " + i->currentLineTokens.at (0).stringValue);
	for (int tokenRef = 1; tokenRef < i->currentLineTokens.size(); tokenRef++) {
		Token tok = i->currentLineTokens.at (tokenRef);
		if (tok.type == TokenType::memoryReference) {
			tok = i->memoryManager.getMemoryValue(tok.memoryReference);
		}
		
		switch (tok.type) {
			case keyword:
				throw runtime_error ("Found unexpected additional keyword: '" + tok.stringValue + "'.");
				break;
			case stringLiteral:
			case undefined:
				cout << tok.stringValue << endl;
				break;
			case intLiteral:
				cout << tok.intValue << endl;
				break;
			case boolLiteral:
				if (tok.boolValue) {
					cout << "true" << endl;
				} else {
					cout << "false" << endl;
				}
				break;
			case memoryReference:
				cout << "Oops?" << endl;
				break;
			case variableReference:
				Variable * v = i->getVariable(tok.variableReference);
				switch (v->type) {
					case pint:
						cout << v->intValue << endl;
						break;
					case pbool:
						cout << v->boolValue << endl;
						break;
					case pstring:
						cout << v->stringValue << endl;
						break;
				}
				break;
		}
	}
	
}
void Interpreter::ktimes(Interpreter* i) {}

Variable *Interpreter::getVariable(std::string name) { 
	for (int ref = 0; ref < variables.size(); ref++) {
		if (variables.at(ref).name == name) {
			Variable* v = &variables.at(ref);
			return v;
		}
	}
	return NULL;
}

