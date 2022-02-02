#include "InterpreterQuitNode.h"

namespace Interpreter
{
    QuitNode::QuitNode()
    {
    }

    QuitNode::~QuitNode()
    {
    }

    Node* QuitNode::Clone()
    {
        return new QuitNode;
    }

    void QuitNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitQuitNode(this);
    }

};
