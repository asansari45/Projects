#pragma once
#include <vector>
#include <optional>
#include "InterpreterNodeVisitor.hpp"
#include "InterpreterErrorInterface.h"
#include "Nodes/InterpreterValueNode.h"
#include "Nodes/InterpreterVarNode.h"
#include "Tables/InterpreterSymbolTable.h"

namespace Interpreter
{

class ExecutionNodeVisitor : public NodeVisitor,
                             public ErrorInterface
{
public:
    ExecutionNodeVisitor(SymbolTable* pGlobalSymbols);
    virtual ~ExecutionNodeVisitor();

    virtual void VisitValueNode(Interpreter::ValueNode* pNode);
    virtual void VisitVarNode(Interpreter::VarNode* pNode);
    virtual void VisitBinaryNode(Interpreter::BinaryNode* pNode);
    virtual void VisitPrintNode(Interpreter::PrintNode* pNode);
    virtual void VisitQuitNode(Interpreter::QuitNode* pNode);
    virtual void VisitHelpNode(Interpreter::HelpNode* pNode);
    virtual void VisitClearNode(Interpreter::ClearNode* pNode);
    virtual void VisitLoadNode(Interpreter::LoadNode* pNode);
    virtual void VisitIfNode(Interpreter::IfNode* pNode);
    virtual void VisitWhileNode(Interpreter::WhileNode* pNode);
    virtual void VisitForNode(Interpreter::ForNode* pNode);
    virtual void VisitFunctionCallNode(FunctionCallNode* pNode);
    virtual void VisitFunctionDefNode(FunctionDefNode* pNode);
    virtual void VisitReturnNode(ReturnNode* pNode);
    virtual void VisitDimNode(DimNode* pNode);
    virtual void VisitPwdNode(PwdNode* pNode);
    virtual void VisitFunctionsNode(FunctionsNode* pNode);
    virtual void VisitVarsCmdNode(VarsCmdNode* pNode);
    virtual void VisitVarListNode(VarListNode* pNode);
    virtual void VisitSrandNode(SrandNode* pNode);
    virtual void VisitRandNode(RandNode* pNode);
    virtual void VisitLenNode(LenNode* pNode);
    virtual void VisitBreakNode(BreakNode* pNode);
    virtual void VisitFileNode(FileNode* pNode);

private:
    ValueNode* GetTopOfStackValue(Node* pTop);
        
    bool ExecuteIfNode(IfNode* pIfNode);
    
    // All nodes are clones of the original tree.
    std::vector<Node*> m_Nodes;

    // Function call stack
    std::vector<FunctionDefNode*> m_FunctionCallStack;

    // Global symbol table.
    SymbolTable* m_pGlobalSymbolTable;

};
};
