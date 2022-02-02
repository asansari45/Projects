#include "InterpreterRefNode.h"

namespace Interpreter
{
    RefNode::RefNode() :
        Node(),
        m_Name()
    {
    }

    RefNode::RefNode(const RefNode& rNode) :
        Node(rNode),
        m_Name(rNode.m_Name)
    {
    }

    RefNode::~RefNode()
    {
    }

    Node* RefNode::Clone()
    {
        return new RefNode(*this);
    }
};
