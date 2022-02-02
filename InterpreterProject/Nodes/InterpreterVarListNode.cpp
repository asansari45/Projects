#include "InterpreterVarListNode.h"

namespace Interpreter
{

    VarListNode::VarListNode() :
        Node(),
        m_pList(nullptr)
    {
    }

    VarListNode::VarListNode(const VarListNode& rNode) :
        Node(rNode),
        m_pList(rNode.m_pList)
    {
    }

    VarListNode::~VarListNode()
    {
        if (m_pList != nullptr)
        {
            m_pList->FreeList();
        }
    }

    Node* VarListNode::Clone()
    {
        VarListNode* pNode = new VarListNode;
        pNode->SetList(m_pList->CloneList());
        return pNode;
    }

    void VarListNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitVarListNode(this);
    }

    void VarListNode::Append(Node* pNode)
    {
        if (m_pList == nullptr)
        {
            m_pList = pNode;
            return;
        }

        Node* pEnd = m_pList;
        for (; pEnd->GetNext() != nullptr; pEnd = pEnd->GetNext());
        pEnd->SetNext(pNode);
    }

    int VarListNode::GetListCount()
    {
        int count = 0;
        for (Node* pNode = m_pList; pNode; pNode = pNode->GetNext())
        {
            count++;
        }
        return count;
    }
};
