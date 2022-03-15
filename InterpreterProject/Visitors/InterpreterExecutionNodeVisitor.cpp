#include <assert.h>
#include <sstream>
#include <filesystem>
#include <memory>
#include "InterpreterExecutionNodeVisitor.h"
#include "Nodes/InterpreterFunctionDefNode.h"
#include "Tables/InterpreterSymbolTable.h"
#include "Algorithm/InterpreterAlgorithmRepository.h"
#include "Driver/InterpreterDriver.hpp"
#include "Tables/InterpreterFunctionTable.h"
#include "Log/InterpreterLog.h"
#include "Nodes/InterpreterDimNode.h"
#include "Nodes/InterpreterForNode.h"
#include "Nodes/InterpreterFunctionCallNode.h"
#include "Nodes/InterpreterHelpNode.h"
#include "Nodes/InterpreterIfNode.h"
#include "Nodes/InterpreterLenNode.h"
#include "Nodes/InterpreterLoadNode.h"
#include "Nodes/InterpreterPrintNode.h"
#include "Nodes/InterpreterStopNode.h"
#include "Nodes/InterpreterWhileNode.h"
#include "Nodes/InterpreterRandNode.h"
#include "Nodes/InterpreterRefNode.h"
#include "Nodes/InterpreterReturnNode.h"
#include "Nodes/InterpreterSrandNode.h"
#include "Nodes/InterpreterVarListNode.h"
#include "Nodes/InterpreterFileOpenNode.h"
#include "Nodes/InterpreterFileReadNode.h"
#include "Nodes/InterpreterFileWriteNode.h"
#include "Nodes/InterpreterFileCloseNode.h"
#include "Nodes/InterpreterFileEofNode.h"
#include "File/InterpreterFile.h"

