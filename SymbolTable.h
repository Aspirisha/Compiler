#pragma once

#include <vector>
#include <map>
#include <string>

enum DataType
{
  NON_EXIST,
  RESERVED,
  UNKNOWN,
  STRING,
  S_INTEGER,
  U_INTEGER,
  FLOAT,
  BOOL,
};

struct SymbolData
{
  SymbolData() : type(DataType::NON_EXIST) {}
  SymbolData(DataType type, const std::string &name) : type(type), name(name) {}
  
  operator bool() { return type != DataType::NON_EXIST; }

  std::string name;
  DataType type;
};

class SymbolTable
{
public:
  SymbolTable() : parent(nullptr) 
  {
    put(SymbolData(RESERVED, "if"));
    put(SymbolData(RESERVED, "elif"));
    put(SymbolData(RESERVED, "else"));
    put(SymbolData(RESERVED, "for"));
    put(SymbolData(RESERVED, "while"));
    put(SymbolData(RESERVED, "begin"));
    put(SymbolData(RESERVED, "end"));
  }

  SymbolTable(SymbolTable *parent) : parent(parent) {}

  int put(const SymbolData &data)
  {
    if (contains(data.name))
      return -1;

    table.push_back(data);

    int index = table.size() - 1;
    names[data.name] = index;
    return index;
  }
  
  SymbolData getFromCurrentScope(const std::string &name) 
  { 
    if (contains(name))
      return table[names[name]];
    return SymbolData(NON_EXIST, "");
  }
  
  SymbolData getFromCurrentScope(size_t index) 
  { 
    if (index >= 0 && index < table.size())
      return table[index]; 
    return SymbolData(NON_EXIST, "");
  }
  
  SymbolData getFromAnyClosestScope(const std::string &name)
  {
    SymbolTable *tbl = this;

    do 
    {
      if (tbl->contains(name))
        return tbl->table[names[name]];
      tbl = tbl->parent;
    } while (tbl);
    return SymbolData(NON_EXIST, "");
  }

  bool contains(const std::string &name) { return (names.find(name) != names.end()); }
private:
  SymbolTable *parent;

  std::vector<SymbolData> table;
  std::map<std::string, size_t> names;
};