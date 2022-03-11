#pragma once
#include <vector>
#include <optional>
#include "InterpreterNodeVisitor.hpp"
#include "InterpreterErrorInterface.h"
#include "InterpreterExecutionNodeVisitorServices.h"
#include "Nodes/InterpreterValueNode.h"
#include "Nodes/InterpreterVarNode.h"
#include "Tables/InterpreterSymbolTable.h"

namespace Interpreter
{

class ExecutionNodeVisitor : public NodeVisitor,
                             public ErrorInterface,
                             public ExecutionNodeVisitorServices
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
    virtual void VisitFileOpenNode(FileOpenNode* pNode);
    virtual void VisitFileCloseNode(FileCloseNode* pNode);
    virtual void VisitFileReadNode(FileReadNode* pNode);
    virtual void VisitFileWriteNode(FileWriteNode* pNode);
    virtual void VisitFileEofNode(FileEofNode* pNode);

#if 0
    std::optional<Value> GetResult()
    {
        if (m_Nodes.size() != 0)
        {
            return GetRvalue(m_Nodes.back());
        }
        return {};
    }
#endif

private:

    ValueNode* GetTopOfStackValue(Node* pTop);

#if 0
    bool IsArray(Node* pNode)
    {
        ValueNode* pValueNode = dynamic_cast<ValueNode*>(pNode);
        if (pValueNode != nullptr)
        {
            return pValueNode->IsArray();
        }

        VarNode* pVarNode = dynamic_cast<VarNode*>(pNode);
        if (pVarNode != nullptr)
        {
            if (pVarNode->GetArraySpecifier().size() != 0)
            {
                return false;
            }

            assert(pVarNode->IsSymbolPresent());
            SymbolTable::SymbolInfo* pInfo = pVarNode->GetSymbolInfo();
            assert(pInfo != nullptr);
            assert(!pInfo->m_IsRef);
            return pInfo->m_IsArray;
        }

        return false;
    }
#endif

    bool ExecuteIfNode(IfNode* pIfNode);

    std::optional<std::string> GetFileOpenString(Node* pNode);
    File* GetFile(Node* pNode);

    // Symbol services
    bool CreateSymbol(std::string name, SymbolTable::SymbolInfo* pSymbolInfo) override;
    bool IsSymbolPresent(std::string name) override;
    SymbolTable::SymbolInfo* ReadSymbol(std::string name, bool getroot=false) override;
    bool UpdateSymbol(std::string name, SymbolTable::SymbolInfo* pSymbolInfo) override;
    void DeleteSymbol(SymbolTable::SymbolInfo* pSymbolInfo) override;
    SymbolTable* GetNoSymbolTable() override;
    
    // All nodes are clones of the original tree.
    std::vector<Node*> m_Nodes;

    // Function call stack
    std::vector<FunctionDefNode*> m_FunctionCallStack;

    // Global symbol table.
    SymbolTable* m_pGlobalSymbolTable;

};
};
