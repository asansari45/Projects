#pragma once
#include "InterpreterNode.h"
namespace Interpreter
{

    class QuitNode : public Node
    {
    public:
        QuitNode()
        {
        }

        virtual ~QuitNode()
        {
        }

        virtual Node* Clone()
        {
            return new QuitNode;
        }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor)
        {
            rVisitor.VisitQuitNode(this);
        }
    };
};
