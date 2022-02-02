#include "InterpreterIfNode.h"

namespace Interpreter
{

    IfNode::IfNode() :
        m_pExpr(nullptr),
        m_pThen(nullptr),
        m_pElif(nullptr),
        m_pElse(nullptr)
    {
    }

    IfNode::~IfNode()
    {
        if (m_pExpr)
        {
            m_pExpr->Free();
            m_pExpr = nullptr;
        }

        if (m_pElse)
        {
            m_pElse->FreeList();
            m_pElse = nullptr;
        }

        if (m_pElif)
        {
            m_pElif->FreeList();
            m_pElif = nullptr;
        }

        if (m_pThen)
        {
            m_pThen->FreeList();
            m_pThen = nullptr;
        }
    }

    Node* IfNode::Clone()
    {
        IfNode* pNode = new IfNode;

        // One node to clone.
        if (m_pExpr != nullptr)
        {
            pNode->SetExpr(m_pExpr->Clone());
        }

        if (m_pThen != nullptr)
        {
            pNode->SetThen(m_pThen->CloneList());
        }

        // Several nodes to clone.
        if (m_pElif != nullptr)
        {
            pNode->SetElif(dynamic_cast<Interpreter::IfNode*>(m_pElif->CloneList()));
        }

        // Several nodes to clone.
        if (m_pElse != nullptr)
        {
            pNode->SetElse(m_pElse->CloneList());
        }

        return pNode;
    }

    void IfNode::Free()
    {
        Node::Free();
    }

    void IfNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitIfNode(this);
    }

};
