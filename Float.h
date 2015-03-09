#pragma once
#include "Token.h"

struct Float : public Token
{
  Float() : value(0) {}
  Float(float v) : value(v) {}

  TokenType getType() { return TokenType::FLOAT; }
  float value;
};
