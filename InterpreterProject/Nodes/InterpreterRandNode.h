#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class RandNode : public Node
    {
    public:
        RandNode() :
            Node()
        {
        }

        virtual ~RandNode()
        {
        }

        virtual Node* Clone()
        {
            return new RandNode;
        }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor)
        {
            rVisitor.VisitRandNode(this);
        }
    };

};
