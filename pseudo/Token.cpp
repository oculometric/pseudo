//
//  Token.cpp
//  pseudo
//
//  Created by Javax on 09/08/2018.
//  Copyright © 2018 Javax Inc. All rights reserved.
//

#include "Token.hpp"

Token::Token (string sv, int iv, bool bv, int mr, string vr, TokenType t) {
	stringValue = sv;
	intValue = iv;
	boolValue = bv;
	memoryReference = mr;
	variableReference = vr;
	type = t;
}
