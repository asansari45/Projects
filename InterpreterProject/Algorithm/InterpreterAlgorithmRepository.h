#pragma once
#include <vector>
#include <optional>
#include "Nodes/InterpreterBinaryNode.h"
#include "Tables/InterpreterSymbolTable.h"
#include "Log/InterpreterLog.h"
#include "Values/InterpreterLvalues.h"

namespace Interpreter
{

class BinaryFunc;

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
