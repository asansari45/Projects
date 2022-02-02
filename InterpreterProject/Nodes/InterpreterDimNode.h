#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class DimNode : public Node
    {
    public:
        DimNode();
        Node* Clone();
        virtual ~DimNode();
        virtual void Accept(Interpreter::NodeVisitor& rVisitor);
        void AddDim(Node* pDim);
        Node* GetDim()
        {
            return m_pDim;
        }

        void ClearDim();

    private:
        Node* m_pDim;
    };

};
