#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

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

};
