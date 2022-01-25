#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class LoadNode : public Node
    {
    public:
        LoadNode() :
            m_pChild(nullptr)
        {
        }

        virtual ~LoadNode()
        {
        }

        virtual Node* Clone()
        {
            return new LoadNode;
        }

        virtual void Free()
        {
            if (m_pChild)
            {
                m_pChild->Free();
            }
            Node::Free();
        }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor)
        {
            rVisitor.VisitLoadNode(this);
        }

        VarNode* GetChild() { return m_pChild; }
        void SetChild(VarNode* pChild) { m_pChild = pChild; }
    private:
        VarNode* m_pChild;
    };

};