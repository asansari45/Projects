#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class LenNode : public Node
    {
    public:
        LenNode() :
            Node(),
            m_Name(),
            m_Dim(0)
        {
        }

        virtual ~LenNode()
        {
        }

        LenNode(LenNode& n) :
            Node(n),
            m_Name(n.m_Name),
            m_Dim(n.m_Dim)
        {
        }

        virtual Node* Clone()
        {
            return new LenNode(*this);
        }

        std::string GetName() { return m_Name; }
        void SetName(std::string n) { m_Name = n; }
        int GetDim() { return m_Dim; }
        void SetDim(int dim) { m_Dim = dim; }

        virtual void Accept(Interpreter::NodeVisitor& rVisitor)
        {
            rVisitor.VisitLenNode(this);
        }
    private:
        std::string m_Name;
        int m_Dim;
    };

};
