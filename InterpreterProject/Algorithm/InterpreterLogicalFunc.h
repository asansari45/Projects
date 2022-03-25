#pragma once

#include "InterpreterBinaryFunc.h"

namespace Interpreter
{
    class Node;
    class ExecutionNodeVisitorServices;
    class ErrorInterface;
    class ValueNode;
    class Lvalue;

    class LogicalFunc : public BinaryFunc
    {
    public:
        Node* Perform(BinaryNode::Operator oper, 
                      Node* pLeft, Node* pRight, 
                      ExecutionNodeVisitorServices* pServices, 
                      ErrorInterface* pErrorInterface) override;

    private:
        bool CheckBinaryTypes(BinaryNode::Operator oper, ValueNode* pA, ValueNode* pB);
        std::type_index GetResultantType(ValueNode* pA, ValueNode* pB);
    };
};

