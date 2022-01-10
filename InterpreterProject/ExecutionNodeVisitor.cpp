#include <assert.h>
#include <sstream>
#include <filesystem>
#include "ExecutionNodeVisitor.hpp"
#include "InterpreterNode.hpp"
#include "InterpreterSymbolTable.hpp"
#include "InterpreterAlgorithmRepository.h"
#include "InterpreterDriver.hpp"
#include "InterpreterFunctionTable.h"
#include "InterpreterLog.h"

namespace Interpreter
{

    ExecutionNodeVisitor::ExecutionNodeVisitor(SymbolTable* pGlobalSymbols) :
        ErrorInterface(),
        m_Nodes(),
        m_pGlobalSymbolTable(pGlobalSymbols),
        m_SymbolTableStack()
    {
    }

    ExecutionNodeVisitor::~ExecutionNodeVisitor()
    {
        // All nodes are cloned.
        for (auto i = 0U; i < m_Nodes.size(); i++)
        {
            delete m_Nodes[i];
        }
    }

    void ExecutionNodeVisitor::VisitValueNode(Interpreter::ValueNode* pNode)
    {
        m_Nodes.push_back(pNode->Clone());
    }

    void ExecutionNodeVisitor::VisitVarNode(Interpreter::VarNode* pNode)
    {
        m_Nodes.push_back(pNode->Clone());
    }

    void ExecutionNodeVisitor::VisitBinaryNode(Interpreter::BinaryNode* pNode)
    {
        // Visit children first.
        if (pNode->GetLeft() != nullptr)
        {
            pNode->GetLeft()->Accept(*this);
        }

        if (pNode->GetRight() != nullptr)
        {
            pNode->GetRight()->Accept(*this);
        }

        Node* pRight = nullptr;
        Node* pLeft = nullptr;
        if (pNode->GetLeft() && pNode->GetRight())
        {
            pRight = m_Nodes.size() != 0 ? m_Nodes.back() : nullptr;
            if (pRight == nullptr)
            {
                // Could not pop, error.
                return;
            }
            m_Nodes.pop_back();

            pLeft = m_Nodes.size() != 0 ? m_Nodes.back() : nullptr;
            if (pLeft == nullptr)
            {
                return;
            }
            m_Nodes.pop_back();
        }
        else if (pNode->GetLeft())
        {
            pLeft = m_Nodes.back();
            m_Nodes.pop_back();
        }

        // rnode operator lnode
        BinaryFunc* pFunc = AlgorithmRepository::GetInst()->Lookup(pNode->GetOperator());
        Node* pResult = pFunc->Perform(pLeft, pRight, m_pGlobalSymbolTable,
            m_SymbolTableStack.size() != 0 ? m_SymbolTableStack.back() : nullptr,
            this);

        // Done with left and right
        delete pLeft;
        delete pRight;

        // Push a value node with the result on the stack.
        if (pResult != nullptr)
        {
            m_Nodes.push_back(pResult);
        }
    }

    void ExecutionNodeVisitor::VisitPrintNode(Interpreter::PrintNode* pNode)
    {
        std::string s;
        for (Interpreter::Node* pChild = pNode->GetChild(); pChild != nullptr; pChild = pChild->GetNext())
        {
            pChild->Accept(*this);
            std::optional<Value> value;
            if (m_Nodes.size() != 0)
            {
                value = GetRvalue(m_Nodes.back());
                m_Nodes.pop_back();
            }

            if (value != std::nullopt)
            {
                std::string repr = value.value().GetRepresentation();
                s += repr;
            }
        }
        Log::GetInst()->AddMessage(s);
    }

    void ExecutionNodeVisitor::VisitQuitNode(Interpreter::QuitNode* pNode)
    {
        exit(0);
    }

    void ExecutionNodeVisitor::VisitHelpNode(Interpreter::HelpNode* pNode)
    {
        Log::GetInst()->AddMessage("Interpreter HELP");
        Log::GetInst()->AddMessage("a=5+4");
        Log::GetInst()->AddMessage("a=5-4");
        Log::GetInst()->AddMessage("a=5*4");
        Log::GetInst()->AddMessage("a=4/2");
        Log::GetInst()->AddMessage("b=5");
        Log::GetInst()->AddMessage("a=5");
        Log::GetInst()->AddMessage("c=a+b");
        Log::GetInst()->AddMessage("if (a>1) {b=4 c=2} else if...else");
        Log::GetInst()->AddMessage("for(a=0, a<10, a++){ b = b-3 }");
        Log::GetInst()->AddMessage("while(a<10){ a = a-3 }");
        Log::GetInst()->AddMessage("function myfunc(a,b,c){ d = 3, e=4, ...}");
        Log::GetInst()->AddMessage("print(\"String\" a, b, c)");
        Log::GetInst()->AddMessage("load(filename.txt)");
        Log::GetInst()->AddMessage("clear()");
        Log::GetInst()->AddMessage("help--prints this message");
        Log::GetInst()->AddMessage("quit--quits");
    }

