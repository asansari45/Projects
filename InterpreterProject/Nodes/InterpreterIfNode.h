#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class IfNode : public Node
    {
    public:
        IfNode() :
            m_pExpr(nullptr),
            m_pThen(nullptr),
            m_pElif(nullptr),
            m_pElse(nullptr)
        {
        }

        virtual ~IfNode()
        {
        }

        virtual Node* Clone()
        {
            IfNode* pNode = new IfNode;

            // One node to clone.
            if (m_pExpr != nullptr)
            {
                pNode->SetExpr(m_pExpr->Clone());
            }

            if (m_pThen != nullptr)
            {
                pNode->SetThen(m_pThen->CloneList());
            }

            // Several nodes to clone.
            if (m_pElif != nullptr)
            {
                pNode->SetElif(dynamic_cast<Interpreter::IfNode*>(m_pElif->CloneList()));
            }

            // Several nodes to clone.
            if (m_pElse != nullptr)
            {
                pNode->SetElse(m_pElse->CloneList());
            }

            return pNode;
        }

        virtual void Free()
        {
            if (m_pExpr)
            {
                m_pExpr->Free();
                m_pExpr = nullptr;
            }

            if (m_pElse)
            {
                m_pElse->FreeList();
                m_pElse = nullptr;
            }

            if (m_pElif)
            {
                m_pElif->FreeList();
                m_pElif = nullptr;
            }

            if (m_pThen)
            {
                m_pThen->FreeList();
                m_pThen = nullptr;
            }
            Node::Free();
        }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor)
        {
            rVisitor.VisitIfNode(this);
        }

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
