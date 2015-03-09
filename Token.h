#pragma once

#include <map>

#define _LEX_BEGIN namespace lex {
#define _LEX_END }

_LEX_BEGIN

enum TokenType
{
  // constants
  INTEGER,
  FLOAT,
  LITERAL,
  BOOL,

  // identifier
  IDENTIFIER,

  // reserved words
  IF,
  ELIF,
  ELSE,
  WHILE,
  FOR,
  BEGIN,
  END,
  
  // operators
  COMPARISON,     // <, <=, >, >=, ==, !=
  ARITHMETIC,     // +, -, *, /, %
  SHIFT,          // <<, >>
  BITWISE_BINARY, // &, |, ^
  BITWISE_NOT,    // ~ 
  LOGIC_BINARY,   // &&, ||
  LOGIC_NOT,      // !
  ASSIGNMENT,     // =

  // punctuation
  LEFT_RND_BRACKET,    // (
  RIGHT_RND_BRACKET,   // )
  LEFT_SQR_BRACKET,    // [
  RIGHT_SQR_BRACKET,   // ]
  SEMICOLON,           // ;
  COMMA,               // ,

};

struct Token
{
  virtual TokenType getType() = 0;
  static bool isCharacterPossibleAfterToken(char c);
  virtual ~Token() {};
};

struct Operand : Token
{
  static bool isCharacterPossibleAfterToken(char c);
};

struct ReservedWord : Token
{
  static bool isCharacterPossibleAfterToken(char c);
};

struct Operator : Token
{
  static bool isCharacterPossibleAfterToken(char c);
};

struct OpenBracket : Token
{
  static bool isCharacterPossibleAfterToken(char c);
};

struct CloseBracket : Token
{
  static bool isCharacterPossibleAfterToken(char c);
};


struct Integer : Operand
{
  Integer() : value(0) {}
  Integer(int v) : value(v) {}

  TokenType getType() { return TokenType::INTEGER; }
  int value;
};

struct Float : Operand
{
  Float() : value(0) {}
  Float(float v) : value(v) {}

  TokenType getType() { return TokenType::FLOAT; }
  float value;
};

struct Literal : Operand
{
  Literal() {}
  Literal(const char *v) : value(v) {}

  TokenType getType() { return TokenType::LITERAL; }
  const char *value;
};

struct Boolean : Operand
{
  Boolean() : value(false) {}
  Boolean(bool v) : value(v) {}

  TokenType getType() { return TokenType::BOOL; }
  bool value;
};

struct Identifier : Operand
{
  Identifier(size_t v) : indexInSymTable(v) {}

  TokenType getType() { return TokenType::IDENTIFIER; }
  size_t indexInSymTable;
};

struct Comparison : Operator
{
  enum ComparisonType
  {
    LESS,
    LEQ,
    EQ,
    GRE,
    GREQ,
    NEQ
  };
  Comparison(ComparisonType v) : type(v) {}

  TokenType getType() { return TokenType::COMPARISON; }

  ComparisonType type;
};

struct Arithmetic : Operator
{
  enum ArithmeticType
  {
    PLUS,
    MINUS,
    DIV, 
    MUL,
    MOD
  };
  Arithmetic(ArithmeticType v) : type(v) {}

  TokenType getType() { return TokenType::ARITHMETIC; }
  ArithmeticType type;
};

struct Shift : Operator
{
  enum ShiftType
  {
    LEFT,
    RIGHT
  };

  Shift(ShiftType v) : type(v) {}

  TokenType getType() { return TokenType::SHIFT; }
  ShiftType type;
};

struct BitwiseBinary : Operator
{
  enum BitwiseType
  {
    AND,
    OR,
    XOR
  };

  BitwiseBinary(BitwiseType v) : type(v) {}

  TokenType getType() { return TokenType::BITWISE_BINARY; }
  BitwiseType type;
};

struct BitwiseNot : Operator
{
  BitwiseNot() {}

  TokenType getType() { return TokenType::BITWISE_NOT; }
};

struct LogicBinary : Operator
{
  enum LogicType
  {
    AND,
    OR,
    XOR
  };

  LogicBinary(LogicType v) : type(v) {}

  TokenType getType() { return TokenType::LOGIC_BINARY; }
  LogicType type;
};

struct LogicNot : Operator
{
  LogicNot() {}

  TokenType getType() { return TokenType::LOGIC_NOT; }
};

struct Assignment : Operator
{
  Assignment() {}

  TokenType getType() { return TokenType::ASSIGNMENT; }
};



struct CharToDigit
{
public:
  CharToDigit();
  int getDigit(char c) const
  {
    if (charToDigit.find(c) != charToDigit.end()) 
      return -1;
    return charToDigit[c];
  }
private:
  mutable std::map<char, size_t> charToDigit;
};


_LEX_END