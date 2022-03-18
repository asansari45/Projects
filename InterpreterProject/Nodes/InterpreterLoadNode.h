#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{
    class ValueNode;

    class LoadNode : public Node
    {
    public:
        LoadNode();

        virtual ~LoadNode();

        virtual Node* Clone();

        virtual void Free();

        virtual void Accept(Interpreter::NodeVisitor& rVisitor);

        ValueNode* GetChild() { return m_pChild; }
        void SetChild(ValueNode* pChild) { m_pChild = pChild; }
    private:
        ValueNode* m_pChild;
    };
};
