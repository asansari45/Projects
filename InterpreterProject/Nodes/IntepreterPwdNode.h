#pragma once
#include "InterpreterNode.h"
class PwdNode : public Node
{
public:
    PwdNode() :
        Node()
    {
    }

    virtual ~PwdNode()
    {
    }

    virtual void Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitPwdNode(this);
    }
};