    void ExecutionNodeVisitor::VisitLoadNode(Interpreter::LoadNode* pLoadNode)
    {
        std::string filename = pLoadNode->GetChild()->GetName();

        // Check if file exists first.
        struct stat info;
        if (stat(filename.c_str(), &info) != 0)
        {
            ErrorInterface::ErrorInfo err(pLoadNode);
            char buf[512];
            sprintf_s(buf, sizeof(buf), ERROR_FILE_DOES_NOT_EXIST, filename.c_str());
            err.m_Msg = buf;
            SetErrorFlag(true);
            SetErrorInfo(err);
            return;
        }

        InterpreterDriver driver(m_pGlobalSymbolTable);
        Context* pContext = new Interpreter::Context;
        pContext->SetFile(filename);
        pContext->SetLine(1);
        pContext->SetColumn(1);
        contextStack.push_back(pContext);
        driver.ParseFromFile(filename);
        Interpreter::ErrorInterface err = driver.GetErrorInfo();
        if (err.IsErrorFlagSet())
        {
            char buf[512];
            sprintf_s(buf, sizeof(buf), "FILE:  %s, LINE:  %d, COLUMN:  %d, %s",
                err.GetErrorInfo().m_File.c_str(),
                err.GetErrorInfo().m_Line,
                err.GetErrorInfo().m_Column,
                err.GetErrorInfo().m_Msg.c_str());
            Interpreter::Log::GetInst()->AddMessage(buf);
        }

        // We just parsed a set of nodes from the file.  Insert the parsed nodes after the Load Node.
        Interpreter::Node* pNext = pLoadNode->GetNext();
        Interpreter::Node* pNode = driver.GetResult();
        pLoadNode->SetNext(pNode);

        // Go to the end of the parsed nodes and insert the pNext.
        if (pNode != nullptr)
        {
            for (; pNode->GetNext(); pNode = pNode->GetNext());
            pNode->SetNext(pNext);
        }

        delete contextStack.back();
        contextStack.pop_back();
    }

    void ExecutionNodeVisitor::VisitClearNode(Interpreter::ClearNode* pNode)
    {
        Interpreter::FunctionTable::GetInst()->Clear();
        m_pGlobalSymbolTable->Clear();
        Value v;
        bool status = m_pGlobalSymbolTable->AddSymbol("main", v);
        assert(status);
    }

    bool ExecutionNodeVisitor::ExecuteIfNode(Interpreter::IfNode* pIfNode)
    {
        Node* pExpr = pIfNode->GetExpr();
        pExpr->Accept(*this);
        if (m_Nodes.size() == 0)
        {
            SetErrorFlag(true);
            ErrorInfo err(pIfNode);
            err.m_Msg = ERROR_INVALID_EXPRESSION_IN_IF_STATEMENT;
            SetErrorInfo(err);
            return false;
        }

        ValueNode* pTop = dynamic_cast<ValueNode*>(m_Nodes.back());
        m_Nodes.pop_back();
        if (pTop == nullptr)
        {
            SetErrorFlag(true);
            ErrorInfo err(pIfNode);
            err.m_Msg = ERROR_INVALID_EXPRESSION_IN_IF_STATEMENT;
            SetErrorInfo(err);
            return false;
        }

        // If this is an array, then fail.
        if (pTop->IsArray())
        {
            SetErrorFlag(true);
            ErrorInfo err(pIfNode);
            err.m_Msg = ERROR_INVALID_EXPRESSION_IN_IF_STATEMENT;
            SetErrorInfo(err);
            return false;
        }

        if (pTop->GetValue().IfEval())
        {
            // Just execute the then statements and leave.
            for (Node* pExecute = pIfNode->GetThen(); pExecute != nullptr; pExecute = pExecute->GetNext())
            {
                pExecute->Accept(*this);
                if (IsErrorFlagSet())
                {
                    // We're done
                    return false;
                }

                // Check for return/break
                if (m_Nodes.size() != 0)
                {
                    StopNode* pNode = dynamic_cast<StopNode*>(m_Nodes.back());
                    if (pNode != nullptr)
                    {
                        // We encountered a return or break statement.  Leave on stack for
                        // function or for/while statement to process.
                        // Stop processing.
                        return true;
                    }
                }
            }

            return true;
        }

        return false;
    }

