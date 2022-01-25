#include <assert.h>
#include <sstream>
#include <filesystem>
#include "ExecutionNodeVisitor.hpp"
#include "InterpreterFunctionDefNode.h"
#include "InterpreterSymbolTable.h"
#include "InterpreterAlgorithmRepository.h"
#include "InterpreterDriver.hpp"
#include "InterpreterFunctionTable.h"
#include "InterpreterLog.h"
#include "InterpreterDimNode.h"
#include "InterpreterForNode.h"
#include "InterpreterFunctionCallNode.h"
#include "InterpreterHelpNode.h"
#include "InterpreterIfNode.h"
#include "InterpreterLenNode.h"
#include "InterpreterLoadNode.h"
#include "InterpreterPrintNode.h"
#include "InterpreterStopNode.h"
#include "InterpreterWhileNode.h"
#include "InterpreterRandNode.h"
#include "InterpreterRefNode.h"
#include "InterpreterReturnNode.h"
#include "InterpreterSrandNode.h"
#include "InterpreterVarListNode.h"


namespace Interpreter
{

    ExecutionNodeVisitor::ExecutionNodeVisitor(SymbolTable* pGlobalSymbols) :
        ErrorInterface(),
        m_Nodes(),
        m_FunctionCallStack(),
        m_pGlobalSymbolTable(pGlobalSymbols)
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
        // Make sure the state of this var node is preserved.
        // Make a clone and modify that state.
        VarNode* pClone = dynamic_cast<VarNode*>(pNode->Clone());
        assert(pClone != nullptr);

        // Calculate the VarNode's symbol table.
        std::optional<SymbolTable::SymbolInfo> symbolInfo;
        if (m_FunctionCallStack.size() != 0)
        {
            SymbolTable* pSymbolTable = m_FunctionCallStack.back()->GetSymbolTable();
            symbolInfo = pSymbolTable->ReadSymbol(pClone->GetName(), // Name
                                                  true);            // Get the root symbol
            if (symbolInfo != std::nullopt)
            {
                pClone->SetSymbolPresent(true);
                pClone->SetSymbolInfo(*symbolInfo);
            }
        }

        if (!pClone->IsSymbolPresent())
        {
            symbolInfo = m_pGlobalSymbolTable->ReadSymbol(pClone->GetName(), // Name
                                                          true);             // Get the root symbol
            if (symbolInfo != std::nullopt)
            {
                pClone->SetSymbolPresent(true);
                pClone->SetSymbolInfo(*symbolInfo);
            }
        }

        if (!pClone->IsSymbolPresent())
        {
            if (m_FunctionCallStack.size() != 0)
            {
                pClone->SetNoSymbol(m_FunctionCallStack.back()->GetSymbolTable());
            }
            else
            {
                pClone->SetNoSymbol(m_pGlobalSymbolTable);
            }
        }

