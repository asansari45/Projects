#pragma once

#include <string>
#include <map>
#include <optional>
#include <typeindex>
#include <assert.h>
#include "Values/InterpreterValue.h"
#include "Values/InterpreterArrayValue.h"

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
            m_pArrayValue(),
            m_IsRef(false),
            m_RefName(),
            m_pRefTable(nullptr)
        {
        }

        SymbolInfo* Clone()
        {
            SymbolInfo* pClone = new SymbolInfo;
            assert(pClone != nullptr);
            pClone->m_Name = m_Name;
            pClone->m_pTable = m_pTable;
            pClone->m_IsArray = m_IsArray;
            if (m_IsArray)
            {
                pClone->m_pArrayValue = m_pArrayValue->Clone();
            }
            pClone->m_IsRef = m_IsRef;
            pClone->m_RefName = m_RefName;
            pClone->m_pRefTable = m_pRefTable;
            return pClone;
        }

        std::string m_Name;
        SymbolTable* m_pTable;
        bool m_IsArray;
        Value m_Value;
        ArrayValue* m_pArrayValue;
        bool m_IsRef;
        std::string m_RefName;
        SymbolTable* m_pRefTable;
    };
    SymbolTable(const std::string name);
    SymbolTable(SymbolTable& rProto);
    ~SymbolTable();
    std::string GetName() { return m_Name; }

    // Create
    bool CreateSymbol(std::string name, SymbolInfo* pInfo);

    // Read
    SymbolInfo* ReadSymbol(std::string name, bool getroot=false);
    bool IsSymbolPresent(std::string name);

    // Update
    bool UpdateSymbol(std::string name, SymbolInfo* pInfo);

    // Delete
    void DeleteSymbol(std::string name);

    void Dump();
    void Clear();
    SymbolTable* Clone();
    static SymbolTable* CreateGlobalSymbols();
    static void DeleteGlobalSymbols(SymbolTable* pGlobalSymbols);

private:
    typedef std::map<std::string, SymbolInfo*> SYMBOL_MAP;
    SYMBOL_MAP m_SymbolMap;
    std::string m_Name;
};
};

