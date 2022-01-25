#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class SrandNode : public Node
    {
    public:
        SrandNode() :
            Node(),
            m_pExpr(nullptr)
        {
        }

        virtual ~SrandNode()
        {
        }

        virtual Node* Clone()
        {
            return new SrandNode;
        }

        virtual void Free()
        {
            if (m_pExpr)
            {
                m_pExpr->Free();
            }
            Node::Free();
        }

        void SetExpr(Node* pExpr)
        {
            m_pExpr = pExpr;
        }

        Node* GetExpr()
        {
            return m_pExpr;
        }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor)
        {
            rVisitor.VisitSrandNode(this);
        }
    private:
        Node* m_pExpr;
    };

};