        m_Nodes.push_back(pClone);
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
        Node* pResult = pFunc->Perform(pLeft, 
                                       pRight, 
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
        VarNode* pVarNode = pNode->GetVar();
        if (pVarNode)
        {
            std::string subTopic = pVarNode->GetName();
            if (subTopic == "expr" || subTopic == "expression")
            {
                Log::GetInst()->AddMessage("a=b+c -- addition");
                Log::GetInst()->AddMessage("a=b-c -- subtraction");
                Log::GetInst()->AddMessage("a=b*c -- multiplication");
                Log::GetInst()->AddMessage("a=b/c -- division");
                Log::GetInst()->AddMessage("a=-b  -- negation");
                Log::GetInst()->AddMessage("result=b<c -- less than");
                Log::GetInst()->AddMessage("result=b>c -- greater than");
                Log::GetInst()->AddMessage("result=b<=c -- less than or equal to");
                Log::GetInst()->AddMessage("result=b>=c -- greater than or equal to");
                Log::GetInst()->AddMessage("result=b==c -- is equal to");
                Log::GetInst()->AddMessage("result=b!=c -- is not equal to");
            }
            else
            {
                ErrorInfo err(pNode);
                char buf[256];
                sprintf_s(buf, sizeof(buf), ERROR_INVALID_HELP_PARAMETER, subTopic.c_str());
                err.m_Msg = buf;
                SetErrorInfo(err);
            }
            return;
        }

        Log::GetInst()->AddMessage("Interpreter HELP");
        Log::GetInst()->AddMessage("help()            -- prints this message");
        Log::GetInst()->AddMessage("help(expr)        -- prints help on expressions");
        Log::GetInst()->AddMessage("");
        Log::GetInst()->AddMessage("quit            -- quits");
        Log::GetInst()->AddMessage(".vars           -- displays all global variables");
        Log::GetInst()->AddMessage(".functions      -- display all functions with parameters");
        Log::GetInst()->AddMessage(".pwd            -- prints the current working directory");
        Log::GetInst()->AddMessage("");
        Log::GetInst()->AddMessage("b=5             -- assignment");
        Log::GetInst()->AddMessage("a=5             -- assignment");
        Log::GetInst()->AddMessage("a=5+4           -- assignment with expression");
        Log::GetInst()->AddMessage("a=5-4           -- assignment with expression");
        Log::GetInst()->AddMessage("a=5*4           -- assignment with expression");
        Log::GetInst()->AddMessage("a=4/2           -- assignment with expression");
        Log::GetInst()->AddMessage("c=a+b           -- assignment with expression");
        Log::GetInst()->AddMessage("");
        Log::GetInst()->AddMessage("a=dim[10]       -- one-dimensional array");
        Log::GetInst()->AddMessage("a=dim[10,10]    -- two-dimensional array");
        Log::GetInst()->AddMessage("a=dim[10,10,10] -- three-dimensional array");
        Log::GetInst()->AddMessage("b=c[0]+d[9,3]+e[0,8,3] -- array specifiers");
        Log::GetInst()->AddMessage("");
        Log::GetInst()->AddMessage("{a,b}={0,1}     -- variable lists");
        Log::GetInst()->AddMessage("");
        Log::GetInst()->AddMessage("if (a>1)        -- if statement");
        Log::GetInst()->AddMessage("{");
        Log::GetInst()->AddMessage("\tb=4");
        Log::GetInst()->AddMessage("\tc=2");
        Log::GetInst()->AddMessage("}");
        Log::GetInst()->AddMessage("else");
        Log::GetInst()->AddMessage("{");
        Log::GetInst()->AddMessage("\tq=5");
        Log::GetInst()->AddMessage("\ts=6");
        Log::GetInst()->AddMessage("}");
        Log::GetInst()->AddMessage("");
        Log::GetInst()->AddMessage("for(a=0, a<10, a++) -- for statement");
        Log::GetInst()->AddMessage("{");
        Log::GetInst()->AddMessage("\tb = b - 3");
        Log::GetInst()->AddMessage("}");
        Log::GetInst()->AddMessage("");
        Log::GetInst()->AddMessage("while(a<10) -- while statement");
        Log::GetInst()->AddMessage("{");
        Log::GetInst()->AddMessage("\tb = b - 3");
        Log::GetInst()->AddMessage("}");
        Log::GetInst()->AddMessage("");
        Log::GetInst()->AddMessage("function myfunc(a,b,c) -- function");
        Log::GetInst()->AddMessage("{");
        Log::GetInst()->AddMessage("\td = 3");
        Log::GetInst()->AddMessage("\te = 4");
        Log::GetInst()->AddMessage("\treturn(a, b, c)");
        Log::GetInst()->AddMessage("}");
        Log::GetInst()->AddMessage("{x,y,z}=myfunc(0,1,2) -- function with variable lists");
        Log::GetInst()->AddMessage("");
        Log::GetInst()->AddMessage("function myfunc(&s)   -- function with reference parameters");
        Log::GetInst()->AddMessage("{");
        Log::GetInst()->AddMessage("\ts=s+1");
        Log::GetInst()->AddMessage("}");
        Log::GetInst()->AddMessage("a=3");
        Log::GetInst()->AddMessage("myfunc(a)");
        Log::GetInst()->AddMessage("");
        Log::GetInst()->AddMessage("print(\"String\" a, b, c) -- print statement");
        Log::GetInst()->AddMessage("// This is a one-line comment -- comments");
        Log::GetInst()->AddMessage("load(filename.txt) -- load and execute from a file");
        Log::GetInst()->AddMessage("clear()            -- clear all variables and functions");
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

        SymbolTable::SymbolInfo s;
        s.m_Name = "main";
        bool status = m_pGlobalSymbolTable->CreateSymbol("main", s);
        assert(status);
    }

