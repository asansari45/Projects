#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class VarNode : public Node
    {
    public:
        VarNode();

        VarNode(const VarNode& rNode);

        virtual ~VarNode();

        virtual Node* Clone();

        virtual void Accept(Interpreter::NodeVisitor& rVisitor);

        std::string GetName() { return m_Name; }
        void SetName(std::string n) { m_Name = n; }

        std::vector<int> GetArraySpecifier() { return m_ArraySpecifier; }
        void SetArraySpecifier(std::vector<int> arraySpecifier) { m_ArraySpecifier = arraySpecifier; }
    private:
        std::string m_Name;
        std::vector<int> m_ArraySpecifier;
    };

};
