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
            m_IsArray(false),
            m_Value(),
            m_ArrayValue(),
            m_IsRef(false),
            m_pTargetTable(nullptr),
            m_TargetName()
        {
        }

        SymbolInfo(const SymbolInfo& rInfo) :
            m_Name(rInfo.m_Name),
            m_IsArray(rInfo.m_IsArray),
            m_Value(rInfo.m_Value),
            m_ArrayValue(rInfo.m_ArrayValue),
            m_IsRef(rInfo.m_IsRef),
            m_pTargetTable(rInfo.m_pTargetTable),
            m_TargetName(rInfo.m_TargetName)
        {
        }

        std::string m_Name;
        bool m_IsArray;
        Value m_Value;
        ArrayValue m_ArrayValue;
        bool m_IsRef;
        SymbolTable* m_pTargetTable;
        std::string m_TargetName;
    };
    SymbolTable(const std::string name);
    SymbolTable(SymbolTable& rProto);
    ~SymbolTable();
    std::string GetName() { return m_Name; }

    // Create
    bool CreateSymbol(std::string name, SymbolInfo info);

    // Read
    std::optional<SymbolInfo> ReadSymbol(std::string name);
    bool IsSymbolPresent(std::string name);

    // Update
    bool UpdateSymbol(std::string name, const SymbolInfo info);

    // Delete
    void DeleteSymbol(std::string name);

    void Dump();
    void Clear();
    static SymbolTable* CreateGlobalSymbols();
    static void DeleteGlobalSymbols(SymbolTable* pGlobalSymbols);

private:
    typedef std::map<std::string, SymbolInfo> SYMBOL_MAP;
    SYMBOL_MAP m_SymbolMap;
    std::string m_Name;
};
};

