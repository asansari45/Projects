#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class PrintNode : public Node
    {
    public:
        PrintNode() :
            m_pChild(nullptr)
        {
        }

        virtual ~PrintNode()
        {
        }

        virtual Node* Clone()
        {
            PrintNode* pNode = new PrintNode;
            if (m_pChild)
            {
                pNode->SetChild(m_pChild->CloneList());
            }
            return pNode;
        }

        virtual void Free()
        {
            if (m_pChild)
            {
                m_pChild->FreeList();
                m_pChild = nullptr;
            }
            Node::Free();
        }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor)
        {
            rVisitor.VisitPrintNode(this);
        }

        Node* GetChild() { return m_pChild; }
        void SetChild(Node* pChild) { m_pChild = pChild; }
    private:
        Node* m_pChild;
    };

};
