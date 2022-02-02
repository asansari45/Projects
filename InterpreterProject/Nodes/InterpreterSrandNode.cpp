#include "InterpreterSrandNode.h"

namespace Interpreter
{
    SrandNode::SrandNode() :
        Node(),
        m_pExpr(nullptr)
    {
    }

    SrandNode::~SrandNode()
    {
        if (m_pExpr)
        {
            m_pExpr->Free();
        }
    }

    Node* SrandNode::Clone()
    {
        return new SrandNode;
    }

    void SrandNode::Free()
    {
        Node::Free();
    }

    void SrandNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitSrandNode(this);
    }
};
