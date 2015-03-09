#pragma once

#include <vector>
#include <map>
#include <string>

enum DataType
{
  NON_EXIST,
  STRING,
  S_INTEGER,
  U_INTEGER,
  FLOAT,
  BOOL,
};

struct SymbolData
{
  SymbolData() : type(DataType::NON_EXIST) {}
  operator bool() { return type != DataType::NON_EXIST; }

  std::string name;
  DataType type;
};

class SymbolTable
{
public:
  SymbolTable() : parent(nullptr) {}
  SymbolTable(SymbolTable *parent) : parent(parent) {}

  void put(std::string name, SymbolData data);
  SymbolData get(std::string name);
private:
  SymbolTable *parent;

  std::map<std::string, size_t> identifierToIndex; 
  std::vector<SymbolData> table;
};