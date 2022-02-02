#pragma once
#include "InterpreterNode.h"
#include "InterpreterVarNode.h"

namespace Interpreter
{

    class FunctionCallNode : public Node
    {
    public:
        FunctionCallNode();
        virtual ~FunctionCallNode();
        virtual Node* Clone();
        virtual void Free();
        virtual void Accept(Interpreter::NodeVisitor& rVisitor);
        int GetInputVarCount();
        Node* GetInputVars() { return m_pInputVars; }
        void SetInputVars(Node* pInputVars) { m_pInputVars = pInputVars; }

        VarNode* GetNameVar() { return m_pNameVar; }
        void SetNameVar(VarNode* pVarNode) { m_pNameVar = pVarNode; }

    private:
        VarNode* m_pNameVar;
        Node* m_pInputVars;
    };

};
