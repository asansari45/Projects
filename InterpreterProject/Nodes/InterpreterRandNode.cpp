#include "InterpreterRandNode.h"

namespace Interpreter
{
    RandNode::RandNode() :
        Node()
    {
    }

    RandNode::~RandNode()
    {
    }

    Node* RandNode::Clone()
    {
        return new RandNode;
    }

    void RandNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitRandNode(this);
    }
};
