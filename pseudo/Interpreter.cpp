//
//  Interpreter.cpp
//  pseudo
//
//  Created by Javax on 10/08/2018.
//  Copyright © 2018 Javax Inc. All rights reserved.
//

#include "Interpreter.hpp"

Variable *Interpreter::getVariable(std::string name) {
	for (int ref = 0; ref < variables->size(); ref++) {
		if (variables->at(ref).name == name) {
			Variable* v = &variables->at(ref);
			return v;
		}
	}
	return NULL;
}

Interpreter::Interpreter (string input, MemoryManager *mm, vector<Variable> *vars, vector<Marker> *mrkrs) {
	directInput = input;
	memoryManager = mm;
	variables = vars;
	markers = mrkrs;
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
		return *new Token ("NULL");
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
		Interpreter nextI = *new Interpreter (subExecutionLines, i->memoryManager, i->variables, i->markers);
		nextI.prepare();
	
		nextI.interpret();
		numTimes--;
	}
	
	// Increment line counter to be aligned with closing brace
	i->lineRef = loc;
}

void Interpreter::koutput(Interpreter* i) {
	// Handle any type of token.
	if (i->currentLineTokens.size() <= 1)
		throw runtime_error ("Expected data token after '" + i->currentLineTokens.at (0).stringValue + "' token.");
	Token outputLoc = *new Token ();
	int endLoc = int (i->currentLineTokens.size())-1;
	
	int toLoc = findFirstKeywordOccurrence(*new Token ("to"), i->currentLineTokens);
	if (toLoc != -1) {
		if (i->currentLineTokens.size() <= toLoc + 1)
			throw runtime_error ("Expected memory or variable reference following 'to' keyword.");
		Token outputTo = i->currentLineTokens.at(toLoc + 1);
		if (outputTo.type != TokenType::memoryReference && outputTo.type != TokenType::variableReference)
			throw runtime_error ("Expected memory or variable reference following 'to' keyword.");
		outputLoc = outputTo;
		endLoc = toLoc - 1;
	}
	
	i->outputInternal(i->currentLineTokens, i, 1, endLoc, outputLoc);
	
	if (outputLoc.type == TokenType::undefined)
		cout << endl;
}

void Interpreter::outputInternal (vector<Token> toks, Interpreter *i, int start, int end, Token outputLocation) {
	stringstream ss;
	for (int tokenRef = start; tokenRef <= end; tokenRef++) {
		Token tok = toks.at (tokenRef);
		if (tok.type == TokenType::memoryReference) {
			tok = i->memoryManager->getMemoryValue(tok.memoryReference);
		}
		
		switch (tok.type) {
			case keyword:
				throw runtime_error ("Found unexpected additional keyword: '" + tok.stringValue + "'.");
				break;
			case stringLiteral:
			case undefined:
				ss << tok.stringValue;
				break;
			case intLiteral:
				ss << tok.intValue;
				break;
			case boolLiteral:
				if (tok.boolValue) {
					ss << "true";
				} else {
					ss << "false";
				}
				break;
			case memoryReference:
				ss << "Oops?";
				break;
			case variableReference:
				Variable * v = i->getVariable(tok.variableReference);
				switch (v->type) {
					case pint:
						ss << v->intValue;
						break;
					case pbool:
						ss << v->boolValue;
						break;
					case pstring:
						ss << v->stringValue;
						break;
					case pundefined:
						ss << v->stringValue << v->intValue << v->boolValue;
						break;
				}
				break;
		}
	}
	
	if (outputLocation.type == TokenType::undefined) {
		cout << ss.str();
		return;
	}
	
	string s = ss.str();
	
	if (outputLocation.type == TokenType::memoryReference)
		i->assignMemoryValueInternal(outputLocation, *new Token (s, 0, false, 0, "", TokenType::stringLiteral), i);
	if (outputLocation.type == TokenType::variableReference)
		i->assignVariableValueInternal(outputLocation.variableReference, *new Token (s, 0, false, 0, "", TokenType::stringLiteral), i);
}

void Interpreter::kinput(Interpreter* i) {
	string input;
	int toLoc = findFirstKeywordOccurrence(*new Token ("to"), i->currentLineTokens);
	if (toLoc == -1)
		throw runtime_error ("Could not locate 'to' keyword to produce input direction.");
	if (i->currentLineTokens.size() <= toLoc + 1)
		throw runtime_error ("Expected memory or variable reference following 'to' keyword.");
	Token inputTo = i->currentLineTokens.at(toLoc + 1);
	if (inputTo.type != TokenType::memoryReference && inputTo.type != TokenType::variableReference)
		throw runtime_error ("Expected memory or variable reference following 'to' keyword.");
	
	// Output all tokens between
	i->outputInternal(i->currentLineTokens, i, 1, toLoc - 1, *new Token());
	
	cin >> input;
	
	if (inputTo.type == TokenType::memoryReference) {
		i->assignMemoryValueInternal(inputTo, *new Token (input, 0, false, 0, "", TokenType::stringLiteral), i);
	} else if (inputTo.type == TokenType::variableReference) {
		i->assignVariableValueInternal(inputTo.variableReference, *new Token (input, 0, false, 0, "", TokenType::stringLiteral), i);
	}
}

void Interpreter::kcast(Interpreter* i) {}

void Interpreter::kwhile(Interpreter* i) {}

void Interpreter::kif(Interpreter* i) {}

void Interpreter::kjump(Interpreter* i) {
	if (i->currentLineTokens.size() < 2)
		throw runtime_error ("Keyword 'jump' requires string literal marker identifier after keyword.");
	if (i->currentLineTokens.at (1).type != TokenType::stringLiteral)
		throw runtime_error ("Token after 'jump' keyword must be a string literal.");
	
	if (!i->markerExists (i, i->currentLineTokens.at (1).stringValue))
		throw runtime_error ("Unknown marker identifier.");
	
	for (Marker mm : *i->markers)
		if (mm.identifier == i->currentLineTokens.at (1).stringValue)
			i->lineRef = mm.lineReference;
}

bool Interpreter::markerExists (Interpreter *i, string s) {
	for (Marker m : *i->markers) {
		if (m.identifier == s) {
			return true;
		}
	}
	return false;
}

void Interpreter::kmarker(Interpreter* i) {
	if (i->currentLineTokens.size() < 2)
		throw runtime_error ("Keyword 'marker' requires string literal as identifier after keyword.");
	if (i->currentLineTokens.at (1).type != TokenType::stringLiteral)
		throw runtime_error ("Token after 'marker' keyword must be a string literal.");
	
	Marker m = *new Marker (i->currentLineTokens.at (1).stringValue, i->lineRef + 1);
	if (!i->markerExists (i, i->currentLineTokens.at (1).stringValue))
		i->markers->push_back(m);
}

// The following should never be implemented
void Interpreter::ktimes(Interpreter* i) {}
void Interpreter::kto(Interpreter* i) {}
void Interpreter::kas(Interpreter* i) {}
void Interpreter::kfrom(Interpreter* i) {}
