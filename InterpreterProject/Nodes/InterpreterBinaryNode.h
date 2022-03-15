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
            ARY,
            OR,
            AND,
            XOR,
            LSH,
            RSH,
            BNEG,
            LOR,
            LAND
        };

        BinaryNode();

        BinaryNode(const BinaryNode& rNode);

        virtual ~BinaryNode();

        virtual Node* Clone();

        virtual void Free();

        virtual void Accept(Interpreter::NodeVisitor& rVisitor);

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
