#pragma once

#include <string>
#include <map>
#include <optional>
#include <typeindex>
#include "InterpreterValue.h"
#include "InterpreterArrayValue.h"

namespace Interpreter
{
class SymbolTable
{
public:
    SymbolTable();
    ~SymbolTable();
    bool AddSymbol(const std::string name, Value value);
    bool AddSymbol(const std::string name, ArrayValue value);
    bool ChangeSymbol(const std::string name, Value value, const std::vector<int>* pElement = nullptr);
    bool ChangeSymbol(const std::string name, ArrayValue value);
    std::optional<std::type_index> GetSymbolType(const std::string name);
    bool IsSymbolArray(const std::string name);
    std::optional<std::vector<int> > GetSymbolDims(const std::string name);
    std::optional<Value> GetSymbolValue(const std::string name, const std::vector<int>* pElement=nullptr);
    std::optional<ArrayValue> GetArraySymbolValue(const std::string name);
    bool IsSymbolPresent(const std::string name);
    bool DelSymbol(const std::string symbol);
    void Dump();
    void Clear();

private:
    struct SymbolInfo
    {
        std::string m_Name;
        bool m_IsArray;
        Value m_Value;
        ArrayValue m_ArrayValue;
    };
    std::map<std::string, SymbolInfo*> m_SymbolMap;
};
};

