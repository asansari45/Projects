#include "InterpreterStopNode.h"

namespace Interpreter
{

    StopNode::StopNode(Reason reason) :
        Node(),
        m_Reason(reason)
    {
    }

    StopNode::~StopNode()
    {
    }

    StopNode::StopNode(StopNode& n) :
        Node(n),
        m_Reason(n.m_Reason)
    {
    }

    Node* StopNode::Clone()
    {
        return new StopNode(*this);
    }

};
