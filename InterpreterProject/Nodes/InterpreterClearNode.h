#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class ClearNode : public Node
    {
    public:
        ClearNode()
        {
        }

        virtual ~ClearNode()
        {
        }

        virtual Node* Clone()
        {
            return new ClearNode;
        }

        virtual void Free()
        {
            Node::Free();
        }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor)
        {
            rVisitor.VisitClearNode(this);
        }
    private:
    };

};
