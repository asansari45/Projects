#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class FunctionDefNode : public Node
    {
    public:
        FunctionDefNode(std::string name);
        virtual ~FunctionDefNode();
        virtual Node* Clone();
        virtual void Free();
        virtual void Accept(Interpreter::NodeVisitor& rVisitor);
        int GetInputVarCount();
        std::string GetName() { return m_Name; }
        void SetName(std::string name) { m_Name = name; }
        Node* GetInputVars() { return m_pInputVars; }
        void SetInputVars(Node* pInputVars) { m_pInputVars = pInputVars; }
        Node* GetCode() { return m_pCode; }
        void SetCode(Node* pCode) { m_pCode = pCode; }

        SymbolTable* GetSymbolTable() { return m_pSymbolTable; }
        void SetSymbolTable(SymbolTable* pSymbolTable);

    private:
        std::string m_Name;
        Node* m_pInputVars;
        Node* m_pCode;
        SymbolTable* m_pSymbolTable;
    };

};
