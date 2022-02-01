#pragma once
#include <assert.h>
#include "InterpreterNode.h"

namespace Interpreter
{
    class ReturnNode : public Node
    {
    public:

        ReturnNode();
        virtual ~ReturnNode();
        virtual Node* Clone();
        virtual void Free();
        virtual void Accept(Interpreter::NodeVisitor& rVisitor);

        void SetExpr(Node* pExpr) { assert(m_pExpr == nullptr);  m_pExpr = pExpr; }
        Node* GetExpr() { return m_pExpr; }

    private:
        Node* m_pExpr;
    };

};
