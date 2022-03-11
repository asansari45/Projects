#include "InterpreterVarNode.h"

namespace Interpreter
{
    VarNode::VarNode() :
        Node(),
        m_Name(),
        m_ArraySpecifier()
    {
    }

    VarNode::VarNode(const VarNode& rNode) :
        Node(rNode),
        m_Name(rNode.m_Name),
        m_ArraySpecifier(rNode.m_ArraySpecifier)
    {
    }

    VarNode::~VarNode()
    {
    }

    Node* VarNode::Clone()
    {
        return new VarNode(*this);
    }

    void VarNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitVarNode(this);
    }
};
