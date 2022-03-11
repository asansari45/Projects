#pragma once
#include <string>
#include "Tables/InterpreterSymbolTable.h"

namespace Interpreter
{
    class ExecutionNodeVisitorServices
    {
    public:
        // Symbol services
        virtual bool CreateSymbol(std::string name, SymbolTable::SymbolInfo* pSymbolInfo) = 0;
        virtual bool IsSymbolPresent(std::string name)=0;
        virtual SymbolTable::SymbolInfo* ReadSymbol(std::string name, bool getroot=false) = 0;
        virtual bool UpdateSymbol(std::string name, SymbolTable::SymbolInfo* pSymbolInfo) = 0;
        virtual void DeleteSymbol(SymbolTable::SymbolInfo* pSymbolInfo) = 0;
        virtual SymbolTable* GetNoSymbolTable() = 0;
    };
};