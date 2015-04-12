#include <iostream>
#include <vector>
#include "Lexer.h"

using namespace lex;
using namespace std;

int main()
{
  Lexer lexer("input.ag");
  
  vector<Token*> tokens;
  Token * token = nullptr;

  token = lexer.getNextToken();
  while (token != nullptr)
  {
    tokens.push_back(token);
    token = lexer.getNextToken();
  }

  return 0;
}