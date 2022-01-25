#pragma once

#include "InterpreterNode.h"

namespace Interpreter
{

    class ReturnNode : public Node
    {
    public:
        ReturnNode() :
            Node(),
            m_pExpr(nullptr)
        {
        }

        virtual ~ReturnNode()
        {
        }

        virtual Node* Clone()
        {
            ReturnNode* pNode = new ReturnNode;
            if (m_pExpr != nullptr)
            {
                pNode->SetExpr(m_pExpr->CloneList());
            }
            return pNode;
        }

        virtual void Free()
        {
            if (m_pExpr != nullptr)
            {
                m_pExpr->Free();
                m_pExpr = nullptr;
            }
            Node::Free();
        }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor)
        {
            rVisitor.VisitReturnNode(this);
        }

        Node* GetExpr() { return m_pExpr; }
        void SetExpr(Node* pExpr) { m_pExpr = pExpr; }

    private:
        Node* m_pExpr;
    };
};
