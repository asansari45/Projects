#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class BreakNode : public Node
    {
    public:
        BreakNode();
        virtual ~BreakNode();
        virtual Node* Clone();
        virtual void Accept(Interpreter::NodeVisitor& rVisitor);
    };

};
