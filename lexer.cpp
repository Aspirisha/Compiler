#include "Lexer.h"
#include <boost\regex.hpp>
#include <string>
#include <sstream>

using namespace std;

_LEX_BEGIN

const CharToDigit Lexer::charToDigit;

int find_match_offset(std::string const& string_to_search, boost::regex const& expression)
{
  boost::smatch results;
  if (boost::regex_match(string_to_search, results, expression))
  {
    int len = results.length();
    return results.position();
  }
  return -1;
}

Lexer::Lexer(const char *fileName) : currentLine(1)
{
  lexemeStart = new LexemeStart;
  readFile(fileName);
}

bool Lexer::readFile(const char *fileName)
{
  ifstream in(fileName, std::ios::in);
  in.close();
  in.open(fileName, ios::in);

  bool retVal = true;
  if (in)
  {
    std::stringstream buffer;
    buffer << in.rdbuf();
    s = buffer.str();
    s.push_back(0); // assume \0 will never appear as a character in any lexeme so it interrupts any lexeme recognition
    currentIndex = 0;
    state = PARSING;
  }
  else
  {
    state = WRONG_FILE;
    retVal = false;
  }

  return retVal;
}

int Lexer::getSign()
{
  switch (s[currentIndex])
  {
  case '+':
    currentIndex++;
    return 1;
  case '-':
    currentIndex++;
    return -1;
  default:
    return 1;
  }
}

Token *Lexer::getNextToken()
{
  skipSpaces();

  Token *token = nullptr;

  switch (s[currentIndex])
  {
  case '>':
  case '<':
  {
    TokenData<Comparison> *comparison = getTokenData<Comparison>();
    if (!comparison->hasNullToken())
    {
      token = comparison->deattachToken();
      delete comparison;
      return token;
    }
    delete comparison;
    
    TokenData<Shift> *shift = getTokenData<Shift>();
    if (!shift->hasNullToken())
    {
      token = shift->deattachToken();
      delete shift;
      return token;
    }
    delete shift;

    return onErrorToken();
  }
  }


  TokenData<Float> *data = getTokenData<Float>();

  return nullptr;
}

void Lexer::skipSpaces()
{
  bool isSingleLineComment = false;
  bool isMultilineComment = false;
  char prevSymbol = 0;

  while (true)
  {
    switch (s[currentIndex])
    {
    case '/':
      if (prevSymbol == '/' && !isMultilineComment && !isSingleLineComment)
        isSingleLineComment = true;

      else if (prevSymbol == '*' && isMultilineComment)
        isMultilineComment = false;
      break;
    case '*':
      if (prevSymbol == '/' && !isSingleLineComment && !isMultilineComment)
        isMultilineComment = true;
      break;
    case '\n':
      currentLine++;
      break;
    case ' ':
    case '\t':
    case '\r':
      break;
    default:
      if (isSingleLineComment || isMultilineComment)
        break;
      return;
    }
    prevSymbol = s[currentIndex++];
  }
}


void Lexer::onStartMatch()
{
  lexemeStart->pos.push_back(currentIndex);
}

Token *Lexer::onErrorToken()
{

  return nullptr;
}

_LEX_END