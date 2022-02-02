#pragma once
#include "InterpreterDimNode.h"

namespace Interpreter
{

    DimNode::DimNode() :
        Node(),
        m_pDim(nullptr)
    {
    }

    Node* DimNode::Clone()
    {
        DimNode* pClone = new DimNode;
        for (Node* pNode = m_pDim; pNode != nullptr; pNode = pNode->GetNext())
        {
            pClone->AddDim(pNode->Clone());
        }
        return pClone;
    }

    DimNode::~DimNode()
    {
        m_pDim->FreeList();
        m_pDim = nullptr;
    }

    void DimNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitDimNode(this);
    }

    void DimNode::AddDim(Node* pDim)
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

    void DimNode::ClearDim()
    {
        m_pDim->FreeList();
        m_pDim = nullptr;
    }
};
