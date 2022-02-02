#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class LenNode : public Node
    {
    public:
        LenNode();
        virtual ~LenNode();
        LenNode(LenNode& n);
        virtual Node* Clone();
        std::string GetName() { return m_Name; }
        void SetName(std::string n) { m_Name = n; }
        int GetDim() { return m_Dim; }
        void SetDim(int dim) { m_Dim = dim; }
        virtual void Accept(Interpreter::NodeVisitor& rVisitor);
    private:
        std::string m_Name;
        int m_Dim;
    };

};
