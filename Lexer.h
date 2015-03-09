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
template <class T> struct TokenData;

class Lexer
{
public:
  Lexer() {}
  Lexer(const char *fileName);

  bool readFile(const char *fileName);
  Token *getNextToken();

private:
  friend struct LexemeStart;
  static const CharToDigit charToDigit;

  std::string s;
  size_t currentIndex;
  size_t currentLine;
  SymbolTable currentTable;
  LexerState state;
  LexemeStart *lexemeStart; 

  void onStartMatch();
  template<class T> TokenData<T> * onEndMatch(T * token = nullptr)
  {
    size_t endIndex = currentIndex;
    currentIndex = lexemeStart->pos.back();
    lexemeStart->pos.pop_back();
    return new TokenData<T>(token, endIndex - currentIndex);
  }

  Token *onErrorToken();
  void skipSpaces();
  int getSign();

  template <class T> TokenData<T> *getTokenData();
  template<> TokenData<Integer> *getTokenData();
  template<> TokenData<Float> *getTokenData();
  template<> TokenData<Literal> *getTokenData();
  template<> TokenData<Boolean> *getTokenData();

  template<> TokenData<Identifier> *getTokenData();

  template<> TokenData<Assignment> *getTokenData();
  template<> TokenData<LogicNot> *getTokenData();
  template<> TokenData<BitwiseNot> *getTokenData();
  template<> TokenData<LogicBinary> *getTokenData();
  template<> TokenData<BitwiseBinary> *getTokenData();
  template<> TokenData<Arithmetic> *getTokenData();
  template<> TokenData<Shift> *getTokenData();
  template<> TokenData<Comparison> *getTokenData();
};


struct LexemeStart
{
  LexemeStart() {}

  friend void Lexer::onStartMatch();
  template<class T> friend TokenData<T> * Lexer::onEndMatch(T * token);
private:
  std::vector<size_t> pos;
};


template <class T> struct TokenData
{
  ~TokenData()
  {
    delete token;
  }

  T *deattachToken()
  {
    T *temp = token;
    token = nullptr;
    return token;
  }

  bool hasNullToken() { return token == nullptr; }

  size_t tokenLength;
private:
  T *token;
  TokenData() : token(nullptr), tokenLength(0) {};
  TokenData(T *t, size_t l) : token(t), tokenLength(l) {};

  template <class T> friend TokenData<T> * Lexer::onEndMatch(T * token);
};

#include "Lexer.hpp"

_LEX_END