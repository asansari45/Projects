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
    struct SymbolInfo
    {
        SymbolInfo() :
            m_Name(),
            m_pTable(nullptr),
            m_IsArray(false),
            m_Value(),
            m_ArrayValue(),
            m_IsRef(false),
            m_RefName(),
            m_pRefTable(nullptr)
        {
        }

        SymbolInfo(const SymbolInfo& rInfo) :
            m_Name(rInfo.m_Name),
            m_pTable(rInfo.m_pTable),
            m_IsArray(rInfo.m_IsArray),
            m_Value(rInfo.m_Value),
            m_ArrayValue(rInfo.m_ArrayValue),
            m_IsRef(rInfo.m_IsRef),
            m_RefName(rInfo.m_RefName),
            m_pRefTable(rInfo.m_pRefTable)
        {
        }

        std::string m_Name;
        SymbolTable* m_pTable;
        bool m_IsArray;
        Value m_Value;
        ArrayValue m_ArrayValue;
        bool m_IsRef;
        std::string m_RefName;
        SymbolTable* m_pRefTable;
    };
    SymbolTable(const std::string name);
    SymbolTable(SymbolTable& rProto);
    ~SymbolTable();
    std::string GetName() { return m_Name; }

    // Create
    bool CreateSymbol(std::string name, SymbolInfo info);

    // Read
    std::optional<SymbolInfo> ReadSymbol(std::string name, bool getroot=false);
    bool IsSymbolPresent(std::string name);

    // Update
    bool UpdateSymbol(std::string name, const SymbolInfo info);

    // Delete
    void DeleteSymbol(std::string name);

    void Dump();
    void Clear();
    SymbolTable* Clone();
    static SymbolTable* CreateGlobalSymbols();
    static void DeleteGlobalSymbols(SymbolTable* pGlobalSymbols);

private:
    typedef std::map<std::string, SymbolInfo> SYMBOL_MAP;
    SYMBOL_MAP m_SymbolMap;
    std::string m_Name;
};
};
