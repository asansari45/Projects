#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class DimNode : public Node
    {
    public:
        DimNode() :
            Node(),
            m_pDim(nullptr)
        {
        }

        Node* Clone()
        {
            DimNode* pClone = new DimNode;
            for (Node* pNode = m_pDim; pNode != nullptr; pNode = pNode->GetNext())
            {
                pClone->AddDim(pNode->Clone());
            }
            return pClone;
        }

        virtual ~DimNode()
        {
            m_pDim->FreeList();
            m_pDim = nullptr;
        }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor)
        {
            rVisitor.VisitDimNode(this);
        }

        void AddDim(Node* pDim)
        {
            if (m_pDim == nullptr)
            {
                m_pDim = pDim;
                return;
            }

            Node* pNode = m_pDim;
            for (; pNode->GetNext() != nullptr; pNode = pNode->GetNext());
            pNode->SetNext(pDim);
        }

        Node* GetDim()
        {
            return m_pDim;
        }

        void ClearDim()
        {
            m_pDim->FreeList();
            m_pDim = nullptr;
        }

    private:
        Node* m_pDim;
    };

};
