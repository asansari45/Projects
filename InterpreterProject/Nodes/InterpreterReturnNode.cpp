#include "InterpreterReturnNode.h"

namespace Interpreter
{

    ReturnNode::ReturnNode() :
        Node(),
        m_pExpr(nullptr)
    {
    }

    ReturnNode::~ReturnNode()
    {
    }

    Node* ReturnNode::Clone()
    {
        ReturnNode* pNode = new ReturnNode;
        if (m_pExpr != nullptr)
        {
            pNode->SetExpr(m_pExpr->CloneList());
        }
        return pNode;
    }

    void ReturnNode::Free()
    {
        if (m_pExpr != nullptr)
        {
            m_pExpr->FreeList();
            m_pExpr = nullptr;
        }
        Node::Free();
    }

    void ReturnNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitReturnNode(this);
    }
};
