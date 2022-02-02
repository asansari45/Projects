#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class ClearNode : public Node
    {
    public:
        ClearNode();
        virtual ~ClearNode();
        virtual Node* Clone();
        virtual void Free();
        virtual void Accept(Interpreter::NodeVisitor& rVisitor);
    private:
    };

};
