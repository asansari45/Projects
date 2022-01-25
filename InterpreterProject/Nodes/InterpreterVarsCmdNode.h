#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class VarsCmdNode : public Node
    {
    public:
        VarsCmdNode() :
            Node()
        {
        }

        virtual ~VarsCmdNode()
        {
        }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor)
        {
            rVisitor.VisitVarsCmdNode(this);
        }
    };
};
