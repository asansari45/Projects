#include "InterpreterPrintNode.h"

namespace Interpreter
{

    PrintNode::PrintNode() :
        m_pChild(nullptr)
    {
    }

    PrintNode::~PrintNode()
    {
        if (m_pChild)
        {
            m_pChild->FreeList();
            m_pChild = nullptr;
        }
    }

    Node* PrintNode::Clone()
    {
        PrintNode* pNode = new PrintNode;
        if (m_pChild)
        {
            pNode->SetChild(m_pChild->CloneList());
        }
        return pNode;
    }

    void PrintNode::Free()
    {
        Node::Free();
    }

    void PrintNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitPrintNode(this);
    }

};
