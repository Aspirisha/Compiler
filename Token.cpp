#include <cctype>
#include "Token.h"

_LEX_BEGIN

CharToDigit::CharToDigit()
{
  for (char c = '0'; c < '9'; c++)
    charToDigit[c] = c - '0';
  for (char c = 'a'; c < 'f'; c++)
    charToDigit[c] = 10 + c - 'a';
  for (char c = 'A'; c < 'F'; c++)
    charToDigit[c] = 10 + c - 'A';
}

bool Operand::isCharacterPossibleAfterToken(char c)
{
  bool isOk = false;
  switch (c)
  {
  case '-':
  case '+':
  case '*':
  case '/':
  case '%':
  case '<':
  case '>':
  case '=': // if it's ==
  case ')':
  case ']':
  case '&':
  case '!':
  case '|':
  case '^':
  case ',':
  case ';':
    isOk = true;
    break;
  }

  return (isOk || isspace(c));
}

bool Operator::isCharacterPossibleAfterToken(char c)
{
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || (c >= '0' && c <= '9') || isspace(c));
}

bool ReservedWord::isCharacterPossibleAfterToken(char c)
{

}

_LEX_END