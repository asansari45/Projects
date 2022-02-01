#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class WhileNode : public Node
    {
    public:
        WhileNode() :
            m_pExpr(nullptr),
            m_pThen(nullptr)
        {
        }

        virtual ~WhileNode()
        {
            if (m_pExpr)
            {
                m_pExpr->Free();
                m_pExpr = nullptr;
            }

            if (m_pThen)
            {
                m_pThen->FreeList();
                m_pThen = nullptr;
            }
        }

        virtual Node* Clone()
        {
            WhileNode* pNode = new WhileNode;

            // One node to clone.
            if (m_pExpr != nullptr)
            {
                pNode->SetExpr(m_pExpr->Clone());
            }

            // Several nodes to clone.
            if (m_pThen != nullptr)
            {
                pNode->SetThen(m_pThen->CloneList());
            }

            return pNode;
        }

        virtual void Free()
        {
            Node::Free();
        }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor)
        {
            rVisitor.VisitWhileNode(this);
        }

        Node* GetExpr() { return m_pExpr; }
        void SetExpr(Node* pExpr) { m_pExpr = pExpr; }

        Node* GetThen() { return m_pThen; }
        void SetThen(Node* pThen) { m_pThen = pThen; }

    private:
        Node* m_pExpr;
        Node* m_pThen;
    };
};
