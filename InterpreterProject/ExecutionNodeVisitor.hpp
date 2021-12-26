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

    std::optional<Value> GetResult()
    {
        if (m_Nodes.size() != 0)
        {
            return GetRvalue(m_Nodes.back());
        }
        return {};
    }

private:
    std::optional<std::type_index> GetLvalueType(Interpreter::Node* pNode)
    {
        Interpreter::VarNode* pVarNode = dynamic_cast<Interpreter::VarNode*>(pNode);
        if (pVarNode != nullptr)
        {
            return GetSymbolType(pVarNode->GetName());
        }

        return {};
    }

    std::optional<VarNode*> GetLvalue(Interpreter::Node* pNode)
    {
        return dynamic_cast<Interpreter::VarNode*>(pNode);
    }

    // Look at the local symbol table first, and then global symbol table.
    std::optional<std::type_index> GetRvalueType(Interpreter::Node* pNode)
    {
        ValueNode* pValueNode = dynamic_cast<ValueNode*>(pNode);
        if (pValueNode != nullptr)
        {
            return typeid(int);
        }

        VarNode* pVarNode = dynamic_cast<VarNode*>(pNode);
        if (pVarNode != nullptr)
        {
            std::string symbol = pVarNode->GetName();
            return GetSymbolType(symbol);
        }

        return {};
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
