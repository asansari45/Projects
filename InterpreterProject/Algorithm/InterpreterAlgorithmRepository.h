#pragma once
#include <vector>
#include <optional>
#include "InterpreterBinaryNode.h"
#include "InterpreterSymbolTable.h"
#include "InterpreterLog.h"
#include "InterpreterLvalues.h"
#include "InterpreterRvalue.h"

namespace Interpreter
{
class SymbolTable;
class ErrorInterface;

class BinaryFunc
{
public:
    virtual Node* Perform(Node* pLeft, Node* pRight, ErrorInterface* pErrorInterface) = 0;

protected:
    std::optional<Rvalue> GetRvalue(Node * pNode, ErrorInterface* pErrorInterface);
    Lvalue* GetLvalue(Node* pNode, Rvalue& rRvalue,
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
