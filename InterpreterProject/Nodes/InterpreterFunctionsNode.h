#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class FunctionsNode : public Node
    {
    public:
        FunctionsNode() :
            Node()
        {
        }

        virtual ~FunctionsNode()
        {
        }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor)
        {
            rVisitor.VisitFunctionsNode(this);
        }
    };

};