    void ExecutionNodeVisitor::VisitIfNode(Interpreter::IfNode* pIfNode)
    {
        bool status = ExecuteIfNode(pIfNode);
        if (status)
        {
            // The if statement executed.
            // We're done.
            return;
        }

        // If there was an error, leave.
        if (IsErrorFlagSet())
        {
            return;
        }

        // Iterate through the elif list.
        for (IfNode* pElif = pIfNode->GetElif(); pElif != nullptr; pElif = dynamic_cast<IfNode*>(pElif->GetNext()))
        {
            status = ExecuteIfNode(pElif);
            if (status)
            {
                // The if statement executed.
                // We're done.
                return;
            }

            // If there was an error, leave.
            if (IsErrorFlagSet())
            {
                return;
            }
        }

        // Nothing executed, execute the else statements.
        for (Node* pNode = pIfNode->GetElse(); pNode != nullptr; pNode = pNode->GetNext())
        {
            pNode->Accept(*this);

            // If there was an error, leave.
            if (IsErrorFlagSet())
            {
                return;
            }

            // Check for return/break
            if (m_Nodes.size() != 0)
            {
                StopNode* pNode = dynamic_cast<StopNode*>(m_Nodes.back());
                if (pNode != nullptr)
                {
                    // We encountered a return or break statement.  Leave on stack for
                    // function or for/while statement to process.
                    // Stop processing.
                    return;
                }
            }
        }
    }

    void ExecutionNodeVisitor::VisitWhileNode(Interpreter::WhileNode* pWhileNode)
    {
        Node* pExpr = pWhileNode->GetExpr();
        pExpr->Accept(*this);
        if (m_Nodes.size() == 0)
        {
            SetErrorFlag(true);
            ErrorInfo err(pWhileNode);
            err.m_Msg = ERROR_INVALID_EXPRESSION_IN_WHILE_STATEMENT;
            SetErrorInfo(err);
            return;
        }

        Node* pTop = m_Nodes.back();
        m_Nodes.pop_back();
        ValueNode* pExprResult = GetTopOfStackValue(pTop);
        if (pExprResult == nullptr)
        {
            return;
        }

        // If it's an entire array, error.
        if (pExprResult->IsArray())
        {
            delete pExprResult;
            SetErrorFlag(true);
            ErrorInfo err(pWhileNode);
            err.m_Msg = ERROR_ARRAY_UNEXPECTED;
            SetErrorInfo(err);
            return;
        }

        bool result = pExprResult->GetValue().IfEval();
        delete pExprResult;
        while (result)
        {
            // Just execute the then statements.
            for (Node* pExecute = pWhileNode->GetThen(); pExecute != nullptr; pExecute = pExecute->GetNext())
            {
                pExecute->Accept(*this);
                if (IsErrorFlagSet())
                {
                    return;
                }

                // Check for return statement
                if (m_Nodes.size() != 0)
                {
                    StopNode* pStopNode = dynamic_cast<StopNode*>(m_Nodes.back());
                    if (pStopNode != nullptr)
                    {
                        // If this is a return then leave it on the stack for the function call to process.
                        if (pStopNode->GetReason() == StopNode::RETURN)
                        {
                            return;
                        }

                        // If this is a break then remove it from the stack and stop executing the while.
                        if (pStopNode->GetReason() == StopNode::BREAK)
                        {
                            delete pStopNode;
                            m_Nodes.pop_back();
                            return;
                        }
                    }
                }
            }

            // Evaluate again
            pExpr->Accept(*this);
            if (IsErrorFlagSet())
            {
                return;
            }
            if (m_Nodes.size() == 0)
            {
                SetErrorFlag(true);
                ErrorInfo err(pWhileNode);
                err.m_Msg = ERROR_INVALID_EXPRESSION_IN_WHILE_STATEMENT;
                SetErrorInfo(err);
                return;
            }

            Node* pTop = m_Nodes.back();
            m_Nodes.pop_back();
            pExprResult = GetTopOfStackValue(pTop);
            if (pExprResult == nullptr)
            {
                return;
            }

            // If it's an entire array, error.
            if (pExprResult->IsArray())
            {
                delete pExprResult;
                SetErrorFlag(true);
                ErrorInfo err(pWhileNode);
                err.m_Msg = ERROR_ARRAY_UNEXPECTED;
                SetErrorInfo(err);
                return;
            }

            result = pExprResult->GetValue().IfEval();
            delete pExprResult;
        }
    };

