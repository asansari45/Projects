// InterpreterProject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
#include <string>
#include <io.h>
#include "InterpreterDriver.hpp"
#include "InterpreterNode.hpp"
#include "ExecutionNodeVisitor.hpp"
#include "InterpreterSymbolTable.hpp"
#include "InterpreterLog.h"
#include "InterpreterErrorInterface.h"
#include "InterpreterContext.h"

void ProcessError(Interpreter::ErrorInterface::ErrorInfo err, bool interactive)
{
    if (interactive)
    {
        char buf[512];
        std::memset(buf, ' ', sizeof(buf));
        buf[err.m_Column - 1] = '^';
        buf[err.m_Column] = '\0';
        Interpreter::Log::GetInst()->AddMessage(buf);
        Interpreter::Log::GetInst()->AddMessage(err.m_Msg);
    }
    else
    {
        char buf[512];
        sprintf_s(buf, sizeof(buf), "FILE:  %s, LINE:  %d, COLUMN:  %d  %s",
            err.m_File.c_str(), err.m_Line, err.m_Column, err.m_Msg.c_str());
        Interpreter::Log::GetInst()->AddMessage(buf);
    }
}

void ExecuteNodes(InterpreterDriver& driver, bool interactive)
{
    Interpreter::ErrorInterface err = driver.GetErrorInfo();
    if (err.IsErrorFlagSet())
    {
        ProcessError(err.GetErrorInfo(), interactive);
        Interpreter::Node* pNode = driver.GetResult();
        if (pNode)
        {
            pNode->FreeList();
        }
        return;
    }

    Interpreter::Node* pNode = driver.GetResult();
    while (pNode != nullptr)
    {
        Interpreter::ExecutionNodeVisitor nodeVisitor(driver.GetGlobalSymbols());
        pNode->Accept(nodeVisitor);

        if (nodeVisitor.IsErrorFlagSet())
        {
            Interpreter::ErrorInterface::ErrorInfo err = nodeVisitor.GetErrorInfo();
            ProcessError(err, interactive);

            // We're finished and done.
            pNode->FreeList();
            break;
        }

        if (pNode->GetNext() == nullptr)
        {
            std::optional<Interpreter:: Value> v = nodeVisitor.GetResult();
            if (v != std::nullopt)
            {
                char buf[512];
                sprintf_s(buf, sizeof(buf), "=%s", v.value().GetRepresentation().c_str());
                Interpreter::Log::GetInst()->AddMessage(buf);
            }
        }

        Interpreter::Node* pNext = pNode->GetNext();
        pNode->Free();
        pNode = pNext;
    }
}

int PerformInteractive()
{
    // Prompt
    Interpreter::Log::GetInst()->AddNoNewlineMessage(">");

    // Create a global symbol table.
    Interpreter::SymbolTable globalSymbols;

    // Add the symbol main
    Interpreter::Value v;
    bool status = globalSymbols.AddSymbol("main", v);
    assert(status);

    // read from stdin until EOF
    char input[256];
    char* pRet = gets_s(input, sizeof(input));
    while (pRet != nullptr)
    {
        // Create a new context for this line.
        Interpreter::Context* pContext = new Interpreter::Context;
        pContext->SetInput(pRet);
        pContext->SetLine(1);
        pContext->SetColumn(1);
        Interpreter::contextStack.push_back(pContext);
        InterpreterDriver driver(&globalSymbols);
        driver.Parse(input);

        ExecuteNodes(driver, true);
        delete Interpreter::contextStack.back();
        Interpreter::contextStack.pop_back();

        // Prompt
        Interpreter::Log::GetInst()->AddNoNewlineMessage(">");
        pRet = gets_s(input, sizeof(input));
    }

    return 0;
}

int PerformFromFile(const std::string filename)
{
    Interpreter::SymbolTable globalSymbols;
    Interpreter::Value v;
    bool status = globalSymbols.AddSymbol("main", v);
    assert(status);

    InterpreterDriver driver(&globalSymbols);

    Interpreter::Context* pContext = new Interpreter::Context;
    pContext->SetFile(filename);
    pContext->SetLine(1);
    pContext->SetColumn(1);
    Interpreter::contextStack.push_back(pContext);

    driver.ParseFromFile(filename);
    ExecuteNodes(driver, false);

    delete Interpreter::contextStack.back();
    Interpreter::contextStack.pop_back();

    return true;
}

int main(int argc, char* argv[])
{
    bool interactive = false;
    bool file = false;
    std::string filename;
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "--file") == 0)
        {
            file = true;
            filename = argv[i + 1];
        }
        else if (strcmp(argv[i], "--interactive") == 0)
        {
            interactive = true;
        }
    }

    if (!interactive && !file)
    {
        interactive = true;
    }

    if (interactive)
    {
        return PerformInteractive();
    }

    if (file)
    {
        return PerformFromFile(filename);
    }

    return 0;
}

extern "C"
{

void yyerror(const char* s)
{
    char buf[512];
    sprintf_s(buf, sizeof(buf), "###Parsing error:  %s", s);
    Interpreter::Log::GetInst()->AddMessage(buf);
}

int yywrap(void)
{
    return 1;
}

}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
