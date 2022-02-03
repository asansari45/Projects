#include "InterpreterLoadNode.h"
#include "InterpreterVarNode.h"

namespace Interpreter
{

    LoadNode::LoadNode() :
        m_pChild(nullptr)
    {
    }

    LoadNode::~LoadNode()
    {
        if (m_pChild)
        {
            m_pChild->Free();
        }
    }

    Node* LoadNode::Clone()
    {
        return new LoadNode;
    }

    void LoadNode::Free()
    {
        Node::Free();
    }

    void LoadNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitLoadNode(this);
    }
};