    void ExecutionNodeVisitor::VisitForNode(Interpreter::ForNode* pForNode)
    {
        Node* pInit = pForNode->GetInit();
        pInit->Accept(*this);

        Node* pExpr = pForNode->GetExpr();
        pExpr->Accept(*this);
        if (m_Nodes.size() == 0)
        {
            SetErrorFlag(true);
            ErrorInfo err(pForNode);
            err.m_Msg = ERROR_INVALID_EXPRESSION_IN_FOR_STATEMENT;
            SetErrorInfo(err);
            return;
        }

        Node* pTop = m_Nodes.back();
        m_Nodes.pop_back();
        ValueNode* pExprResult = GetTopOfStackValue(pTop);
        if (pExprResult == nullptr)
        {
            return;
        }

        if (pExprResult->IsArray())
        {
            SetErrorFlag(true);
            ErrorInfo err(pForNode);
            err.m_Msg = ERROR_ARRAY_UNEXPECTED;
            SetErrorInfo(err);
            return;
        }

        bool result = pExprResult->GetValue().IfEval();
        delete pExprResult;

        while (result)
        {
            // Just execute the then statements.
            for (Node* pExecute = pForNode->GetThen(); pExecute != nullptr; pExecute = pExecute->GetNext())
            {
                pExecute->Accept(*this);
                if (IsErrorFlagSet())
                {
                    return;
                }

                if (m_Nodes.size() != 0)
                {
                    StopNode* pStopNode = dynamic_cast<StopNode*>(m_Nodes.back());
                    if (pStopNode != nullptr )
                    {
                        // If this is a return then leave it on the stack for the function call to process.
                        if (pStopNode->GetReason() == StopNode::RETURN)
                        {
                            return;
                        }

                        // If this is a break then remove it from the stack and stop executing the for.
                        if (pStopNode->GetReason() == StopNode::BREAK)
                        {
                            delete pStopNode;
                            m_Nodes.pop_back();
                            return;
                        }
                    }
                }
            }

            // Evaluate again
            pExpr->Accept(*this);
            if (IsErrorFlagSet())
            {
                return;
            }
            if (m_Nodes.size() == 0)
            {
                SetErrorFlag(true);
                ErrorInfo err(pForNode);
                err.m_Msg = ERROR_INVALID_EXPRESSION_IN_FOR_STATEMENT;
                SetErrorInfo(err);
                return;
            }

            pTop = m_Nodes.back();
            m_Nodes.pop_back();
            ValueNode* pExprResult = GetTopOfStackValue(pTop);
            if (pExprResult == nullptr)
            {
                return;
            }

            if (pExprResult->IsArray())
            {
                SetErrorFlag(true);
                ErrorInfo err(pForNode);
                err.m_Msg = ERROR_ARRAY_UNEXPECTED;
                SetErrorInfo(err);
                return;
            }

            result = pExprResult->GetValue().IfEval();
            delete pExprResult;
        }
    };

    void ExecutionNodeVisitor::VisitFunctionDefNode(FunctionDefNode* pNode)
    {
        // Let's create a function definition in the table.
        bool status = FunctionTable::GetInst()->Add(pNode->GetNameVar()->GetName(),
            dynamic_cast<FunctionDefNode*>(pNode->Clone()));
        if (!status)
        {
            SetErrorFlag(true);
            ErrorInfo err(pNode);
            char buf[512];
            sprintf_s(buf, sizeof(buf), ERROR_FUNCTION_CREATION, pNode->GetNameVar()->GetName().c_str());
            err.m_Msg = buf;
            SetErrorInfo(err);
            return;
        }
    };

