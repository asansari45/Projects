#pragma once
#include "InterpreterForNode.h"

namespace Interpreter
{
    ForNode::ForNode() :
        m_pInit(nullptr),
        m_pExpr(nullptr),
        m_pThen(nullptr)
    {
    }

    ForNode::~ForNode()
    {
        if (m_pInit)
        {
            m_pInit->Free();
            m_pInit = nullptr;
        }

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

    Node* ForNode::Clone()
    {
        ForNode* pNode = new ForNode;

        // One node to clone.
        if (m_pInit != nullptr)
        {
            pNode->SetInit(m_pInit->Clone());
        }

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

    void ForNode::Free()
    {
        Node::Free();
    }

    void ForNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitForNode(this);
    }
};