#include "DebugMemory/DebugMemory.h"

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
        Node* pResult = pFunc->Perform(pLeft, 
                                       pRight, 
                                       this,
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
        std::stringstream s;
        for (Interpreter::Node* pChild = pNode->GetChild(); pChild != nullptr; pChild = pChild->GetNext())
        {
            // Deal with formatting nodes first.
            PrintNode* pFormatNode = dynamic_cast<PrintNode*>(pChild);
            if (pFormatNode != nullptr)
            {
                pFormatNode->ModifyStream(s);
            }
            else
            {
                pChild->Accept(*this);
                if (m_Nodes.size() != 0)
                {
                    std::unique_ptr<Node> pTop(m_Nodes.back());
                    m_Nodes.pop_back();
                    std::unique_ptr<ValueNode> pValueNode(GetTopOfStackValue(pTop.get()));
                    if (pValueNode->IsArray() == false)
                    {
                        pValueNode->GetValue().FillStream(s);
                    }
                    else
                    {
                        s << pValueNode->GetArrayValue()->GetValuesRepr();
                    }
                }
            }
        }
        Log::GetInst()->AddNoNewlineMessage(s.str());
    }

    void ExecutionNodeVisitor::VisitQuitNode(Interpreter::QuitNode* pNode)
    {
        // Should not get here.
        assert(false);
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
                Log::GetInst()->AddMessage("a=b|c -- bitwise or");
                Log::GetInst()->AddMessage("a=b&c -- bitwise and");
                Log::GetInst()->AddMessage("a=b^c -- bitwise xor");
                Log::GetInst()->AddMessage("a=b||c -- logical or");
                Log::GetInst()->AddMessage("a=b&&c -- logical and");
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
        Log::GetInst()->AddMessage(".help()            -- prints this message");
        Log::GetInst()->AddMessage(".help(expr)        -- prints help on expressions");
        Log::GetInst()->AddMessage("");
        Log::GetInst()->AddMessage(".quit            -- quits");
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
        Log::GetInst()->AddMessage("print(\"String\", a, dec, b, oct, c, endl) -- print statement");
        Log::GetInst()->AddMessage("print(\"String\", width=10, fill=0, a, hex, b, c, endl) -- print statement");
        Log::GetInst()->AddMessage("// This is a one-line comment -- comments");
        Log::GetInst()->AddMessage("load(filename.txt) -- load and execute from a file");
        Log::GetInst()->AddMessage("clear()            -- clear all variables and functions");
        Log::GetInst()->AddMessage("{status,file} = fopen(\"file.bin\", \"rb\")");
        Log::GetInst()->AddMessage("status=fwrite(file, x)");
        Log::GetInst()->AddMessage("{status,q}=fread(file)");
        Log::GetInst()->AddMessage("status=fclose(file)");
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

        // We just parsed a set of nodes from the file.
        // Execute the nodes using the context that we currently have.
        for (Interpreter::Node* pNode = driver.GetResult(); pNode != nullptr; pNode = pNode->GetNext())
        {
            pNode->Accept(*this);
            if (IsErrorFlagSet())
            {
                driver.GetResult()->FreeList();
                delete contextStack.back();
                contextStack.pop_back();
                return;
            }
        }

        // done with the nodes
        driver.GetResult()->FreeList();

        delete contextStack.back();
        contextStack.pop_back();
    }

    void ExecutionNodeVisitor::VisitClearNode(Interpreter::ClearNode* pNode)
    {
        Interpreter::FunctionTable::GetInst()->Clear();
        m_pGlobalSymbolTable->Clear();

        SymbolTable::SymbolInfo* s = new SymbolTable::SymbolInfo;
        s->m_Name = "main";
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

        std::unique_ptr<ValueNode> pTop(dynamic_cast<ValueNode*>(m_Nodes.back()));
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

        std::unique_ptr<Node> pTop(m_Nodes.back());
        m_Nodes.pop_back();
        std::unique_ptr<ValueNode> pExprResult(GetTopOfStackValue(pTop.get()));

        if (pExprResult == nullptr)
        {
            return;
        }

        // If it's an entire array, error.
        if (pExprResult->IsArray())
        {
            ErrorInfo err(pWhileNode);
            err.m_Msg = ERROR_ARRAY_UNEXPECTED;
            SetErrorInfo(err);
            return;
        }

        bool result = pExprResult->GetValue().IfEval();
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

            std::unique_ptr<Node> pTop(m_Nodes.back());
            m_Nodes.pop_back();
            pExprResult.reset(GetTopOfStackValue(pTop.get()));

            if (pExprResult == nullptr)
            {
                return;
            }

            // If it's an entire array, error.
            if (pExprResult->IsArray())
            {
                ErrorInfo err(pWhileNode);
                err.m_Msg = ERROR_ARRAY_UNEXPECTED;
                SetErrorInfo(err);
                return;
            }

            result = pExprResult->GetValue().IfEval();
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

        std::unique_ptr<Node> pTop(m_Nodes.back());
        m_Nodes.pop_back();
        std::unique_ptr<ValueNode> pExprResult(GetTopOfStackValue(pTop.get()));
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

            pTop.reset(m_Nodes.back());
            m_Nodes.pop_back();
            pExprResult.reset(GetTopOfStackValue(pTop.get()));

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

        // If the function isn't found, then error.
        if (pDefNode == nullptr)
        {
            ErrorInfo err(pCallNode);
            char buf[512];
            sprintf_s(buf, sizeof(buf), ERROR_FUNCTION_NOT_FOUND, functionName.c_str());
            err.m_Msg = buf;
            SetErrorInfo(err);
            return;
        }

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
                std::unique_ptr<Node> pTop(m_Nodes.back());
                m_Nodes.pop_back();

                // We have a reference here.  The call expression is only allowed to be
                // a variable with no array specifier.  A reference is simply a rename
                // for a passed in parameter.
                VarNode* pVarNode = dynamic_cast<VarNode*>(pTop.get());
                if (pVarNode == nullptr)
                {
                    ErrorInterface::ErrorInfo err(pCallExprNode);
                    char buf[512];
                    sprintf_s(buf, sizeof(buf), ERROR_INVALID_REFERENCE_PARAMETER, pRefNode->GetName().c_str());
                    err.m_Msg = buf;
                    SetErrorInfo(err);
                    pLocalTable->Clear();
                    return;
                }

                // Find the variable first in the previous table on the stack.
                if (!IsSymbolPresent(pVarNode->GetName()))
                {
                    ErrorInterface::ErrorInfo err(pCallExprNode);
                    char buf[512];
                    sprintf_s(buf, sizeof(buf), ERROR_INVALID_REFERENCE_PARAMETER, pRefNode->GetName().c_str());
                    err.m_Msg = buf;
                    SetErrorInfo(err);
                    pLocalTable->Clear();
                    return;
                }

                SymbolTable::SymbolInfo* pReferencee = ReadSymbol(pVarNode->GetName(), true);
                assert(pReferencee->m_IsRef == false);

                SymbolTable::SymbolInfo* pReferencer = new SymbolTable::SymbolInfo;
                pReferencer->m_IsRef = true;
                pReferencer->m_Name = pRefNode->GetName();
                pReferencer->m_RefName = pReferencee->m_Name;
                pReferencer->m_pRefTable = pReferencee->m_pTable;

                bool status = pLocalTable->CreateSymbol(pRefNode->GetName(), pReferencer);
                if (!status)
                {
                    delete pReferencer;
                    ErrorInterface::ErrorInfo err(pCallExprNode);
                    char buf[512];
                    sprintf_s(buf, sizeof(buf), ERROR_INVALID_REFERENCE_PARAMETER, pRefNode->GetName().c_str());
                    err.m_Msg = buf;
                    SetErrorInfo(err);
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

                std::unique_ptr<Node> pTop(m_Nodes.back());
                m_Nodes.pop_back();

                std::unique_ptr<ValueNode> pValueNode(GetTopOfStackValue(pTop.get()));

                // Check if array
                if (pValueNode->IsArray())
                {
                    SymbolTable::SymbolInfo* pInfo = new SymbolTable::SymbolInfo;
                    assert(pInfo != nullptr);
                    pInfo->m_Name = symbol;
                    pInfo->m_IsArray = true;
                    pInfo->m_pArrayValue = pValueNode->GetArrayValue()->Clone();
                    pLocalTable->CreateSymbol(symbol, pInfo);
                }
                else
                {
                    SymbolTable::SymbolInfo* pInfo = new SymbolTable::SymbolInfo;
                    assert(pInfo != nullptr);
                    pInfo->m_Value = pValueNode->GetValue();
                    pLocalTable->CreateSymbol(symbol, pInfo);
                }
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
        if (!IsSymbolPresent(symbol))
        {
            ErrorInfo err(pTop);
            char buf[256];
            sprintf_s(buf, sizeof(buf), ERROR_MISSING_SYMBOL, symbol);
            err.m_Msg = buf;
            SetErrorInfo(err);
            return nullptr;
        }

        SymbolTable::SymbolInfo* pInfo = ReadSymbol(symbol, true);
        if (elementSpecifier.size() != 0)
        {
            if (pInfo->m_IsArray == false)
            {
                ErrorInfo err(pTop);
                char buf[256];
                sprintf_s(buf, sizeof(buf), ERROR_UNEXPECTED_ARRAY_SPECIFIER, symbol);
                err.m_Msg = buf;
                SetErrorInfo(err);
                return nullptr;
            }
        
            std::optional<Value> v = pInfo->m_pArrayValue->GetValue(elementSpecifier);
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
        else if (pInfo->m_IsArray)
        {
            pValueNode = new ValueNode;
            pValueNode->SetArrayValue(pInfo->m_pArrayValue->Clone());
        }
        else
        {
            pValueNode = new ValueNode;
            pValueNode->SetValue(pInfo->m_Value);
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
        for (Node* pExpr = pReturnNode->GetExpr(); pExpr != nullptr; pExpr = pExpr->GetNext())
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
            std::unique_ptr<Node> pTop(m_Nodes.back());
            m_Nodes.pop_back();

            ValueNode* pValueNode = GetTopOfStackValue(pTop.get());
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
        std::unique_ptr<Node> pTop(m_Nodes.back());
        m_Nodes.pop_back();

        std::unique_ptr<ValueNode> pValueNode(GetTopOfStackValue(pTop.get()));
        if (IsErrorFlagSet())
        {
            return;
        }

        if (pValueNode->IsArray())
        {
            ErrorInterface::ErrorInfo err(pValueNode.get());
            err.m_Msg = ErrorInterface::ERROR_ARRAY_UNEXPECTED;
            SetErrorInfo(err);
            return;
        }

        // Only ints allowed
        Value v = pValueNode->GetValue();
        if (v.GetType() != typeid(int))
        {
            ErrorInterface::ErrorInfo err(pValueNode.get());
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
        SymbolTable::SymbolInfo* pInfo = nullptr;
        if (pSymbolTable)
        {
            pInfo = pSymbolTable->ReadSymbol(pNode->GetName(), true);
            if (pInfo == nullptr)
            {
                pInfo = m_pGlobalSymbolTable->ReadSymbol(pNode->GetName());
            }
        }
        else
        {
            pInfo = m_pGlobalSymbolTable->ReadSymbol(pNode->GetName());
        }

        if (pInfo == nullptr)
        {
            ErrorInterface::ErrorInfo err(pNode);
            char buf[512];
            sprintf_s(buf, sizeof(buf), ERROR_MISSING_SYMBOL, pNode->GetName().c_str());
            err.m_Msg = buf;
            
            SetErrorInfo(err);
            return;
        }

        if (!pInfo->m_IsArray)
        {
            ErrorInterface::ErrorInfo err(pNode);
            char buf[512];
            sprintf_s(buf, sizeof(buf), ERROR_ENTIRE_ARRAY_EXPECTED, pNode->GetName().c_str());
            err.m_Msg = buf;
            
            SetErrorInfo(err);
            return;
        }

        std::vector<int> symbolDims = pInfo->m_pArrayValue->GetDims();
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

    std::optional<std::string> ExecutionNodeVisitor::GetFileOpenString(Node* pNode)
    {
        // We will open a file and push the value on the stack.
        // Get the filename that could be in a variable.
        pNode->Accept(*this);
        if (m_Nodes.size() == 0)
        {
            // Could not get the name
            return {};
        }

        std::unique_ptr<Node> pTop(m_Nodes.back());
        m_Nodes.pop_back();
        std::unique_ptr<ValueNode> pFilenameNode(GetTopOfStackValue(pTop.get()));
        if (pFilenameNode->IsArray())
        {
            ErrorInfo err(pFilenameNode.get());
            err.m_Msg = ERROR_ARRAY_UNEXPECTED;
            SetErrorInfo(err);
            return {};
        }

        Value fileNameValue = pFilenameNode->GetValue();
        if (fileNameValue.GetType() != typeid(std::string))
        {
            ErrorInfo err(pFilenameNode.get());
            err.m_Msg = ERROR_INCORRECT_TYPE;
            SetErrorInfo(err);
            return {};
        }

        return fileNameValue.GetStringValue();
    }

    // {status,f} = fopen("filename", "wb")
    void ExecutionNodeVisitor::VisitFileOpenNode(FileOpenNode* pNode)
    {
        std::optional<std::string> filename = GetFileOpenString(pNode->GetFilenameNode());
        if (filename == std::nullopt)
        {
            return;
        }

        std::optional<std::string> mode = GetFileOpenString(pNode->GetModeNode());
        if (mode == std::nullopt)
        {
            return;
        }

        // Create a varlist node for results.
        VarListNode* pVarListNode = new VarListNode;
        assert(pVarListNode != nullptr);

        File* pFile = File::Open(*filename, *mode);
        int status = pFile == nullptr ? 0 : 1;
        Value v(status);
        ValueNode* pStatus = new ValueNode(v);
        assert(pStatus != nullptr);

        v.SetFileValue(pFile);
        ValueNode* pFileResult = new ValueNode(v);
        assert(pFileResult != nullptr);

        pStatus->SetNext(pFileResult);
        pVarListNode->SetList(pStatus);
        m_Nodes.push_back(pVarListNode);
    }

    File* ExecutionNodeVisitor::GetFile(Node* pFileNode)
    {
        pFileNode->Accept(*this);
        if (IsErrorFlagSet())
        {
            return nullptr;
        }

        if (m_Nodes.size() == 0)
        {
            return nullptr;
        }

        std::unique_ptr<Node> pTop(m_Nodes.back());
        m_Nodes.pop_back();
        std::unique_ptr<ValueNode> pValueNode(GetTopOfStackValue(pTop.get()));

        if (pValueNode->IsArray())
        {
            ErrorInfo err;
            err.m_Msg = ERROR_ARRAY_UNEXPECTED;
            SetErrorInfo(err);
            return nullptr;
        }

        Value v = pValueNode->GetValue();
        if (v.GetType() != typeid(File*))
        {
            ErrorInfo err;
            err.m_Msg = ERROR_INCORRECT_TYPE;
            SetErrorInfo(err);
            return nullptr;
        }

        return v.GetFileValue();
    }

    // status = fwrite(f,x)
    void ExecutionNodeVisitor::VisitFileWriteNode(FileWriteNode* pNode)
    {
        std::unique_ptr<ValueNode> pStatusNode(new ValueNode);
        Value statusValue;

        // Get the file interface by processing file node.
        File* pFile = GetFile(pNode->GetFileNode());
        if (pFile == nullptr)
        {
            statusValue.SetIntValue(0);
            pStatusNode->SetValue(statusValue);
            m_Nodes.push_back(pStatusNode.release());
            return;
        }

        // Process the value to write.
        Node* pWriteNode = pNode->GetWriteNode();
        pWriteNode->Accept(*this);
        if (IsErrorFlagSet())
        {
            return;
        }

        if (m_Nodes.size() == 0)
        {
            return;
        }

        std::unique_ptr<Node> pTop(m_Nodes.back());
        m_Nodes.pop_back();
        std::unique_ptr<ValueNode> pValueNode(GetTopOfStackValue(pTop.get()));

        bool status;
        if (pValueNode->IsArray())
        {
            status = pFile->Write(pValueNode->GetArrayValue());
        }
        else 
        {
            status = pFile->Write(pValueNode->GetValue());
        }

        statusValue.SetIntValue(status);
        pStatusNode->SetValue(statusValue);
        m_Nodes.push_back(pStatusNode.release());
    }

    // {status,r} = fread(f)
    void ExecutionNodeVisitor::VisitFileReadNode(FileReadNode* pNode)
    {
        ValueNode* pStatusNode(new ValueNode);
        Value statusValue;

        ValueNode* pResultNode(new ValueNode);
        pStatusNode->SetNext(pResultNode);

        VarListNode* pVarListNode = new VarListNode;
        assert(pVarListNode != nullptr);
        pVarListNode->SetList(pStatusNode);

        // Get the file interface by processing file node.
        File* pFile = GetFile(pNode->GetFileNode());
        if (pFile == nullptr)
        {
            // status = 0, r = 0
            m_Nodes.push_back(pVarListNode);
            return;
        }

        bool isArray = false;
        ArrayValue* pArrayValue;
        Value v;
        bool status = pFile->Read(isArray, v, &pArrayValue);
        if (!status)
        {
            // Read failure.
            // status = 0, r = 0
            m_Nodes.push_back(pVarListNode);
            return;
        }

        // File operation succeeded.
        statusValue.SetIntValue(1);
        pStatusNode->SetValue(statusValue);

        if (isArray)
        {
            pResultNode->SetArrayValue(pArrayValue);
        }
        else
        {
            pResultNode->SetValue(v);
        }

        m_Nodes.push_back(pVarListNode);
    };

    // status = fclose(f)
    void ExecutionNodeVisitor::VisitFileCloseNode(FileCloseNode* pNode)
    {
        ValueNode* pStatusNode = new ValueNode;
        assert(pStatusNode != nullptr);
        Value statusValue;

        // Get the file interface by processing file node.
        File* pFile = GetFile(pNode->GetFileNode());
        if (pFile == nullptr)
        {
            // status = 0
            m_Nodes.push_back(pStatusNode);
            return;
        }

        statusValue.SetIntValue(1);
        pStatusNode->SetValue(statusValue);

        pFile->Close();

        m_Nodes.push_back(pStatusNode);
    }

    // {status,eof} = feof(f)
    void ExecutionNodeVisitor::VisitFileEofNode(FileEofNode* pNode)
    {
        ValueNode* pStatusNode = new ValueNode;
        assert(pStatusNode != nullptr);
        Value statusValue;

        ValueNode* pResultNode = new ValueNode;
        assert(pResultNode != nullptr);
        pStatusNode->SetNext(pResultNode);

        VarListNode* pVarListNode = new VarListNode;
        assert(pVarListNode != nullptr);
        pVarListNode->SetList(pStatusNode);

        // Get the file interface by processing file node.
        File* pFile = GetFile(pNode->GetFileNode());
        if (pFile == nullptr)
        {
            // status = 0
            m_Nodes.push_back(pVarListNode);
            return;
        }

        statusValue.SetIntValue(1);
        pStatusNode->SetValue(statusValue);

        bool status = pFile->Eof();
        Value eofValue(status);
        pResultNode->SetValue(eofValue);
        m_Nodes.push_back(pVarListNode);
    }

    bool ExecutionNodeVisitor::CreateSymbol(std::string name, SymbolTable::SymbolInfo* pSymbolInfo)
    {
        if (m_FunctionCallStack.size() != 0)
        {
            SymbolTable* pSymbolTable = m_FunctionCallStack.back()->GetSymbolTable();
            return pSymbolTable->CreateSymbol(name, pSymbolInfo);
        }

        return m_pGlobalSymbolTable->CreateSymbol(name, pSymbolInfo);
    }

    bool ExecutionNodeVisitor::IsSymbolPresent(std::string name)
    {
        if (m_FunctionCallStack.size() != 0)
        {
            SymbolTable* pSymbolTable = m_FunctionCallStack.back()->GetSymbolTable();
            bool status = pSymbolTable->IsSymbolPresent(name);
            if (status)
            {
                return status;
            }
        }

        return m_pGlobalSymbolTable->IsSymbolPresent(name);
    }

    SymbolTable::SymbolInfo* ExecutionNodeVisitor::ReadSymbol(std::string name, bool getroot)
    {
        if (m_FunctionCallStack.size() != 0)
        {
            SymbolTable* pSymbolTable = m_FunctionCallStack.back()->GetSymbolTable();
            SymbolTable::SymbolInfo* pSymbolInfo = pSymbolTable->ReadSymbol(name, getroot);
            if (pSymbolInfo != nullptr)
            {
                return pSymbolInfo;
            }
        }

        return m_pGlobalSymbolTable->ReadSymbol(name, getroot);
    }

    bool ExecutionNodeVisitor::UpdateSymbol(std::string name, SymbolTable::SymbolInfo* pSymbolInfo)
    {
        SymbolTable* pSymbolTable = pSymbolInfo->m_pTable;
        return pSymbolTable->UpdateSymbol(name, pSymbolInfo);
    }

    void ExecutionNodeVisitor::DeleteSymbol(SymbolTable::SymbolInfo* pSymbolInfo)
    {
        SymbolTable* pTable = pSymbolInfo->m_pTable;
        pTable->DeleteSymbol(pSymbolInfo->m_Name);
    }

    SymbolTable* ExecutionNodeVisitor::GetNoSymbolTable()
    {
        if (m_FunctionCallStack.size() != 0)
        {
            return m_FunctionCallStack.back()->GetSymbolTable();
        }

        return m_pGlobalSymbolTable;
    }
}
