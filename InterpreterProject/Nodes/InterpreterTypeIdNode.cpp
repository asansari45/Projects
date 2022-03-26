#include "InterpreterTypeIdNode.h"

namespace Interpreter
{
    TypeIdNode::TypeIdNode() :
        Node(),
        m_pExpr(nullptr)
    {
    }

    TypeIdNode::~TypeIdNode()
    {
        if (m_pExpr)
        {
            m_pExpr->Free();
        }
    }

    Node* TypeIdNode::Clone()
    {
        return new TypeIdNode;
    }

    void TypeIdNode::Free()
    {
        Node::Free();
    }

    void TypeIdNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitTypeIdNode(this);
    }
};