    void ExecutionNodeVisitor::VisitFunctionCallNode(FunctionCallNode* pCallNode)
    {
        // Look up the function definition and create a symbol table for it.
        std::string functionName = pCallNode->GetNameVar()->GetName();
        FunctionDefNode* pDefNode = FunctionTable::GetInst()->Lookup(functionName);

        // Check for parameter count match
        if (pCallNode->GetInputVarCount() != pDefNode->GetInputVarCount())
        {
            SetErrorFlag(true);
            ErrorInfo err(pCallNode);
            char buf[512];
            sprintf_s(buf, sizeof(buf), ERROR_FUNCTION_ARGS, functionName, pDefNode->GetInputVarCount(), pCallNode->GetInputVarCount());
            err.m_Msg = buf;
            SetErrorInfo(err);
            return;
        }

        // Create a symbol table.
        SymbolTable* pLocalTable = new SymbolTable;
        VarNode* pDefVarNode = pDefNode->GetInputVars();
        Node* pCallExprNode = pCallNode->GetInputVars();
        while (pDefVarNode != nullptr && pCallExprNode != nullptr)
        {
            // symbol is in the function definition
            std::string symbol = pDefVarNode->GetName();

            // value is in the function call
            pCallExprNode->Accept(*this);
            if (IsErrorFlagSet())
            {
                return;
            }

            Node* pTop = m_Nodes.back();
            m_Nodes.pop_back();

            // Check if array
            if (IsArray(pTop))
            {
                std::optional<ArrayValue> v = GetRarrayValue(pTop);
                if (v == std::nullopt)
                {
                    SetErrorFlag(true);
                    ErrorInfo err(pCallExprNode);
                    char buf[512];
                    sprintf_s(buf, sizeof(buf), ERROR_FUNCTION_EXPRESSION, pDefVarNode->GetName().c_str());
                    err.m_Msg = buf;
                    SetErrorInfo(err);
                    return;
                }

                pLocalTable->AddSymbol(symbol, *v);
            }
            else
            {
                std::optional<Value> v = GetRvalue(pTop);
                if (v == std::nullopt)
                {
                    SetErrorFlag(true);
                    ErrorInfo err(pCallExprNode);
                    char buf[512];
                    sprintf_s(buf, sizeof(buf), ERROR_FUNCTION_EXPRESSION, pDefVarNode->GetName().c_str());
                    err.m_Msg = buf;
                    SetErrorInfo(err);
                    return;
                }

                pLocalTable->AddSymbol(symbol, *v);
            }

            // Advance
            pTop->Free();
            pDefVarNode = dynamic_cast<VarNode*>(pDefVarNode->GetNext());
            pCallExprNode = pCallExprNode->GetNext();
        }

        // The function will look at variables in the local table first,
        // then the global table.
        m_SymbolTableStack.push_back(pLocalTable);

        // Run the function until the end, or error, or stop because we saw a return statement.
        for (Node* pFuncNode = pDefNode->GetCode(); pFuncNode != nullptr; pFuncNode = pFuncNode->GetNext())
        {
            pFuncNode->Accept(*this);
            if (IsErrorFlagSet())
            {
                return;
            }

            if (m_Nodes.size() != 0)
            {
                StopNode* pNode = dynamic_cast<StopNode*>(m_Nodes.back());
                if (pNode != nullptr && pNode->GetReason() == StopNode::RETURN)
                {
                    delete pNode;
                    m_Nodes.pop_back();
                    break;
                }
            }
        }

        // We're done, let's remove and delete the symbol table.
        delete pLocalTable;
        m_SymbolTableStack.pop_back();
    };

