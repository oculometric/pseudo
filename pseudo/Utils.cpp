//
//  Utils.cpp
//  pseudo
//
//  Created by Javax on 23/08/2018.
//  Copyright © 2018 Javax Inc. All rights reserved.
//

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
