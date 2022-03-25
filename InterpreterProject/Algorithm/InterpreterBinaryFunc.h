#pragma once

#include "Nodes/InterpreterBinaryNode.h"

namespace Interpreter
{
    class Node;
    class ExecutionNodeVisitorServices;
    class ErrorInterface;
    class ValueNode;
    class Lvalue;

    class BinaryFunc
    {
    public:
        virtual Node* Perform(BinaryNode::Operator oper,
                              Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices, 
                              ErrorInterface* pErrorInterface) = 0;

    protected:
        ValueNode* GetRvalue(Node * pNode, ExecutionNodeVisitorServices* pServices, 
                             ErrorInterface* pErrorInterface);
        Lvalue* GetLvalue(Node* pNode, ValueNode* pRvalue,
                          ExecutionNodeVisitorServices* pServices,
                          ErrorInterface* pErrorInterface);
        bool CheckBinaryArithmeticTypes(BinaryNode::Operator oper, ValueNode* pA, ValueNode* pB);
        bool CheckBinaryLogicalTypes(BinaryNode::Operator oper, ValueNode* pA, ValueNode* pB);
        bool CheckBinaryBitwiseTypes(BinaryNode::Operator oper, ValueNode* pA, ValueNode* pB);
        std::type_index GetResultantArithmeticType(ValueNode* pA, ValueNode* pB);
        std::type_index GetResultantBitwiseType(ValueNode* pA, ValueNode* pB);


    };
};
