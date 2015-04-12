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
  currentTable = new SymbolTable;
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

Token *deattachToken(TokenData * d)
{
  Token * token = d->deattachToken();
  delete d;
  return token;
}

Token *Lexer::getNextToken()
{
  skipSpaces();
  if (state == FINISHED)
    return nullptr;

  TokenData *data = nullptr;
  if (s[currentIndex] == '<' || s[currentIndex] == '>')
  {
    data = getComparisonToken();
    if (!data->hasNullToken())
      return (deattachToken(data));
    delete data;
    
    data = getShiftToken();
    if (!data->hasNullToken())
      return (deattachToken(data));
    delete data;

    return onErrorToken();
  }

  if (s[currentIndex] >= '0' && s[currentIndex] <= '9')
  {
    data = getIntegerToken();
    if (!data->hasNullToken())
      return (deattachToken(data));
    delete data;

    data = getFloatToken(); // add flags like f to determine floats
    if (!data->hasNullToken())
      return (deattachToken(data));
    delete data;
  }
 
  data = getArithmeticToken();
  if (!data->hasNullToken())
    return (deattachToken(data));
  delete data;

  data = getLogicBinaryToken();
  if (!data->hasNullToken())
    return (deattachToken(data));
  delete data;

  data = getBitwiseBinaryToken();
  if (!data->hasNullToken())
    return (deattachToken(data));
  delete data;

  data = getLogicNotToken();
  if (!data->hasNullToken())
    return (deattachToken(data));
  delete data;

  data = getBitwiseNotToken();
  if (!data->hasNullToken())
    return (deattachToken(data));
  delete data;

  data = getBooleanData();
  if (!data->hasNullToken())
    return (deattachToken(data));
  delete data;

  data = getAssignmentToken();
  if (!data->hasNullToken())
    return (deattachToken(data));
  delete data;

  data = getLiteralToken();
  if (!data->hasNullToken())
    return (deattachToken(data));
  delete data;

  data = getIdentifierToken();
  if (!data->hasNullToken())
    return (deattachToken(data));
  delete data;

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
      if (!isSingleLineComment && !isMultilineComment)
      {
        if (prevSymbol != '/')
          return;
        isMultilineComment = true;
      }
      break;
    case '\n':
      currentLine++;
      isSingleLineComment = false;
      break;
    case ' ':
    case '\t':
    case '\r':
      break;
    case 0:
      state = FINISHED;
      return;
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

TokenData * Lexer::onEndMatch(Token *token)
{
  size_t endIndex = currentIndex;
  if (token == nullptr)
    currentIndex = lexemeStart->pos.back();
  lexemeStart->pos.pop_back();
  return new TokenData(token, endIndex - currentIndex);
}

Token *Lexer::onErrorToken()
{

  return nullptr;
}

TokenData *Lexer::getIntegerToken()
{
  /*static const boost::regex e10("^\\d+");
  static const boost::regex e16("^0x[0-9a-fA-F]+");
  static const boost::regex e8("^0[0-7]+");
  static const boost::regex e4("^0q[0-3]+[^0-3]*");
  static const boost::regex e2("^0b[01]+");*/

  int base = 10;
  int num = 0;

  //int pos = find_match_offset(s.substr(currentIndex), e4);

  onStartMatch();

  if (s[currentIndex] == '0') // not 10 base
  {
    switch (s[++currentIndex])
    {
    case 'x':
      base = 16;
      break;
    case 'o': 
      base = 8;
      break;
    case 'q':
      base = 4;
      break;
    case 'b':
      base = 2;
      break;
    case '.': // it's probably floating point number
      return onEndMatch();
    default: // it's just signed zero
      return onEndMatch(new Integer(0));
    }
    currentIndex++;
  }

  num = charToDigit.getDigit(s[currentIndex++]);
  if (num == -1 || num >= base)  // number with x, b or q must not contain 0 digits after letter
    return onEndMatch();

  while (true)
  {
    int digit = charToDigit.getDigit(s[currentIndex]);
    if (digit == -1)  // number is read
      break;
    
    if (digit >= base) // can't be if it's not an error
      return onEndMatch();

    num *= base;
    num += digit;
    currentIndex++;
  }

  if (!Integer::isCharacterPossibleAfterToken(s[currentIndex]))
    return onEndMatch(); 

  return onEndMatch(new Integer(num));
}

TokenData *Lexer::getFloatToken()
{
  float base = 10.0f;
  size_t intBase = static_cast<size_t>(base);
  float invBase = 1.0f;
  float num = 0;
  size_t power = 0;
  int sign = 1;
  bool dotFound = false;
  bool expFound = false;

  onStartMatch();

  sign = getSign();

  num = static_cast<float>(charToDigit.getDigit(s[currentIndex++]));
  if (num == -1 || num >= base)  // number with x, b or q must not contain 0 digits after letter
    return onEndMatch();

  while (true)
  {
    char c = s[currentIndex++];
    int digit = charToDigit.getDigit(c);
    if (digit == -1)  // number is read
    {
      switch (c)
      {
      case '.':
        if (dotFound)
          return onEndMatch();
        dotFound = true;
        continue;
      default:
        return onEndMatch(new Float(num));
      }
    }

    if (digit >= base) // can't be if it's not an error
    {
      if (digit != 0xe)
        return onEndMatch();

      if (expFound)
        return onEndMatch();
      expFound = true;
    }
    else
    {
      if (!dotFound && !expFound)
      {
        num *= base;
        num += digit;
      }
      else if (dotFound && !expFound)
      {
        invBase /= base;
        num += (digit * invBase);
      }
      else
      {
        power *= intBase;
        num += digit;
      }
    }
  }

  if (!Float::isCharacterPossibleAfterToken(s[currentIndex]))
    return onEndMatch(); 

  return onEndMatch(new Float(static_cast<float>(pow(base, power))));
}

TokenData *Lexer::getComparisonToken()
{
//  static const boost::regex afterComparison("^[0-9a-zA-Z_(].*");

  onStartMatch();
  Comparison::ComparisonType type = Comparison::ComparisonType::EQ;
  
  switch (s[currentIndex++])
  {
  case '<':
     type =   Comparison::ComparisonType::LESS;
     break;
  case '>':
    type =   Comparison::ComparisonType::GRE;
    break;
  case '=':
    break;
  case '!':
    type =   Comparison::ComparisonType::NEQ;
    break;
  default:
    return onEndMatch();
  }

  if (s[currentIndex] == '=')
  {
    currentIndex++;
    switch (type)
    {
    case Comparison::ComparisonType::LESS:
      type = Comparison::ComparisonType::LEQ;
      break;
    case Comparison::ComparisonType::GRE:
      type = Comparison::ComparisonType::GREQ;
      break;
    }
  }
  else
  {
    if (type == Comparison::ComparisonType::EQ || type == Comparison::ComparisonType::NEQ)
      return onEndMatch();

    if (!Comparison::isCharacterPossibleAfterToken(s[currentIndex]))
      return onEndMatch();
  }

  return onEndMatch(new Comparison(type));
}

TokenData *Lexer::getShiftToken()
{
  onStartMatch();
  Shift::ShiftType type = Shift::LEFT;

  switch (s[currentIndex++])
  {
  case '>':
    if (s[currentIndex++] == '>')
      type = Shift::RIGHT;
    else 
      return onEndMatch();
    break;
  case '<':
    if (s[currentIndex++] == '<')
      type = Shift::LEFT;
    else
      return onEndMatch();
    break;

  default:
    return onEndMatch();
  }

  if (!Shift::isCharacterPossibleAfterToken(s[currentIndex]))
    return onEndMatch();

  return onEndMatch(new Shift(type));
}

TokenData *Lexer::getArithmeticToken()
{
  onStartMatch();
  Arithmetic::ArithmeticType type = Arithmetic::PLUS;

  switch (s[currentIndex++])
  {
  case '-':
    type = Arithmetic::MINUS;
    break;
  case '*':
    type = Arithmetic::MUL;
    break;
  case '/':
    type = Arithmetic::DIV;
    break;
  case '%':
    type = Arithmetic::MOD;
    break;
  default:
    return onEndMatch();
  }

  if (!Arithmetic::isCharacterPossibleAfterToken(s[currentIndex]))
    return onEndMatch();

  return onEndMatch(new Arithmetic(type));
}

TokenData *Lexer::getBitwiseBinaryToken()
{
  onStartMatch();

  BitwiseBinary::BitwiseType type = BitwiseBinary::AND;
  switch (s[currentIndex++])
  {
  case '|':
    type = BitwiseBinary::OR;
    break;
  case '^':
    type = BitwiseBinary::XOR;
    break;
  default:
    return onEndMatch();
  }

  if (!BitwiseBinary::isCharacterPossibleAfterToken(s[currentIndex]))
    return onEndMatch();

  return onEndMatch(new BitwiseBinary(type));
}

TokenData *Lexer::getBitwiseNotToken()
{
  onStartMatch();

  if (s[currentIndex++] != '~')
    return onEndMatch();

  if (!BitwiseNot::isCharacterPossibleAfterToken(s[currentIndex]))
    return onEndMatch();

  return onEndMatch(new BitwiseNot());
}

TokenData *Lexer::getLogicBinaryToken()
{
  onStartMatch();

  LogicBinary::LogicType type = LogicBinary::AND;
  switch (s[currentIndex++])
  {
  case '|':
    if (s[currentIndex++] != '|')
      return onEndMatch();
    type = LogicBinary::OR;
    break;
  case '&':
    if (s[currentIndex++] != '&')
      return onEndMatch();
    break;
  default:
    return onEndMatch();
  }

  if (!LogicBinary::isCharacterPossibleAfterToken(s[currentIndex]))
    return onEndMatch();

  return onEndMatch(new LogicBinary(type));
}

TokenData *Lexer::getLogicNotToken()
{
  onStartMatch();

  if (s[currentIndex++] != '!')
    return onEndMatch();

  if (!LogicNot::isCharacterPossibleAfterToken(s[currentIndex]))
    return onEndMatch();

  return onEndMatch(new LogicNot());
}

TokenData *Lexer::getAssignmentToken()
{
  onStartMatch();

  if (s[currentIndex++] != '=')
    return onEndMatch();

  if (!Assignment::isCharacterPossibleAfterToken(s[currentIndex]))
    return onEndMatch();

  return onEndMatch(new Assignment());
}

TokenData *Lexer::getLiteralToken()
{
  onStartMatch();

  char *lit = nullptr;

  if (s[currentIndex++] != '"')
    return onEndMatch();

  size_t startIndex = currentIndex;
  while (currentIndex < s.size())
  {
    if (s[currentIndex] == '"' && s[currentIndex] != '\\')
      break;
    currentIndex++;
  }

  if (currentIndex == s.size())
    return onEndMatch();

  // s[currentIndex] == '"'
  if (!Literal::isCharacterPossibleAfterToken(s[currentIndex + 1]))
    return onEndMatch();

  lit = new char[currentIndex - startIndex];
  strcpy(lit, s.substr(startIndex, currentIndex - startIndex - 1).c_str());

  return onEndMatch(new Literal(lit));
}

TokenData *Lexer::getBooleanData()
{
  onStartMatch();

  bool value = false;

  if (!s.compare(currentIndex, 4, "true"))
  {
    value = true;
    currentIndex += 4;
  }
  else if (s.compare(currentIndex, 5, "false"))
    return onEndMatch();
  else
    currentIndex += 5;

  if (!Boolean::isCharacterPossibleAfterToken(s[currentIndex]))
    return onEndMatch();

  return onEndMatch(new Boolean(value));
}

/*TokenData *Lexer::getKeyWord()
{
  static const boost::regex e("^begin\\s");
  onStartMatch();
}*/

TokenData *Lexer::getIdentifierToken()
{
  static const boost::regex e("^([a-zA-Z_][a-zA-Z0-9_]*).*");
  onStartMatch();

  boost::match_results<const char *> results;
  if (boost::regex_match(s.c_str() + currentIndex, results, e))
  {
    std::string name = results[1];
    int len = name.length();
    currentIndex += len;

    SymbolData data;
    
    int index = currentTable->put(SymbolData(DataType::UNKNOWN, name));
    
    if (index == -1) // means it is reserved word
    {
      if (!name.compare("if"))
        return onEndMatch(new ReservedWord(ReservedWord::ReservedType::IF));
      if (!name.compare("elif"))
        return onEndMatch(new ReservedWord(ReservedWord::ReservedType::ELIF));
      if (!name.compare("else"))
        return onEndMatch(new ReservedWord(ReservedWord::ReservedType::ELSE));
      if (!name.compare("while"))
        return onEndMatch(new ReservedWord(ReservedWord::ReservedType::WHILE));
      if (!name.compare("for"))
        return onEndMatch(new ReservedWord(ReservedWord::ReservedType::FOR));
      if (!name.compare("begin"))
      {
        currentTable = new SymbolTable(currentTable);
        return onEndMatch(new ReservedWord(ReservedWord::ReservedType::BEGIN));
      }
      if (!name.compare("end"))
      {
        currentTable = currentTable->getParent();
        if (currentTable == nullptr)
        {
          state = LexerState::SYNTAX_ERROR;
          return onEndMatch();
        }
        return onEndMatch(new ReservedWord(ReservedWord::ReservedType::END));
      }
    }
    return onEndMatch(new Identifier(index, currentTable));
  }
  return onEndMatch();
}
_LEX_END