    ValueNode* ExecutionNodeVisitor::GetTopOfStackValue(Node* pTop)
    {
        ValueNode* pValueNode = dynamic_cast<ValueNode*>(pTop);

        // If it's a ValueNode, we're done.
        if (pValueNode != nullptr)
        {
            return dynamic_cast<ValueNode*>(pValueNode->Clone());
        }

        // It must be a var node
        VarNode* pVarNode = dynamic_cast<VarNode*>(pTop);
        assert(pVarNode != nullptr);

        // Get the value of the VarNode.
        std::string symbol = pVarNode->GetName();
        std::vector<int> elementSpecifier = pVarNode->GetArraySpecifier();

        SymbolTable* pLocalSymbols = m_SymbolTableStack.size() != 0 ? m_SymbolTableStack.back() : nullptr;
        if (pLocalSymbols && pLocalSymbols->IsSymbolPresent(symbol))
        {
            if (elementSpecifier.size() != 0)
            {
                std::optional<Value> v = pLocalSymbols->GetSymbolValue(symbol, &elementSpecifier);
                if (v == std::nullopt)
                {
                    SetErrorFlag(true);
                    ErrorInfo err(pTop);
                    char buf[256];
                    sprintf_s(buf, sizeof(buf), ERROR_INCORRECT_ARRAY_SPECIFIER, symbol);
                    err.m_Msg = buf;
                    SetErrorInfo(err);
                    return nullptr;
                }

                pValueNode = new ValueNode;
                pValueNode->SetValue(*v);
            }
            else if (pLocalSymbols->IsSymbolArray(symbol))
            {
                std::optional<ArrayValue> v = pLocalSymbols->GetArraySymbolValue(symbol);
                assert(v != std::nullopt);
                pValueNode = new ValueNode;
                pValueNode->SetArrayValue(*v);
            }
            else
            {
                std::optional<Value> v = pLocalSymbols->GetSymbolValue(symbol);
                assert(v != std::nullopt);

                pValueNode = new ValueNode;
                pValueNode->SetValue(*v);
            }
        }
        else if (m_pGlobalSymbolTable->IsSymbolPresent(symbol))
        {
            if (elementSpecifier.size() != 0)
            {
                std::optional<Value> v = m_pGlobalSymbolTable->GetSymbolValue(symbol, &elementSpecifier);
                if (v == std::nullopt)
                {
                    SetErrorFlag(true);
                    ErrorInfo err(pTop);
                    char buf[256];
                    sprintf_s(buf, sizeof(buf), ERROR_INCORRECT_ARRAY_SPECIFIER, symbol);
                    err.m_Msg = buf;
                    SetErrorInfo(err);
                    return nullptr;
                }

                pValueNode = new ValueNode;
                pValueNode->SetValue(*v);
            }
            else if (m_pGlobalSymbolTable->IsSymbolArray(symbol))
            {
                std::optional<ArrayValue> v = m_pGlobalSymbolTable->GetArraySymbolValue(symbol);
                assert(v != std::nullopt);
                pValueNode = new ValueNode;
                pValueNode->SetArrayValue(*v);
            }

            std::optional<Value> v = m_pGlobalSymbolTable->GetSymbolValue(symbol);
            assert(v != std::nullopt);

            pValueNode = new ValueNode;
            pValueNode->SetValue(*v);
        }
        else
        {
            SetErrorFlag(true);
            ErrorInfo err(pTop);
            char buf[256];
            sprintf_s(buf, sizeof(buf), ERROR_MISSING_SYMBOL, symbol);
            err.m_Msg = buf;
            SetErrorInfo(err);
            return nullptr;
        }

        return pValueNode;
    }

