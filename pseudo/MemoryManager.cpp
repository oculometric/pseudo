//
//  MemoryManager.cpp
//  pseudo
//
//  Created by Javax on 10/08/2018.
//  Copyright © 2018 Javax Inc. All rights reserved.
//

#include "MemoryManager.hpp"

void MemoryManager::setMemoryValue (Token value, int location) {
	memory.resize(location + 1);
	memory.at (location) = value;
}

Token MemoryManager::getMemoryValue (int location) {
	return memory.at(location);
}
