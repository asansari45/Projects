#include "InterpreterWhileNode.h"

namespace Interpreter
{

    WhileNode::WhileNode() :
        m_pExpr(nullptr),
        m_pThen(nullptr)
    {
    }

    WhileNode::~WhileNode()
    {
        if (m_pExpr)
        {
            m_pExpr->Free();
            m_pExpr = nullptr;
        }

        if (m_pThen)
        {
            m_pThen->FreeList();
            m_pThen = nullptr;
        }
    }

    Node* WhileNode::Clone()
    {
        WhileNode* pNode = new WhileNode;

        // One node to clone.
        if (m_pExpr != nullptr)
        {
            pNode->SetExpr(m_pExpr->Clone());
        }

        // Several nodes to clone.
        if (m_pThen != nullptr)
        {
            pNode->SetThen(m_pThen->CloneList());
        }

        return pNode;
    }

    void WhileNode::Free()
    {
        Node::Free();
    }

    void WhileNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitWhileNode(this);
    }
};