    void ExecutionNodeVisitor::VisitReturnNode(Interpreter::ReturnNode* pReturnNode)
    {
        // Count the number of expressions.
        int count = 0;

        // Evaluate all expressions, convert everything to value nodes.
        ValueNode* pHead = nullptr;
        ValueNode* pCurr = nullptr;
        for (Node* pExpr = pReturnNode->GetExpr(); pExpr; pExpr = pExpr->GetNext())
        {
            // Visit the expression.
            pExpr->Accept(*this);

            // Clean and bail out if there was an error.
            if (IsErrorFlagSet())
            {
                if (pHead != nullptr)
                {
                    pHead->FreeList();
                    return;
                }
            }

            assert(m_Nodes.size() != 0);
            Node* pTop = m_Nodes.back();
            m_Nodes.pop_back();

            ValueNode* pValueNode = dynamic_cast<ValueNode*>(pTop);

            // If it's a ValueNode, we're done.
            if (pValueNode == nullptr)
            {
                // It must be a var node
                VarNode* pVarNode = dynamic_cast<VarNode*>(pTop);
                assert(pVarNode != nullptr);

                // Get the value of the VarNode.
                std::string symbol = pVarNode->GetName();
                std::vector<int> elementSpecifier = pVarNode->GetArraySpecifier();

                SymbolTable* pLocalSymbols = m_SymbolTableStack.size() != 0 ? m_SymbolTableStack.back() : nullptr;
                if (pLocalSymbols && pLocalSymbols->IsSymbolPresent(symbol))
                {
                    if (elementSpecifier.size() != 0)
                    {
                        std::optional<Value> v = pLocalSymbols->GetSymbolValue(symbol, &elementSpecifier);
                        if (v == std::nullopt)
                        {
                            SetErrorFlag(true);
                            ErrorInfo err(pTop);
                            char buf[256];
                            sprintf_s(buf, sizeof(buf), ERROR_INCORRECT_ARRAY_SPECIFIER, symbol);
                            err.m_Msg = buf;
                            SetErrorInfo(err);
                            if (pHead != nullptr)
                            {
                                pHead->FreeList();
                            }
                            return;
                        }

                        pValueNode = new ValueNode;
                        pValueNode->SetValue(*v);
                    }
                    else if (pLocalSymbols->IsSymbolArray(symbol))
                    {
                        std::optional<ArrayValue> v = pLocalSymbols->GetArraySymbolValue(symbol);
                        assert(v != std::nullopt);
                        pValueNode = new ValueNode;
                        pValueNode->SetArrayValue(*v);
                    }
                    else
                    {
                        std::optional<Value> v = pLocalSymbols->GetSymbolValue(symbol);
                        assert(v != std::nullopt);

                        pValueNode = new ValueNode;
                        pValueNode->SetValue(*v);
                    }
                }
                else if (m_pGlobalSymbolTable->IsSymbolPresent(symbol))
                {
                    if (elementSpecifier.size() != 0)
                    {
                        std::optional<Value> v = m_pGlobalSymbolTable->GetSymbolValue(symbol, &elementSpecifier);
                        if (v == std::nullopt)
                        {
                            SetErrorFlag(true);
                            ErrorInfo err(pTop);
                            char buf[256];
                            sprintf_s(buf, sizeof(buf), ERROR_INCORRECT_ARRAY_SPECIFIER, symbol);
                            err.m_Msg = buf;
                            SetErrorInfo(err);
                            if (pHead != nullptr)
                            {
                                pHead->FreeList();
                            }
                            return;
                        }

                        pValueNode = new ValueNode;
                        pValueNode->SetValue(*v);
                    }
                    else if (m_pGlobalSymbolTable->IsSymbolArray(symbol))
                    {
                        std::optional<ArrayValue> v = m_pGlobalSymbolTable->GetArraySymbolValue(symbol);
                        assert(v != std::nullopt);
                        pValueNode = new ValueNode;
                        pValueNode->SetArrayValue(*v);
                    }

                    std::optional<Value> v = m_pGlobalSymbolTable->GetSymbolValue(symbol);
                    assert(v != std::nullopt);

                    pValueNode = new ValueNode;
                    pValueNode->SetValue(*v);
                }
                else
                {
                    SetErrorFlag(true);
                    ErrorInfo err(pTop);
                    char buf[256];
                    sprintf_s(buf, sizeof(buf), ERROR_MISSING_SYMBOL, symbol);
                    err.m_Msg = buf;
                    SetErrorInfo(err);
                    if (pHead != nullptr)
                    {
                        pHead->FreeList();
                    }
                    return;
                }
            }

            assert(pValueNode != nullptr);
            if (pHead == nullptr)
            {
                pHead = pValueNode;
                pCurr = pHead;
            }
            else
            {
                pCurr->SetNext(pValueNode);
                pCurr = pValueNode;
            }

            count++;
        }

        // If we have more than one result, house it all it a VarListNode.
        if (count > 1)
        {
            VarListNode* pNode = new VarListNode;
            pNode->SetList(pHead);
            m_Nodes.push_back(pNode);
        }
        else if (count == 1)
        {
            m_Nodes.push_back(pHead);
        }

        // Let's place a quit node to alert the function to stop executing
        StopNode* pStopNode = new StopNode(StopNode::RETURN);
        m_Nodes.push_back(pStopNode);
    };

    void ExecutionNodeVisitor::VisitDimNode(Interpreter::DimNode* pDimNode)
    {
        // Evaluate all the dimensions.
        Node* pHead = nullptr;
        Node* pCurr = nullptr;
        for (Node* pNode = pDimNode->GetDim(); pNode != nullptr; pNode = pNode->GetNext())
        {
            pNode->Accept(*this);
            if (IsErrorFlagSet())
            {
                return;
            }
            Node* pResult = m_Nodes.back();
            m_Nodes.pop_back();
            if (pHead == nullptr)
            {
                pHead = pResult;
                pCurr = pResult;
            }
            else
            {
                pCurr->SetNext(pResult);
                pCurr = pResult;
            }
        }

        // Assign new nodes for the dimensions.
        pDimNode->ClearDim();
        pDimNode->AddDim(pHead);
        m_Nodes.push_back(pDimNode->Clone());
    };

    void ExecutionNodeVisitor::VisitPwdNode(PwdNode* pNode)
    {
        // Just print out the current working directory.
        std::string path(std::filesystem::current_path().u8string());
        Log::GetInst()->AddMessage(path);
    }

    void ExecutionNodeVisitor::VisitFunctionsNode(FunctionsNode* pNode)
    {
        FunctionTable::GetInst()->Dump();
    }

