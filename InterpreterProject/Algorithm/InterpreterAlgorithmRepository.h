#pragma once
#include <vector>
#include <optional>
#include "Nodes/InterpreterBinaryNode.h"
#include "Tables/InterpreterSymbolTable.h"
#include "Log/InterpreterLog.h"
#include "Values/InterpreterLvalues.h"

namespace Interpreter
{
class SymbolTable;
class ErrorInterface;
class ValueNode;
class ExecutionNodeVisitorServices;

class BinaryFunc
{
public:
    virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices, 
                          ErrorInterface* pErrorInterface) = 0;

protected:
    ValueNode* GetRvalue(Node * pNode, ExecutionNodeVisitorServices* pServices, 
                         ErrorInterface* pErrorInterface);
    Lvalue* GetLvalue(Node* pNode, ValueNode* pRvalue,
                      ExecutionNodeVisitorServices* pServices,
                      ErrorInterface* pErrorInterface);
};

class AlgorithmRepository
{
public:
    static AlgorithmRepository* GetInst();
    static void Shutdown();
    BinaryFunc* Lookup(BinaryNode::Operator oper);

private:
    AlgorithmRepository();
    ~AlgorithmRepository();
    static AlgorithmRepository* m_pInst;
    static BinaryFunc* m_pFunctionTable[];
};


};
