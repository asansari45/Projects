#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class SrandNode : public Node
    {
    public:
        SrandNode();

        virtual ~SrandNode();

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
