#pragma once
#include "InterpreterNode.h"
#include "InterpreterVarNode.h"

namespace Interpreter
{

    class FunctionCallNode : public Node
    {
    public:
        FunctionCallNode() :
            m_pNameVar(nullptr),
            m_pInputVars(nullptr)
        {
        }

        virtual ~FunctionCallNode()
        {
        }

        virtual Node* Clone()
        {
            FunctionCallNode* pNode = new FunctionCallNode;
            pNode->SetNameVar(dynamic_cast<VarNode*>(m_pNameVar->Clone()));
            pNode->SetInputVars(m_pInputVars->CloneList());
            return pNode;
        }

        virtual void Free()
        {
            if (m_pNameVar)
            {
                m_pNameVar->Free();
            }

            if (m_pInputVars)
            {
                m_pInputVars->FreeList();
                m_pInputVars = nullptr;
            }
            Node::Free();
        }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor)
        {
            rVisitor.VisitFunctionCallNode(this);
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

        Node* GetInputVars() { return m_pInputVars; }
        void SetInputVars(Node* pInputVars) { m_pInputVars = pInputVars; }

        VarNode* GetNameVar() { return m_pNameVar; }
        void SetNameVar(VarNode* pVarNode) { m_pNameVar = pVarNode; }

    private:
        VarNode* m_pNameVar;
        Node* m_pInputVars;
    };

};
