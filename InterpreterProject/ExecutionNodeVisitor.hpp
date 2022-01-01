#pragma once
#include <vector>
#include <optional>
#include "InterpreterNodeVisitor.hpp"
#include "InterpreterErrorInterface.h"
#include "InterpreterNode.hpp"
#include "InterpreterSymbolTable.hpp"

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

    std::optional<Value> GetResult()
    {
        if (m_Nodes.size() != 0)
        {
            return GetRvalue(m_Nodes.back());
        }
        return {};
    }

private:
    ValueNode* GetTopOfStackValue(Node* pTop);
        
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
            return pVarNode->IsArray(m_pGlobalSymbolTable, 
                                     m_SymbolTableStack.size() != 0 ? m_SymbolTableStack.back() : nullptr);
        }

        return false;
    }

    // Look at the local symbol table first, and then global symbol table.
    std::optional<Value> GetRvalue(Interpreter::Node* pNode)
    {
        ValueNode* pValueNode = dynamic_cast<ValueNode*>(pNode);
        if (pValueNode != nullptr)
        {
            return pValueNode->GetValue();
        }

        VarNode* pVarNode = dynamic_cast<VarNode*>(pNode);
        if (pVarNode != nullptr)
        {
            return GetSymbolValue(pVarNode);
        }

        return {};
    }

    // Look at the local symbol table first, and then global symbol table.
    std::optional<ArrayValue> GetRarrayValue(Interpreter::Node* pNode)
    {
        assert(IsArray(pNode));

        ValueNode* pValueNode = dynamic_cast<ValueNode*>(pNode);
        if (pValueNode != nullptr)
        {
            return pValueNode->GetArrayValue();
        }

        VarNode* pVarNode = dynamic_cast<VarNode*>(pNode);
        if (pVarNode != nullptr)
        {
            return pVarNode->GetArrayValue(m_pGlobalSymbolTable,
                                           m_SymbolTableStack.size() != 0 ? m_SymbolTableStack.back() : nullptr);
        }

        return {};
    }

    std::optional<Value> GetSymbolValue(VarNode* pVarNode)
    {
        std::string name = pVarNode->GetName();
        std::vector<int> arraySpecifier = pVarNode->GetArraySpecifier();
        SymbolTable* pLocalSymbolTable = m_SymbolTableStack.size() != 0 ? m_SymbolTableStack.back() : nullptr;
        if (pLocalSymbolTable)
        {
            std::optional<Value> value = pLocalSymbolTable->GetSymbolValue(name, arraySpecifier.size() ? &arraySpecifier : nullptr );
            if (value != std::nullopt)
            {
                return value;
            }
        }

        return m_pGlobalSymbolTable->GetSymbolValue(name, arraySpecifier.size() ? &arraySpecifier : nullptr );
    }

    std::optional<std::type_index> GetSymbolType(const std::string s)
    {
        SymbolTable* pLocalSymbolTable = m_SymbolTableStack.size() != 0 ? m_SymbolTableStack.back() : nullptr;
        if (pLocalSymbolTable)
        {
            std::optional<std::type_index> type = pLocalSymbolTable->GetSymbolType(s);
            if (type != std::nullopt)
            {
                return type;
            }
        }

        return m_pGlobalSymbolTable->GetSymbolType(s);
    }

    bool ExecuteIfNode(IfNode* pIfNode);
    
    // All nodes are clones of the original tree.
    std::vector<Node*> m_Nodes;

    // Global symbol table.
    SymbolTable* m_pGlobalSymbolTable;

    // Local symbol tables.
    std::vector<SymbolTable*> m_SymbolTableStack;
};
};