    void ExecutionNodeVisitor::VisitVarsCmdNode(VarsCmdNode* pNode)
    {
        m_pGlobalSymbolTable->Dump();
    }

    void ExecutionNodeVisitor::VisitVarListNode(VarListNode* pNode)
    {
        m_Nodes.push_back(pNode->Clone());
    }

    void ExecutionNodeVisitor::VisitSrandNode(SrandNode* pNode)
    {
        // Evaulate the expression
        Node* pExpr = pNode->GetExpr();
        pExpr->Accept(*this);
        if (IsErrorFlagSet())
        {
            return;
        }

        assert(m_Nodes.size() != 0);
        Node* pTop = m_Nodes.back();
        m_Nodes.pop_back();

        ValueNode* pValueNode = GetTopOfStackValue(pTop);
        delete pTop;
        if (IsErrorFlagSet())
        {
            return;
        }

        if (pValueNode->IsArray())
        {
            SetErrorFlag(true);
            ErrorInterface::ErrorInfo err(pValueNode);
            err.m_Msg = ErrorInterface::ERROR_ARRAY_UNEXPECTED;
            SetErrorInfo(err);
            return;
        }

        // Only ints allowed
        Value v = pValueNode->GetValue();
        if (v.GetType() != typeid(int))
        {
            SetErrorFlag(true);
            ErrorInterface::ErrorInfo err(pValueNode);
            err.m_Msg = ErrorInterface::ERROR_INCORRECT_TYPE;
            SetErrorInfo(err);
            return;
        }

        // Invoke srand()
        srand(v.GetIntValue());
    }

    void ExecutionNodeVisitor::VisitRandNode(RandNode* pNode)
    {
        // Just evaluate rand() and place on stack.
        int result = rand();
        ValueNode* pValueNode = new ValueNode;
        Value v;
        v.SetIntValue(result);
        pValueNode->SetValue(v);
        m_Nodes.push_back(pValueNode);
    }

    void ExecutionNodeVisitor::VisitLenNode(LenNode* pNode)
    {
        // Make sure the symbol represents an array.
        SymbolTable* pSymbolTable = m_SymbolTableStack.size() != 0 ? m_SymbolTableStack.back() : nullptr;
        if (pSymbolTable)
        {
            if (!pSymbolTable->IsSymbolPresent(pNode->GetName()))
            {
                pSymbolTable = m_pGlobalSymbolTable;
            }
        }
        else
        {
            pSymbolTable = m_pGlobalSymbolTable;
        }

        if (!pSymbolTable->IsSymbolPresent(pNode->GetName()))
        {
            ErrorInterface::ErrorInfo err(pNode);
            char buf[512];
            sprintf_s(buf, sizeof(buf), ERROR_MISSING_SYMBOL, pNode->GetName().c_str());
            err.m_Msg = buf;
            SetErrorFlag(true);
            SetErrorInfo(err);
            return;
        }

        if (!pSymbolTable->IsSymbolArray(pNode->GetName()))
        {
            ErrorInterface::ErrorInfo err(pNode);
            char buf[512];
            sprintf_s(buf, sizeof(buf), ERROR_ENTIRE_ARRAY_EXPECTED, pNode->GetName().c_str());
            err.m_Msg = buf;
            SetErrorFlag(true);
            SetErrorInfo(err);
            return;
        }

        std::optional<std::vector<int> > symbolDims = pSymbolTable->GetSymbolDims(pNode->GetName());
        assert(symbolDims != std::nullopt);

        if (pNode->GetDim() >= symbolDims->size())
        {
            ErrorInterface::ErrorInfo err(pNode);
            char buf[512];
            sprintf_s(buf, sizeof(buf), ERROR_INCORRECT_LEN_DIMS, pNode->GetName().c_str(), symbolDims->size()-1);
            err.m_Msg = buf;
            SetErrorFlag(true);
            SetErrorInfo(err);
            return;
        }

        ValueNode* pValueNode = new ValueNode;
        Value v;
        v.SetIntValue(symbolDims->at(pNode->GetDim()));
        pValueNode->SetValue(v);
        m_Nodes.push_back(pValueNode);
    }

    // Encountered a break statement.  Let's place a stop node on the stack.
    void ExecutionNodeVisitor::VisitBreakNode(BreakNode* pNode)
    {
        StopNode* pStopNode = new StopNode(StopNode::BREAK);
        m_Nodes.push_back(pStopNode);
    }
};

