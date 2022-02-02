#pragma once
#include "InterpreterNode.h"
#include "InterpreterVarNode.h"

namespace Interpreter
{
    class HelpNode : public Node
    {
    public:
        HelpNode();
        virtual ~HelpNode();
        virtual Node* Clone();
        virtual void Accept(Interpreter::NodeVisitor& rVisitor);
        void SetVar(VarNode* pVarNode) { m_pVarNode = pVarNode; }
        VarNode* GetVar() { return m_pVarNode; }

    private:
        VarNode* m_pVarNode;
    };
};
