#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    // Returns the type as a ValueNode(string) on the stack.
    class TypeIdNode : public Node
    {
    public:
        TypeIdNode();

        virtual ~TypeIdNode();

        virtual Node* Clone();

        virtual void Free();

        void SetExpr(Node* pExpr)
        {
            m_pExpr = pExpr;
        }

        Node* GetExpr()
        {
            return m_pExpr;
        }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor);
    private:
        Node* m_pExpr;
    };

};
