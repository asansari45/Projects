#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class IfNode : public Node
    {
    public:
        IfNode();
        virtual ~IfNode();
        virtual Node* Clone();
        virtual void Free();
        virtual void Accept(Interpreter::NodeVisitor& rVisitor);
        Node* GetExpr() { return m_pExpr; }
        void SetExpr(Node* pExpr) { m_pExpr = pExpr; }

        Node* GetThen() { return m_pThen; }
        void SetThen(Node* pThen) { m_pThen = pThen; }

        IfNode* GetElif() { return m_pElif; }
        void SetElif(IfNode* pElif) { m_pElif = pElif; }

        Node* GetElse() { return m_pElse; }
        void SetElse(Node* pElse) { m_pElse = pElse; }

    private:
        Node* m_pExpr;
        Node* m_pThen;
        IfNode* m_pElif;
        Node* m_pElse;
    };

};
