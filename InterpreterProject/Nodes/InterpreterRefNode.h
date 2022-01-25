#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class RefNode : public Node
    {
    public:
        RefNode() :
            Node(),
            m_Name()
        {
        }

        RefNode(const RefNode& rNode) :
            Node(rNode),
            m_Name(rNode.m_Name)
        {
        }

        virtual ~RefNode()
        {
        }

        virtual Node* Clone()
        {
            return new RefNode(*this);
        }

        std::string GetName() { return m_Name; }
        void SetName(std::string n) { m_Name = n; }

    private:
        std::string m_Name;
    };
};
