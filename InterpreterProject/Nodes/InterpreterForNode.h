#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class ForNode : public Node
    {
    public:
        ForNode();

        virtual ~ForNode();

        virtual Node* Clone();
        virtual void Free();

        virtual void Accept(Interpreter::NodeVisitor& rVisitor);

        Node* GetInit() { return m_pInit; }
        void SetInit(Node* pExpr) { m_pInit = pExpr; }

        Node* GetExpr() { return m_pExpr; }
        void SetExpr(Node* pExpr) { m_pExpr = pExpr; }

        Node* GetThen() { return m_pThen; }
        void SetThen(Node* pThen) { m_pThen = pThen; }

    private:
        Node* m_pInit;
        Node* m_pExpr;
        Node* m_pThen;
    };

};
