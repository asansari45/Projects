#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class PrintNode : public Node
    {
    public:
        PrintNode();
        virtual ~PrintNode();
        virtual Node* Clone();
        virtual void Free();
        virtual void Accept(Interpreter::NodeVisitor& rVisitor);
        Node* GetChild() { return m_pChild; }
        void SetChild(Node* pChild) { m_pChild = pChild; }
    private:
        Node* m_pChild;
    };

};
