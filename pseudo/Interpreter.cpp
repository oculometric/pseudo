//
//  Interpreter.cpp
//  pseudo
//
//  Created by Javax on 10/08/2018.
//  Copyright © 2018 Javax Inc. All rights reserved.
//

#include <fstream>

#include "Interpreter.hpp"

string replaceAll (const string orig, const string search, const string replace) {
	if (search.size() > orig.size() || replace.size() > orig.size()) {
		throw runtime_error ("Search string cannot be longer than original string.");
	}
	
	string output = orig;
	
	string::size_type n = 0;
	
	while ((n = output.find(search, n)) != string::npos) {
		output.replace(n, search.size(), replace);
		n += replace.size();
	}
	
	return output;
}

string substring (string orig, int start, int end) {
	string newStr;
	for (int i = start; i <= end; i++) {
		newStr += orig.at(i);
	}
	return newStr;
}

bool containsChar (vector<char> v, char c) {
	for (char cc : v) {
		if (cc == c) {
			return true;
		}
	}
	return false;
}

bool containsChar (string v, char c) {
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

vector<string> split (string input, char regex) {
	vector<string> ret;
	int loc = 0;
	ret.push_back("");
	
	int charLoc = 0;
	
	for (char c : input) {
		if (c != regex) {
			ret.at(loc) += c;
		} else {
			ret.push_back("");
			loc++;
		}
		charLoc++;
	}
	
	return ret;
}

string readEntireTextFile (string path) {
	string output;
	string line;
	
	ifstream file;
	file.open(path);
	
	string last3 = substring(path, int(path.size())-4, int (path.size()-1));
	
	if (file.is_open() && last3 == ".pso") {
		while (getline(file, line))
			output += line + "\n";
		file.close();
	} else {
		throw runtime_error ("Specified file '" + path + "' cannot be opened.");
	}
	
	return output;
}

Variable *Interpreter::getVariable(std::string name) {
	for (int ref = 0; ref < variables->size(); ref++) {
		if (variables->at(ref).name == name) {
			Variable* v = &variables->at(ref);
			return v;
		}
	}
	return NULL;
}

Interpreter::Interpreter (string input, MemoryManager *mm, vector<Variable> *vars) {
	directInput = input;
	memoryManager = mm;
	variables = vars;
	isPrepared = false;
}

void Interpreter::prepare () {
	preparedInput = "";
	for (char c : directInput) {
		if (c == '\n') {
			preparedInput += ';';
		} else if (c == '\t') {
		} else {
			preparedInput += c;
		}
	}
	
	string tmp = preparedInput;
	preparedInput = "";
	
	vector<string> parts = split (tmp, '\"');
	
	for (int loc = 0; loc < parts.size(); loc++) {
			if (loc % 2 != 0) {
				if (parts.at (loc).size() > 0)
					preparedInput += "\"" + replaceAll(parts.at(loc), " ", "\\_") + "\"";
			} else {
				preparedInput += parts.at(loc);
			}
	}
	
	lineRef = 0;
	isPrepared = true;
}

void Interpreter::interpret () {
	if (isPrepared) {
		lines = {};
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
		while (lineRef < lines.size()) {
			string line = lines.at(lineRef);
			if (line.size() > 1)
				interpretLine(line);
			lineRef++;
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
		if (sToken.size() > 0) {
			if (!(sToken.size() > 1 && sToken.at (0) == '/' && sToken.at(1) == '/')) {
				Token t = makeToken(sToken);
				if (t.type == TokenType::stringLiteral) {t.stringValue = replaceAll(t.stringValue, "\\_", " ");}
				tokens.push_back(t);
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
	} else if (tokenInput == "NULL") {
		return *new Token ();
	} else if (tokenInput == "{") {
		return *new Token (tokenInput);
	} else if (tokenInput == "}") {
		return *new Token (tokenInput);
	} // Undefined, syntax error.
	throw runtime_error ("Invalid keyword: '" + tokenInput + "'.");
}

void Interpreter::kincrement(Interpreter* i) {
	Token nextToken = i->currentLineTokens.at (1);
	if (nextToken.type == TokenType::memoryReference) {
		Token toInc = i->memoryManager->getMemoryValue(nextToken.memoryReference);
		if (toInc.type == TokenType::undefined) {
			i->memoryManager->setMemoryValue(Token ("", 1, false, 0, "", TokenType::intLiteral), nextToken.memoryReference);
		} else if (toInc.type == TokenType::intLiteral) {
			i->memoryManager->setMemoryValue(Token ("", toInc.intValue + 1, false, 0, "", TokenType::intLiteral), nextToken.memoryReference);
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
		Token toInc = i->memoryManager->getMemoryValue(nextToken.memoryReference);
		if (toInc.type == TokenType::undefined) {
			i->memoryManager->setMemoryValue(Token ("", -1, false, 0, "", TokenType::intLiteral), nextToken.memoryReference);
		} else if (toInc.type == TokenType::intLiteral) {
			i->memoryManager->setMemoryValue(Token ("", toInc.intValue - 1, false, 0, "", TokenType::intLiteral), nextToken.memoryReference);
		}
	} else if (nextToken.type == TokenType::variableReference) {
		Variable* v = i->getVariable(nextToken.variableReference);
		if (v != NULL) {
			if (v->type == Primitive::pint) {
				v->intValue -= 1;
			}
		} else {
			throw runtime_error ("Reference to undeclared variable '" + nextToken.variableReference + "' after 'decrement' keyword.");
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
		if (assignee.type == TokenType::memoryReference) {
			i->assignMemoryValueInternal(assignee, newValue, i);
		} else if (assignee.type == TokenType::variableReference) {
			i->assignVariableValueInternal(assignee.variableReference, newValue, i);
		} else {throw runtime_error ("Token '" + assignee.stringValue + "' after 'set' keyword is not assignable.");}
	}
}

void Interpreter::assignVariableValueInternal (string assigneeName, Token newValue, Interpreter* i) {
	Variable * assignee = i->getVariable(assigneeName);
	Token realNewMemoryValue = i->memoryManager->getMemoryValue(newValue.memoryReference);
	Variable * realNewVariableValue = i->getVariable(newValue.variableReference);
	if (assignee == NULL) {
		i->variables->push_back(*new Variable (assigneeName, Primitive::pundefined, "", 0, false));
		assignee = i->getVariable(assigneeName);
	}
	if ((assignee->type == Primitive::pundefined) || (assignee->type == newValue.type) || (assignee->type == realNewMemoryValue.type) || (realNewVariableValue != NULL && assignee->type == realNewVariableValue->type)) {
		switch (newValue.type) {
			case undefined:
				break;
			case keyword:
				throw runtime_error ("Cannot assign a keyword to a memory reference");
				break;
			case stringLiteral:
				assignee->stringValue = newValue.stringValue;
				assignee->type = Primitive::pstring;
				break;
			case intLiteral:
				assignee->intValue = newValue.intValue;
				assignee->type = Primitive::pint;
				break;
			case boolLiteral:
				assignee->boolValue = newValue.boolValue;
				assignee->type = Primitive::pbool;
				break;
			case memoryReference:
				i->assignVariableValueInternal(assigneeName, realNewMemoryValue, i);
				break;
			case variableReference:
				if (realNewVariableValue != NULL) {
					if (realNewVariableValue->type == Primitive::pint) {
						assignee->intValue = realNewVariableValue->intValue;
					} else if (realNewVariableValue->type == Primitive::pstring) {
						assignee->stringValue = realNewVariableValue->stringValue;
					} else if (realNewVariableValue->type == Primitive::pbool) {
						assignee->boolValue = realNewVariableValue->boolValue;
					} else {
						assignee = realNewVariableValue;
					}
				} else {
					throw runtime_error ("Reference to undeclared variable '" + newValue.variableReference + "' after 'to' keyword.");
				}
				break;
		}
	} else {throw runtime_error ("Token '" + newValue.stringValue + "' cannot be assigned to an unmatching typed reference.");}
}

void Interpreter::assignMemoryValueInternal (Token assignee, Token newValue, Interpreter* i) {
	Token realAssignee = i->memoryManager->getMemoryValue(assignee.memoryReference);
	Token realNewMemoryValue = i->memoryManager->getMemoryValue(newValue.memoryReference);
	Variable * realNewVariableValue = i->getVariable(newValue.variableReference);
	if ((realAssignee.type == newValue.type) || (realAssignee.type == TokenType::undefined)  || (realAssignee.type == realNewMemoryValue.type) || (realNewVariableValue != NULL && realAssignee.type == realNewVariableValue->type) || (newValue.type == TokenType::undefined)) {
		int memoryRef = assignee.memoryReference;
		switch (newValue.type) {
			case undefined:
				i->memoryManager->setMemoryValue(*new Token (), memoryRef);
				break;
			case keyword:
				throw runtime_error ("Cannot assign a keyword to a memory reference");
				break;
			case stringLiteral:
				i->memoryManager->setMemoryValue(*new Token (newValue.stringValue, 0, false, 0, "", TokenType::stringLiteral), memoryRef);
				break;
			case intLiteral:
				i->memoryManager->setMemoryValue(*new Token ("", newValue.intValue, false, 0, "", TokenType::intLiteral), memoryRef);
				break;
			case boolLiteral:
				i->memoryManager->setMemoryValue(*new Token ("", 0, newValue.boolValue, 0, "", TokenType::boolLiteral), memoryRef);
				break;
			case memoryReference:
				i->assignMemoryValueInternal(assignee, realNewMemoryValue, i);
				break;
			case variableReference:
				if (realNewVariableValue != NULL) {
					if (realNewVariableValue->type == Primitive::pint) {
						i->memoryManager->setMemoryValue(*new Token ("", realNewVariableValue->intValue, false, 0, "", TokenType::intLiteral), memoryRef);
					} else if (realNewVariableValue->type == Primitive::pstring) {
						i->memoryManager->setMemoryValue(*new Token (realNewVariableValue->stringValue, 0, false, 0, "", TokenType::stringLiteral), memoryRef);
					} else if (realNewVariableValue->type == Primitive::pbool) {
						i->memoryManager->setMemoryValue(*new Token ("", 0, realNewVariableValue->boolValue, 0, "", TokenType::boolLiteral), memoryRef);
					} else {
						i->memoryManager->setMemoryValue(*new Token (realNewVariableValue->stringValue, realNewVariableValue->intValue, realNewVariableValue->boolValue, 0, "", TokenType::boolLiteral), memoryRef);
					}
				} else {
					throw runtime_error ("Reference to undeclared variable '" + newValue.variableReference + "' after 'to' keyword.");
				}
				break;
		}
	} else {throw runtime_error ("Token '" + newValue.stringValue + "' cannot be assigned to an unmatching typed reference.");}
}
									 
void Interpreter::krepeat(Interpreter* i) {
	// Calculate required number of iterations
	Token numTimesTok = i->currentLineTokens.at(1);
	int numTimes = 0;
	if (numTimesTok.type == TokenType::intLiteral) {
		numTimes = numTimesTok.intValue;
	} else if (numTimesTok.type == TokenType::memoryReference) {
		Token realToken = i->memoryManager->getMemoryValue(numTimesTok.memoryReference);
		if (realToken.type == TokenType::intLiteral) {
			numTimes = realToken.intValue;
		} else {
			throw runtime_error ("Keyword token 'repeat' requires integer as iteration count specifier.");
		}
	} else if (numTimesTok.type == TokenType::variableReference) {
		Variable *realToken = i->getVariable(numTimesTok.variableReference);
		if (realToken->type == Primitive::pint) {
			numTimes = realToken->intValue;
		} else {
			throw runtime_error ("Keyword token 'repeat' requires integer as iteration count specifier.");
		}
	} else {
		throw runtime_error ("Keyword token 'repeat' requires integer as iteration count specifier.");
	}
	
	// Find lines to execute
	if (!(i->currentLineTokens.at (2).stringValue == "times")) {
		throw runtime_error("Expected 'times' after iteration count specifier.");
	}
	
	if (!(i->currentLineTokens.at (3).stringValue == "{")) {
		throw runtime_error("Expected '{' after 'times' token.");
	}
	
	string subExecutionLines;
	
	int loc = i->lineRef+1;
	
	while (loc < i->lines.size()) {
		if ((i->lines.at (loc).size() > 0) &&(i->lines.at (loc)).at(0) == '}') {
			break;
		} else {
			subExecutionLines += i->lines.at (loc) + ";";
		}
		loc++;
	}
	
	// Execute lines
	while (numTimes > 0) {
		Interpreter nextI = *new Interpreter (subExecutionLines, i->memoryManager, i->variables);
		nextI.prepare();
	
		nextI.interpret();
		numTimes--;
	}
	
	// Increment line counter to be aligned with closing brace
	i->lineRef = loc;
}
void Interpreter::kto(Interpreter* i) {}
void Interpreter::kwhile(Interpreter* i) {}
void Interpreter::kfrom(Interpreter* i) {}
void Interpreter::kinput(Interpreter* i) {}
void Interpreter::koutput(Interpreter* i) {
	// Handle any type of token.
	if (i->currentLineTokens.size() <= 1)
		throw runtime_error ("Expected data token after '" + i->currentLineTokens.at (0).stringValue + "' token.");
	for (int tokenRef = 1; tokenRef < i->currentLineTokens.size(); tokenRef++) {
		Token tok = i->currentLineTokens.at (tokenRef);
		if (tok.type == TokenType::memoryReference) {
			tok = i->memoryManager->getMemoryValue(tok.memoryReference);
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
					case pundefined:
						cout << v->stringValue << v->intValue << v->boolValue << endl;
						break;
				}
				break;
		}
	}
	
}
void Interpreter::ktimes(Interpreter* i) {}
