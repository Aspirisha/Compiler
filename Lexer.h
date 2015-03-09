#pragma once

#include <fstream>
#include <vector>
#include "Token.h"
#include "SymbolTable.h"

_LEX_BEGIN

enum LexerState
{
  PARSING,
  WRONG_FILE,
  FINISHED
};

class Lexer;
struct LexemeStart;
struct TokenData;

class Lexer
{
public:
  Lexer() {}
  Lexer(const char *fileName);

  bool readFile(const char *fileName);
  Token *getNextToken();

private:
  friend struct LexemeStart;
  friend struct TokenData;
  static const CharToDigit charToDigit;

  std::string s;
  size_t currentIndex;
  size_t currentLine;
  SymbolTable currentTable;
  LexerState state;
  LexemeStart *lexemeStart; 

  void onStartMatch();
  TokenData * onEndMatch(Token * token = nullptr);

  Token *onErrorToken();
  Token *onReturnToken(TokenData *data);

  void skipSpaces();
  int getSign();

  TokenData *getIntegerToken();
  TokenData *getFloatToken();
  TokenData *getLiteralToken();
  TokenData *getBooleanData();

  TokenData *getIdentifierToken();

  TokenData *getAssignmentToken();
  TokenData *getLogicNotToken();
  TokenData *getBitwiseNotToken();
  TokenData *getLogicBinaryToken();
  TokenData *getBitwiseBinaryToken();
  TokenData *getArithmeticToken();
  TokenData *getShiftToken();
  TokenData *getComparisonToken();
};


struct LexemeStart
{
  LexemeStart() {}

  friend void Lexer::onStartMatch();
  friend TokenData * Lexer::onEndMatch(Token * token);
private:
  std::vector<size_t> pos;
};


struct TokenData
{
  ~TokenData()
  {
    delete token;
  }

  Token *deattachToken()
  {
    Token *temp = token;
    token = nullptr;
    return token;
  }

  bool hasNullToken() { return token == nullptr; }

  size_t tokenLength;
private:
  Token *token;
  TokenData() : token(nullptr), tokenLength(0) {};
  TokenData(Token *t, size_t l) : token(t), tokenLength(l) {};

  friend TokenData * Lexer::onEndMatch(Token * token);
};

_LEX_END

#include "Lexer.hpp"

