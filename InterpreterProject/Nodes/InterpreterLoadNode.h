#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class LoadNode : public Node
    {
    public:
        LoadNode();

        virtual ~LoadNode();

        virtual Node* Clone();

        virtual void Free();

        virtual void Accept(Interpreter::NodeVisitor& rVisitor);

        VarNode* GetChild() { return m_pChild; }
        void SetChild(VarNode* pChild) { m_pChild = pChild; }
    private:
        VarNode* m_pChild;
    };

};
