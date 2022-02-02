#pragma once
#include "InterpreterNode.h"
namespace Interpreter
{

    class QuitNode : public Node
    {
    public:
        QuitNode();
        virtual ~QuitNode();
        virtual Node* Clone();
        virtual void Accept(Interpreter::NodeVisitor& rVisitor);
    };
};
