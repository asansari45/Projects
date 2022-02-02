#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class RefNode : public Node
    {
    public:
        RefNode();
        RefNode(const RefNode& rNode);
        virtual ~RefNode();
        virtual Node* Clone();
        std::string GetName() { return m_Name; }
        void SetName(std::string n) { m_Name = n; }

    private:
        std::string m_Name;
    };
};
