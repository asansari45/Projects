#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class RandNode : public Node
    {
    public:
        RandNode();
        virtual ~RandNode();
        virtual Node* Clone();
        virtual void Accept(Interpreter::NodeVisitor& rVisitor);
    };

};
