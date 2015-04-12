#include "SymbolTable.h"

std::set<std::string> SymbolTable::s_reservedNames;
bool SymbolTable::reservedInited = false;