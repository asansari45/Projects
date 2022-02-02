#include "InterpreterFunctionCallNode.h"

namespace Interpreter
{

    FunctionCallNode::FunctionCallNode() :
        m_pNameVar(nullptr),
        m_pInputVars(nullptr)
    {
    }

    FunctionCallNode::~FunctionCallNode()
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
    }

    Node* FunctionCallNode::Clone()
    {
        FunctionCallNode* pNode = new FunctionCallNode;
        pNode->SetNameVar(dynamic_cast<VarNode*>(m_pNameVar->Clone()));
        pNode->SetInputVars(m_pInputVars->CloneList());
        return pNode;
    }

    void FunctionCallNode::Free()
    {
        Node::Free();
    }

    void FunctionCallNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitFunctionCallNode(this);
    }

    int FunctionCalNode::GetInputVarCount()
    {
        int count = 0;
        for (Interpreter::Node* pNode = m_pInputVars; pNode; pNode = pNode->GetNext())
        {
            count++;
        }
        return count;
    }

};
