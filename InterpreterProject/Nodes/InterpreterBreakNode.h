#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class BreakNode : public Node
    {
    public:
        BreakNode() :
            Node()
        {
        }

        virtual ~BreakNode()
        {
        }

        virtual Node* Clone()
        {
            return new BreakNode;
        }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor)
        {
            rVisitor.VisitBreakNode(this);
        }
    };

};
