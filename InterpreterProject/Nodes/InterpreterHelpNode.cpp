#include "InterpreterHelpNode.h"

namespace Interpreter
{
    HelpNode::HelpNode():
        m_pVarNode(nullptr)
    {
    }

    HelpNode::~HelpNode()
    {
        delete m_pVarNode;
    }

    Node* HelpNode::Clone()
    {
        HelpNode* pClone = new HelpNode;
        assert(pClone != nullptr);
        if (m_pVarNode != nullptr)
        {
            pClone->SetVar(dynamic_cast<VarNode*>(m_pVarNode->Clone()));
        }
        return pClone;
    }

    void HelpNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitHelpNode(this);
    }
};
