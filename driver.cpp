#include <iostream>
#include "Lexer.h"

using namespace lex;

int main()
{
  Lexer lexer("input.ag");
  Token * token = lexer.getNextToken();
  return 0;
}