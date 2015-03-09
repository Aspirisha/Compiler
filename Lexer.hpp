#pragma once
template<> TokenData<Integer> *Lexer::getTokenData()
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
      return onEndMatch<Integer>();
    default: // it's just signed zero
      return onEndMatch<Integer>(new Integer(0));
    }
    currentIndex++;
  }

  num = charToDigit.getDigit(s[currentIndex++]);
  if (num == -1 || num >= base)  // number with x, b or q must not contain 0 digits after letter
    return onEndMatch<Integer>();

  while (true)
  {
    int digit = charToDigit.getDigit(s[currentIndex++]);
    if (digit == -1)  // number is read
      break;
    
    if (digit >= base) // can't be if it's not an error
      return onEndMatch<Integer>();

    num *= base;
    num += digit;
  }

  if (!Integer::isCharacterPossibleAfterToken(s[currentIndex]))
    return onEndMatch<Integer>(); 

  return onEndMatch<Integer>(new Integer(num));
}

template<> TokenData<Float> *Lexer::getTokenData()
{
  float base = 10.0f;
  float invBase = 1.0f;
  float num = 0;
  size_t power = 0;
  int sign = 1;
  bool dotFound = false;
  bool expFound = false;

  onStartMatch();

  sign = getSign();

  num = charToDigit.getDigit(s[currentIndex++]);
  if (num == -1 || num >= base)  // number with x, b or q must not contain 0 digits after letter
    return onEndMatch<Float>();

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
          return onEndMatch<Float>();
        dotFound = true;
        break;
      }
    }
    
    if (digit >= base) // can't be if it's not an error
    {
      if (digit != 0xe)
        return onEndMatch<Float>();

      if (expFound)
        return onEndMatch<Float>();
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
        power *= base;
        num += digit;
      }
    }
  }

  if (!Float::isCharacterPossibleAfterToken(s[currentIndex]))
    return onEndMatch<Float>(); 

  return onEndMatch<Float>(new Float(num * sign * pow(base, power)));
}

template<> TokenData<Comparison> *Lexer::getTokenData()
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
    return onEndMatch<Comparison>();
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
      return onEndMatch<Comparison>();

    if (!Comparison::isCharacterPossibleAfterToken(s[currentIndex]))
      return onEndMatch<Comparison>();
  }

  return onEndMatch<Comparison>(new Comparison(type));
}

template<> TokenData<Shift> *Lexer::getTokenData()
{
  onStartMatch();
  Shift::ShiftType type = Shift::LEFT;

  switch (s[currentIndex++])
  {
  case '>':
    if (s[currentIndex++] == '>')
      type = Shift::RIGHT;
    else 
      return onEndMatch<Shift>();
    break;
  case '<':
    if (s[currentIndex++] == '<')
      type = Shift::LEFT;
    else
      return onEndMatch<Shift>();
    break;

  default:
    return onEndMatch<Shift>();
  }

  if (!Shift::isCharacterPossibleAfterToken(s[currentIndex]))
    return onEndMatch<Shift>();

  return onEndMatch<Shift>(new Shift(type));
}

template<> TokenData<Arithmetic> *Lexer::getTokenData()
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
    return onEndMatch<Arithmetic>();
  }

  if (!Arithmetic::isCharacterPossibleAfterToken(s[currentIndex]))
    return onEndMatch<Arithmetic>();

  return onEndMatch<Arithmetic>(new Arithmetic(type));
}

template<> TokenData<BitwiseBinary> *Lexer::getTokenData()
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
    return onEndMatch<BitwiseBinary>();
  }

  if (!BitwiseBinary::isCharacterPossibleAfterToken(s[currentIndex]))
    return onEndMatch<BitwiseBinary>();

  return onEndMatch<BitwiseBinary>(new BitwiseBinary(type));
}

template<> TokenData<BitwiseNot> *Lexer::getTokenData()
{
  onStartMatch();

  if (s[currentIndex++] != '~')
    return onEndMatch<BitwiseNot>();

  if (!BitwiseNot::isCharacterPossibleAfterToken(s[currentIndex]))
    return onEndMatch<BitwiseNot>();

  return onEndMatch<BitwiseNot>(new BitwiseNot());
}

template<> TokenData<LogicBinary> *Lexer::getTokenData()
{
  onStartMatch();

  LogicBinary::LogicType type = LogicBinary::AND;
  switch (s[currentIndex++])
  {
  case '|':
    if (s[currentIndex++] != '|')
      return onEndMatch<LogicBinary>();
    type = LogicBinary::OR;
    break;
  case '&':
    if (s[currentIndex++] != '&')
      return onEndMatch<LogicBinary>();
    break;
  default:
    return onEndMatch<LogicBinary>();
  }

  if (!LogicBinary::isCharacterPossibleAfterToken(s[currentIndex]))
    return onEndMatch<LogicBinary>();

  return onEndMatch<LogicBinary>(new LogicBinary(type));
}

template<> TokenData<LogicNot> *Lexer::getTokenData()
{
  onStartMatch();

  if (s[currentIndex++] != '!')
    return onEndMatch<LogicNot>();

  if (!LogicNot::isCharacterPossibleAfterToken(s[currentIndex]))
    return onEndMatch<LogicNot>();

  return onEndMatch<LogicNot>(new LogicNot());
}

template<> TokenData<Assignment> *Lexer::getTokenData()
{
  onStartMatch();

  if (s[currentIndex++] != '=')
    return onEndMatch<Assignment>();

  if (!Assignment::isCharacterPossibleAfterToken(s[currentIndex]))
    return onEndMatch<Assignment>();

  return onEndMatch<Assignment>(new Assignment());
}