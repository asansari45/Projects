#pragma once
#include <vector>
#include <optional>
#include "InterpreterNodeVisitor.hpp"
#include "InterpreterErrorInterface.h"
#include "InterpreterValueNode.h"
#include "InterpreterVarNode.h"
#include "InterpreterSymbolTable.h"

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
            if (pVarNode->GetArraySpecifier().size() != 0)
            {
                return false;
            }

            assert(pVarNode->IsSymbolPresent());
            std::optional<SymbolTable::SymbolInfo> info = pVarNode->GetSymbolInfo();
            assert(info != std::nullopt);
            assert(!info->m_IsRef);
            return info->m_IsArray;
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
            assert(pVarNode->IsSymbolPresent());
            std::optional<SymbolTable::SymbolInfo> info = pVarNode->GetSymbolInfo();
            assert(info != std::nullopt);
            assert(!info->m_IsRef);
            if (pVarNode->GetArraySpecifier().size() != 0)
            {
                if (!info->m_IsArray)
                {
                    return {};
                }
                return info->m_ArrayValue.GetValue(pVarNode->GetArraySpecifier());
            }

            if (info->m_IsArray)
            {
                return {};
            }

            return info->m_Value;
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
            assert(pVarNode->IsSymbolPresent());
            std::optional<SymbolTable::SymbolInfo> info = pVarNode->GetSymbolInfo();
            assert(info != std::nullopt);
            assert(!info->m_IsRef);
            assert(info->m_IsArray);
            return info->m_ArrayValue;
        }

        return {};
    }

    bool ExecuteIfNode(IfNode* pIfNode);
    
    // All nodes are clones of the original tree.
    std::vector<Node*> m_Nodes;

    // Function call stack
    std::vector<FunctionDefNode*> m_FunctionCallStack;

    // Global symbol table.
    SymbolTable* m_pGlobalSymbolTable;

};
};
