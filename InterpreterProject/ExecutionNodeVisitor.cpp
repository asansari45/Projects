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
        Interpreter::Node* pNode = driver.GetResult();
        pLoadNode->SetNext(pNode);

        delete contextStack.back();
        contextStack.pop_back();
    }

    void ExecutionNodeVisitor::VisitClearNode(Interpreter::ClearNode* pNode)
    {
        Interpreter::FunctionTable::GetInst()->Clear();
        m_pGlobalSymbolTable->Clear();
    }

    bool ExecutionNodeVisitor::ExecuteIfNode(Interpreter::IfNode* pIfNode)
    {
        Node* pExpr = pIfNode->GetExpr();
        pExpr->Accept(*this);
        std::optional<Value> expr = GetResult();
        if (expr == std::nullopt)
        {
            SetErrorFlag(true);
            ErrorInfo err(pIfNode);
            err.m_Msg = ERROR_INVALID_EXPRESSION_IN_IF_STATEMENT;
            SetErrorInfo(err);
            return false;
        }

        if (expr.value().IfEval())
        {
            // Just execute the then statements and leave.
            for (Node* pExecute = pIfNode->GetThen(); pExecute != nullptr; pExecute = pExecute->GetNext())
            {
                pExecute->Accept(*this);
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
        }
    }

    void ExecutionNodeVisitor::VisitWhileNode(Interpreter::WhileNode* pWhileNode)
    {
        Node* pExpr = pWhileNode->GetExpr();
        pExpr->Accept(*this);
        std::optional<Value> expr = GetResult();
        if (expr == std::nullopt)
        {
            SetErrorFlag(true);
            ErrorInfo err(pWhileNode);
            err.m_Msg = ERROR_INVALID_EXPRESSION_IN_WHILE_STATEMENT;
            SetErrorInfo(err);
            return;
        }

        while (expr.value().IfEval())
        {
            // Just execute the then statements.
            for (Node* pExecute = pWhileNode->GetThen(); pExecute != nullptr; pExecute = pExecute->GetNext())
            {
                pExecute->Accept(*this);
                if (IsErrorFlagSet())
                {
                    return;
                }
            }

            // Evaluate again
            pExpr->Accept(*this);
            if (IsErrorFlagSet())
            {
                return;
            }
            expr = GetResult();
        }
    };

    void ExecutionNodeVisitor::VisitForNode(Interpreter::ForNode* pForNode)
    {
        Node* pInit = pForNode->GetInit();
        pInit->Accept(*this);

        Node* pExpr = pForNode->GetExpr();
        pExpr->Accept(*this);
        std::optional<Value> result = GetResult();
        if (result == std::nullopt)
        {
            SetErrorFlag(true);
            ErrorInfo err(pForNode);
            err.m_Msg = ERROR_INVALID_EXPRESSION_IN_FOR_STATEMENT;
            SetErrorInfo(err);
            return;
        }

        while (result.value().IfEval())
        {
            // Just execute the then statements.
            for (Node* pExecute = pForNode->GetThen(); pExecute != nullptr; pExecute = pExecute->GetNext())
            {
                pExecute->Accept(*this);
                if (IsErrorFlagSet())
                {
                    return;
                }
            }

            // Evaluate again
            pExpr->Accept(*this);
            if (IsErrorFlagSet())
            {
                return;
            }
            result = GetResult();
        }
    };

    void ExecutionNodeVisitor::VisitFunctionDefNode(FunctionDefNode* pNode)
    {
        // Let's create a function definition in the table.
        bool status = FunctionTable::GetInst()->AttachDefinition(pNode->GetNameVar()->GetName(), 
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
            sprintf_s(buf, sizeof(buf), ERROR_FUNCTION_ARGS, pDefNode->GetInputVarCount(), pCallNode->GetInputVarCount());
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
            std::optional<Value> value = GetResult();

            // Done with the stack result
            m_Nodes.pop_back();
            if (value == std::nullopt)
            {
                SetErrorFlag(true);
                ErrorInfo err(pCallExprNode);
                char buf[512];
                sprintf_s(buf, sizeof(buf), ERROR_FUNCTION_EXPRESSION, pDefVarNode->GetName().c_str());
                err.m_Msg = buf;
                SetErrorInfo(err);
                return;
            }

            pLocalTable->AddSymbol(symbol, value.value());

            // Advance
            pDefVarNode = dynamic_cast<VarNode*>(pDefVarNode->GetNext());
            pCallExprNode = pCallExprNode->GetNext();
        }

        // The function will look at variables in the local table first,
        // then the global table.
        m_SymbolTableStack.push_back(pLocalTable);

        // Run the function 
        for (Node* pFuncNode = pDefNode->GetCode(); pFuncNode != nullptr; pFuncNode = pFuncNode->GetNext())
        {
            pFuncNode->Accept(*this);
            if (IsErrorFlagSet())
            {
                return;
            }
        }

        // We're done, let's remove and delete the symbol table.
        delete pLocalTable;
        m_SymbolTableStack.pop_back();
    };

    void ExecutionNodeVisitor::VisitReturnNode(Interpreter::ReturnNode* pReturnNode)
    {
        // Evaluate the expression, it's result will just be left on the stack.
        Node* pExpr = pReturnNode->GetExpr();
        if (pExpr == nullptr)
        {
            // Nothing to do.
            return;
        }

        pExpr->Accept(*this);
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
};