    bool ExecutionNodeVisitor::ExecuteIfNode(Interpreter::IfNode* pIfNode)
    {
        Node* pExpr = pIfNode->GetExpr();
        pExpr->Accept(*this);
        if (m_Nodes.size() == 0)
        {
            
            ErrorInfo err(pIfNode);
            err.m_Msg = ERROR_INVALID_EXPRESSION_IN_IF_STATEMENT;
            SetErrorInfo(err);
            return false;
        }

        ValueNode* pTop = dynamic_cast<ValueNode*>(m_Nodes.back());
        m_Nodes.pop_back();
        if (pTop == nullptr)
        {
            
            ErrorInfo err(pIfNode);
            err.m_Msg = ERROR_INVALID_EXPRESSION_IN_IF_STATEMENT;
            SetErrorInfo(err);
            return false;
        }

        // If this is an array, then fail.
        if (pTop->IsArray())
        {
            
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
        bool status = FunctionTable::GetInst()->CreateFunction(pNode->GetName(),
            dynamic_cast<FunctionDefNode*>(pNode->Clone()));
        if (!status)
        {
            
            ErrorInfo err(pNode);
            char buf[512];
            sprintf_s(buf, sizeof(buf), ERROR_FUNCTION_CREATION, pNode->GetName());
            err.m_Msg = buf;
            SetErrorInfo(err);
            return;
        }
    };

    void ExecutionNodeVisitor::VisitFunctionCallNode(FunctionCallNode* pCallNode)
    {
        // Look up the function definition and create a symbol table for it.
        std::string functionName = pCallNode->GetNameVar()->GetName();
        FunctionDefNode* pDefNode = FunctionTable::GetInst()->ReadFunction(functionName);

        char buf[512];
        sprintf_s(buf, sizeof(buf), "Executing function %s", functionName.c_str());
        Log::GetInst()->AddMessage(Log::DEBUG, buf);

        // Check for parameter count match
        if (pCallNode->GetInputVarCount() != pDefNode->GetInputVarCount())
        {
            
            ErrorInfo err(pCallNode);
            char buf[512];
            sprintf_s(buf, sizeof(buf), ERROR_FUNCTION_ARGS, functionName.c_str(), pDefNode->GetInputVarCount(), pCallNode->GetInputVarCount());
            err.m_Msg = buf;
            SetErrorInfo(err);
            return;
        }
        
        // Fill in the functions symbol table.
        SymbolTable* pLocalTable = pDefNode->GetSymbolTable();
        Node* pDefVarNode = pDefNode->GetInputVars();
        Node* pCallExprNode = pCallNode->GetInputVars();
        while (pDefVarNode != nullptr && pCallExprNode != nullptr)
        {
            RefNode* pRefNode = dynamic_cast<RefNode*>(pDefVarNode);
            if (pRefNode)
            {
                pCallExprNode->Accept(*this);
                if (IsErrorFlagSet())
                {
                    pLocalTable->Clear();
                    return;
                }

                assert(m_Nodes.size() != 0);
                Node* pTop = m_Nodes.back();
                m_Nodes.pop_back();

                // We have a reference here.  The call expression is only allowed to be
                // a variable with no array specifier.  A reference is simply a rename
                // for a passed in parameter.
                VarNode* pVarNode = dynamic_cast<VarNode*>(pTop);
                if (pVarNode == nullptr)
                {
                    ErrorInterface::ErrorInfo err(pCallExprNode);
                    char buf[512];
                    sprintf_s(buf, sizeof(buf), ERROR_INVALID_REFERENCE_PARAMETER, pRefNode->GetName().c_str());
                    err.m_Msg = buf;
                    SetErrorInfo(err);
                    
                    delete pTop;
                    pLocalTable->Clear();
                    return;
                }

                // Find the variable first in the previous table on the stack.
                if (!pVarNode->IsSymbolPresent())
                {
                    ErrorInterface::ErrorInfo err(pCallExprNode);
                    char buf[512];
                    sprintf_s(buf, sizeof(buf), ERROR_INVALID_REFERENCE_PARAMETER, pRefNode->GetName().c_str());
                    err.m_Msg = buf;
                    SetErrorInfo(err);
                    
                    delete pTop;
                    pLocalTable->Clear();
                    return;
                }

                std::optional<SymbolTable::SymbolInfo> referencee = pVarNode->GetSymbolInfo();
                assert(referencee->m_IsRef == false);

                SymbolTable::SymbolInfo referencer;
                referencer.m_IsRef = true;
                referencer.m_Name = pRefNode->GetName();
                referencer.m_RefName = referencee->m_Name;
                referencer.m_pRefTable = referencee->m_pTable;

                bool status = pLocalTable->CreateSymbol(pRefNode->GetName(), referencer);
                if (!status)
                {
                    ErrorInterface::ErrorInfo err(pCallExprNode);
                    char buf[512];
                    sprintf_s(buf, sizeof(buf), ERROR_INVALID_REFERENCE_PARAMETER, pRefNode->GetName().c_str());
                    err.m_Msg = buf;
                    SetErrorInfo(err);
                    
                    delete pTop;
                    pLocalTable->Clear();
                    return;
                }
            }
            else
            {
                // value is in the function call
                VarNode* pVarNode = dynamic_cast<VarNode*>(pDefVarNode);
                assert(pVarNode != nullptr);
                std::string symbol = pVarNode->GetName();

                pCallExprNode->Accept(*this);
                if (IsErrorFlagSet())
                {
                    pLocalTable->Clear();
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
                        
                        ErrorInfo err(pCallExprNode);
                        char buf[512];
                        sprintf_s(buf, sizeof(buf), ERROR_FUNCTION_EXPRESSION, functionName.c_str());
                        err.m_Msg = buf;
                        SetErrorInfo(err);
                        pLocalTable->Clear();
                        return;
                    }

                    SymbolTable::SymbolInfo info;
                    info.m_Name = symbol;
                    info.m_ArrayValue = *v;
                    pLocalTable->CreateSymbol(symbol, info);
                }
                else
                {
                    std::optional<Value> v = GetRvalue(pTop);
                    if (v == std::nullopt)
                    {
                        
                        ErrorInfo err(pCallExprNode);
                        char buf[512];
                        sprintf_s(buf, sizeof(buf), ERROR_FUNCTION_EXPRESSION, functionName.c_str());
                        err.m_Msg = buf;
                        SetErrorInfo(err);
                        pLocalTable->Clear();
                        return;
                    }

                    SymbolTable::SymbolInfo info;
                    info.m_Name = symbol;
                    info.m_Value = *v;
                    pLocalTable->CreateSymbol(symbol, info);
                }

                pTop->Free();
            }

            // Advance
            pDefVarNode = pDefVarNode->GetNext();
            pCallExprNode = pCallExprNode->GetNext();
        }

        // The function will look at variables in the local table first,
        // then the global table.
        m_FunctionCallStack.push_back(pDefNode);

        // Run the function until the end, or error, or stop because we saw a return statement.
        for (Node* pFuncNode = pDefNode->GetCode(); pFuncNode != nullptr; pFuncNode = pFuncNode->GetNext())
        {
            pFuncNode->Accept(*this);
            if (IsErrorFlagSet())
            {
                break;
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

        // We're done, let's remove and clear the symbol table.
        pLocalTable->Clear();
        m_FunctionCallStack.pop_back();
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
        if (!pVarNode->IsSymbolPresent())
        {
            
            ErrorInfo err(pTop);
            char buf[256];
            sprintf_s(buf, sizeof(buf), ERROR_MISSING_SYMBOL, symbol);
            err.m_Msg = buf;
            SetErrorInfo(err);
            return nullptr;
        }

        SymbolTable::SymbolInfo info = pVarNode->GetSymbolInfo();
        if (elementSpecifier.size() != 0)
        {
            if (info.m_IsArray == false)
            {
                ErrorInfo err(pTop);
                
                char buf[256];
                sprintf_s(buf, sizeof(buf), ERROR_UNEXPECTED_ARRAY_SPECIFIER, symbol);
                err.m_Msg = buf;
                SetErrorInfo(err);
                return nullptr;
            }
        
            std::optional<Value> v = info.m_ArrayValue.GetValue(elementSpecifier);
            if (v == std::nullopt)
            {
                
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
        // No element specifier
        else if (info.m_IsArray)
        {
            pValueNode = new ValueNode;
            pValueNode->SetArrayValue(info.m_ArrayValue);
        }
        else
        {
            pValueNode = new ValueNode;
            pValueNode->SetValue(info.m_Value);
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

            ValueNode* pValueNode = GetTopOfStackValue(pTop);
            if (pValueNode == nullptr)
            {
                return;
            }

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
        // Leave the original DimNode intact.
        DimNode* pClone = dynamic_cast<DimNode*>(pDimNode->Clone());
        assert(pClone != nullptr);

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
        pClone->ClearDim();
        pClone->AddDim(pHead);
        m_Nodes.push_back(pClone);
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

    void ExecutionNodeVisitor::VisitVarListNode(VarListNode* pVarList)
    {
        // Do not modify the original list.
        VarListNode* pClone = dynamic_cast<VarListNode*>(pVarList->Clone());
        assert(pClone != nullptr);

        // Simplify the each of expressions in the VarList.
        Node* pHead = nullptr;
        Node* pCurr = nullptr;
        for (Node* pNode = pClone->GetList(); pNode != nullptr; pNode = pNode->GetNext())
        {
            pNode->Accept(*this);
            if (IsErrorFlagSet())
            {
                return;
            }

            assert(m_Nodes.size() != 0);
            Node* pNewNode = m_Nodes.back();
            m_Nodes.pop_back();

            if (pHead == nullptr)
            {
                pHead = pNewNode;
                pCurr = pNewNode;
            }
            else
            {
                pCurr->SetNext(pNewNode);
                pCurr = pNewNode;
            }
        }

        pClone->GetList()->FreeList();
        pClone->SetList(pHead);
        m_Nodes.push_back(pClone);
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
            
            ErrorInterface::ErrorInfo err(pValueNode);
            err.m_Msg = ErrorInterface::ERROR_ARRAY_UNEXPECTED;
            SetErrorInfo(err);
            return;
        }

        // Only ints allowed
        Value v = pValueNode->GetValue();
        if (v.GetType() != typeid(int))
        {
            
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
        SymbolTable* pSymbolTable = m_FunctionCallStack.size() != 0 ? m_FunctionCallStack.back()->GetSymbolTable() : nullptr;
        std::optional<SymbolTable::SymbolInfo> info;
        if (pSymbolTable)
        {
            info = pSymbolTable->ReadSymbol(pNode->GetName(), true);
            if (info == std::nullopt)
            {
                info = m_pGlobalSymbolTable->ReadSymbol(pNode->GetName());
            }
        }
        else
        {
            info = m_pGlobalSymbolTable->ReadSymbol(pNode->GetName());
        }

        if (info == std::nullopt)
        {
            ErrorInterface::ErrorInfo err(pNode);
            char buf[512];
            sprintf_s(buf, sizeof(buf), ERROR_MISSING_SYMBOL, pNode->GetName().c_str());
            err.m_Msg = buf;
            
            SetErrorInfo(err);
            return;
        }

        if (!info->m_IsArray)
        {
            ErrorInterface::ErrorInfo err(pNode);
            char buf[512];
            sprintf_s(buf, sizeof(buf), ERROR_ENTIRE_ARRAY_EXPECTED, pNode->GetName().c_str());
            err.m_Msg = buf;
            
            SetErrorInfo(err);
            return;
        }

        std::vector<int> symbolDims = info->m_ArrayValue.GetDims();
        assert(symbolDims.size() != 0);

        if (pNode->GetDim() >= symbolDims.size())
        {
            ErrorInterface::ErrorInfo err(pNode);
            char buf[512];
            sprintf_s(buf, sizeof(buf), ERROR_INCORRECT_LEN_DIMS, pNode->GetName().c_str(), symbolDims.size()-1);
            err.m_Msg = buf;
            
            SetErrorInfo(err);
            return;
        }

        ValueNode* pValueNode = new ValueNode;
        Value v;
        v.SetIntValue(symbolDims[pNode->GetDim()]);
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

