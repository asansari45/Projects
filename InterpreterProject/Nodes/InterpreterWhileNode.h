#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{
    class WhileNode : public Node
    {
    public:
        WhileNode();
        virtual ~WhileNode();
        virtual Node* Clone();
        virtual void Free();
        virtual void Accept(Interpreter::NodeVisitor& rVisitor);
        Node* GetExpr() { return m_pExpr; }
        void SetExpr(Node* pExpr) { m_pExpr = pExpr; }

        Node* GetThen() { return m_pThen; }
        void SetThen(Node* pThen) { m_pThen = pThen; }

    private:
        Node* m_pExpr;
        Node* m_pThen;
    };
};
