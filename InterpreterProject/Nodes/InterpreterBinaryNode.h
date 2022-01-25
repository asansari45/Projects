#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class BinaryNode : public Node
    {
    public:

        enum Operator
        {
            EQU = 1,
            ADD,
            SUB,
            MUL,
            DIV,
            LES,
            LEQ,
            GRT,
            GEQ,
            DEQ,
            NEQ,
            NEG,
            DIM,
            ARY
        };

        BinaryNode() :
            m_Operator(EQU),
            m_pLeft(nullptr),
            m_pRight(nullptr)
        {
        }

        BinaryNode(const BinaryNode& rNode) :
            Node(rNode),
            m_Operator(rNode.m_Operator)
        {
        }

        virtual ~BinaryNode()
        {
        }

        virtual Node* Clone()
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

        virtual void Free()
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
            Node::Free();
        }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor)
        {
            rVisitor.VisitBinaryNode(this);
        }

        Operator GetOperator() { return m_Operator; }
        void SetOperator(Operator o) { m_Operator = o; }

        Node* GetLeft() { return m_pLeft; }
        void SetLeft(Node* pNode) { m_pLeft = pNode; }
        Node* GetRight() { return m_pRight; }
        void SetRight(Node* pNode) { m_pRight = pNode; }

    private:
        Operator m_Operator;
        Node* m_pLeft;
        Node* m_pRight;
    };

};
