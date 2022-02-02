#pragma once
#include "InterpreterBinaryNode.h"

namespace Interpreter
{

    BinaryNode::BinaryNode() :
            m_Operator(EQU),
            m_pLeft(nullptr),
            m_pRight(nullptr)
    {
    }

    BinaryNode::BinaryNode(const BinaryNode& rNode) :
            Node(rNode),
            m_Operator(rNode.m_Operator)
    {
    }

    BinaryNode::~BinaryNode()
    {
        // delete left child and right children first
        if (m_pLeft)
        {
            m_pLeft->Free();
            m_pLeft = nullptr;
        }

        if (m_pRight)
        {
            m_pRight->Free();
            m_pRight = nullptr;
        }
    }

    Node* BinaryNode::Clone()
    {
        BinaryNode* pNode = new BinaryNode(*this);
        if (m_pLeft != nullptr)
        {
            pNode->SetLeft(m_pLeft->Clone());
        }

        if (m_pRight != nullptr)
        {
            pNode->SetRight(m_pRight->Clone());
        }

        return pNode;
    }

    void BinaryNode::Free()
    {
        Node::Free();
    }

    void BinaryNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitBinaryNode(this);
    }
};
