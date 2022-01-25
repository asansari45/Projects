#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class FunctionDefNode : public Node
    {
    public:
        FunctionDefNode(std::string name) :
            m_Name(name),
            m_pInputVars(nullptr),
            m_pCode(nullptr),
            m_pSymbolTable(nullptr)
        {
            m_pSymbolTable = new SymbolTable(m_Name);
            assert(m_pSymbolTable != nullptr);
        }

        virtual ~FunctionDefNode()
        {
            delete m_pSymbolTable;
        }

        virtual Node* Clone()
        {
            FunctionDefNode* pNode = new FunctionDefNode(m_Name);
            pNode->SetInputVars(m_pInputVars->CloneList());
            pNode->SetCode(m_pCode->CloneList());
            pNode->SetSymbolTable(m_pSymbolTable->Clone());

            return pNode;
        }

        virtual void Free()
        {
            if (m_pInputVars)
            {
                m_pInputVars->FreeList();
                m_pInputVars = nullptr;
            }

            if (m_pCode)
            {
                m_pCode->FreeList();
                m_pCode = nullptr;
            }

            delete m_pSymbolTable;
            m_pSymbolTable = nullptr;
            Node::Free();
        }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor)
        {
            rVisitor.VisitFunctionDefNode(this);
        }

        int GetInputVarCount()
        {
            int count = 0;
            for (Interpreter::Node* pNode = m_pInputVars; pNode; pNode = pNode->GetNext())
            {
                count++;
            }
            return count;
        }

        std::string GetName() { return m_Name; }
        void SetName(std::string name) { m_Name = name; }
        Node* GetInputVars() { return m_pInputVars; }
        void SetInputVars(Node* pInputVars) { m_pInputVars = pInputVars; }

        Node* GetCode() { return m_pCode; }
        void SetCode(Node* pCode) { m_pCode = pCode; }

        SymbolTable* GetSymbolTable() { return m_pSymbolTable; }
        void SetSymbolTable(SymbolTable* pSymbolTable)
        {
            delete m_pSymbolTable;
            m_pSymbolTable = pSymbolTable;
        }


    private:
        std::string m_Name;
        Node* m_pInputVars;
        Node* m_pCode;
        SymbolTable* m_pSymbolTable;
    };

};
