#pragma once
#include "InterpreterNode.h"
namespace Interpreter
{

    class VarListNode : public Node
    {
    public:
        VarListNode() :
            Node(),
            m_pList(nullptr)
        {
        }

        VarListNode(const VarListNode& rNode) :
            Node(rNode),
            m_pList(rNode.m_pList)
        {
        }

        virtual ~VarListNode()
        {
            if (m_pList != nullptr)
            {
                m_pList->FreeList();
            }
        }

        virtual Node* Clone()
        {
            VarListNode* pNode = new VarListNode;
            pNode->SetList(m_pList->CloneList());
            return pNode;
        }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor)
        {
            rVisitor.VisitVarListNode(this);
        }

        void SetList(Node* pList)
        {
            m_pList = pList;
        }

        Node* GetList()
        {
            return m_pList;
        }

        void Append(Node* pNode)
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

        int GetListCount()
        {
            int count = 0;
            for (Node* pNode = m_pList; pNode; pNode = pNode->GetNext())
            {
                count++;
            }
            return count;
        }
    private:

        // The list of nodes.
        Node* m_pList;
    };
};
