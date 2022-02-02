#pragma once
#include "InterpreterBreakNode.h"

namespace Interpreter
{
    BreakNode::BreakNode() :
        Node()
    {
    }

    BreakNode::~BreakNode()
    {
    }

    Node* BreakNode::Clone()
    {
        return new BreakNode;
    }

    void BreakNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitBreakNode(this);
    }
};
