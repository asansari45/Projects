#pragma once
#include "InterpreterClearNode.h"

namespace Interpreter
{

    ClearNode::ClearNode()
    {
    }

    ClearNode::~ClearNode()
    {
    }

    Node* ClearNode::Clone()
    {
        return new ClearNode;
    }

    void ClearNode::Free()
    {
        Node::Free();
    }

    void ClearNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitClearNode(this);
    }
};
