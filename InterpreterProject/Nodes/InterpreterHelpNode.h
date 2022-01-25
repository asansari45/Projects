#pragma once
#include "InterpreterNode.h"
#include "InterpreterVarNode.h"

namespace Interpreter
{

    class HelpNode : public Node
    {
    public:
        HelpNode()
        {
        }

        virtual ~HelpNode()
        {
        }

        virtual Node* Clone()
        {
            HelpNode* pClone = new HelpNode;
            assert(pClone != nullptr);
            if (m_pVarNode != nullptr)
            {
                pClone->SetVar(dynamic_cast<VarNode*>(m_pVarNode->Clone()));
            }
            return pClone;
        }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor)
        {
            rVisitor.VisitHelpNode(this);
        }

        void SetVar(VarNode* pVarNode) { m_pVarNode = pVarNode; }
        VarNode* GetVar() { return m_pVarNode; }

    private:
        VarNode* m_pVarNode;
    };

};
