#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class VarNode : public Node
    {
    public:
        VarNode();

        VarNode(const VarNode& rNode);

        virtual ~VarNode();

        virtual Node* Clone();

        virtual void Accept(Interpreter::NodeVisitor& rVisitor);

        std::string GetName() { return m_Name; }
        void SetName(std::string n) { m_Name = n; }

        std::vector<int> GetArraySpecifier() { return m_ArraySpecifier; }
        void SetArraySpecifier(std::vector<int> arraySpecifier) { m_ArraySpecifier = arraySpecifier; }

        void SetSymbolPresent(bool present) { m_SymbolPresent = present; }
        bool IsSymbolPresent() { return m_SymbolPresent; }

        void SetSymbolInfo(SymbolTable::SymbolInfo symbolInfo) { m_SymbolInfo = symbolInfo; }
        SymbolTable::SymbolInfo GetSymbolInfo() { return m_SymbolInfo; }

        void SetNoSymbol(SymbolTable* pNoSymbol) { m_pNoSymbol = pNoSymbol; }
        SymbolTable* GetNoSymbol() { return m_pNoSymbol; }
    private:
        std::string m_Name;
        std::vector<int> m_ArraySpecifier;
        bool m_SymbolPresent;
        SymbolTable::SymbolInfo m_SymbolInfo;
        SymbolTable* m_pNoSymbol; // Table to use for when there is no symbol present.
    };

};
