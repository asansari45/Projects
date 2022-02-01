#include "InterpreterNode.h"

namespace Interpreter
{

    Node::Node() :
        m_pNext(nullptr),
        m_File(),
        m_Line(0),
        m_Column(0)
    {
        Context* pContext = contextStack.size() != 0 ? contextStack.back() : nullptr;
        if (pContext != nullptr)
        {
            m_File = pContext->GetFile();
            m_Line = pContext->GetLine();
            m_Column = pContext->GetColumn();
        }
    }

    Node::Node(const Node& rNode) :
        m_pNext(nullptr),
        m_File(rNode.m_File),
        m_Line(rNode.m_Line),
        m_Column(rNode.m_Column)
    {
    }

    // Clone only node and sons.
    Node* Node::Clone()
    {
        Node* pThis = new Node;
        assert(pThis != nullptr);
        return pThis;
    }

    Node* Node::CloneList()
    {
        Node* pCopyStart = nullptr;
        Node* pCopyCurr = nullptr;
        for (Node* pCurr = this; pCurr != nullptr; pCurr = pCurr->GetNext())
        {
            Node* pCopy = pCurr->Clone();
            if (pCopyStart == nullptr)
            {
                pCopyStart = pCopy;
                pCopyCurr = pCopy;
            }
            else
            {
                pCopyCurr->SetNext(pCopy);
                pCopyCurr = pCopy;
            }
        }

        return pCopyStart;
    }

    void Node::FreeList()
    {
        Node* pCurr = this;
        while (pCurr != nullptr)
        {
            Node* pNext = pCurr->GetNext();
            pCurr->Free();
            pCurr = pNext;
        }
    }

    void Node::Free()
    {
        delete this;
    }

    void Node::Accept(NodeVisitor& rVisitor)
    {
        rVisitor.VisitNode(this);
    }

};
