//
//  MemoryManager.hpp
//  pseudo
//
//  Created by Javax on 10/08/2018.
//  Copyright © 2018 Javax Inc. All rights reserved.
//

#ifndef MemoryManager_hpp
#define MemoryManager_hpp

#include "Token.hpp"

using namespace std;

class MemoryManager {
private:
	vector<Token> memory;
public:
	void setMemoryValue (Token value, int location);
	Token getMemoryValue (int location);
};

#endif /* MemoryManager_hpp */
