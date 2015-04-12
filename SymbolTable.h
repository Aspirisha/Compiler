#pragma once

#include <vector>
#include <map>
#include <string>
#include <set>

enum DataType
{
  NON_EXIST,
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
  static const size_t s_reservedWordsNumber = 7;
  static std::set<std::string> s_reservedNames;

  static bool isReservedWord(const std::string &name)
  {
    return (s_reservedNames.find(name) != s_reservedNames.end());
  }

  SymbolTable() : parent(nullptr) 
  {
    if (!reservedInited)
      initTable();
  }

  SymbolTable(SymbolTable *parent) : parent(parent) 
  {
    if (!reservedInited)
      initTable();
  }


  int put(const SymbolData &data)
  {
    if (isReservedWord(data.name))
      return -1;

    if (contains(data.name))
      return names[data.name];

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

  int getIndex(const SymbolData &data)
  {
    if (contains(data.name))
      return names[data.name];

    return -1;
  }

  SymbolTable *getParent()
  {
    return parent;
  }

private:
  SymbolTable *parent;
  static bool reservedInited;
  std::vector<SymbolData> table;
  std::map<std::string, size_t> names;
  
  void initTable()
  {
    s_reservedNames.insert("if");
    s_reservedNames.insert("elif");
    s_reservedNames.insert("else");
    s_reservedNames.insert("for");
    s_reservedNames.insert("while");
    s_reservedNames.insert("begin");
    s_reservedNames.insert("end");

    reservedInited = true;
  }
}; 