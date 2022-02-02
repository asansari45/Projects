#include "InterpreterLenNode.h"

namespace Interpreter
{
    LenNode::LenNode() :
        Node(),
        m_Name(),
        m_Dim(0)
    {
    }

    LenNode::~LenNode()
    {
    }

    LenNode::LenNode(LenNode& n) :
        Node(n),
        m_Name(n.m_Name),
        m_Dim(n.m_Dim)
    {
    }

    Node* LenNode::Clone()
    {
        return new LenNode(*this);
    }

    void LenNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitLenNode(this);
    }
};
