#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class ForNode : public Node
    {
    public:
        ForNode() :
            m_pInit(nullptr),
            m_pExpr(nullptr),
            m_pThen(nullptr)
        {
        }

        virtual ~ForNode()
        {
            if (m_pInit)
            {
                m_pInit->Free();
                m_pInit = nullptr;
            }

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
            ForNode* pNode = new ForNode;

            // One node to clone.
            if (m_pInit != nullptr)
            {
                pNode->SetInit(m_pInit->Clone());
            }

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
            rVisitor.VisitForNode(this);
        }

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
