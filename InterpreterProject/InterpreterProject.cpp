// InterpreterProject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
#include <string>
#include <chrono>
#include "Algorithm/InterpreterAlgorithmRepository.h"
#include "Driver/InterpreterDriver.hpp"
#include "Nodes/InterpreterNode.h"
#include "Visitors/InterpreterExecutionNodeVisitor.h"
#include "Tables/InterpreterFunctionTable.h"
#include "Tables/InterpreterSymbolTable.h"
#include "Log/InterpreterLog.h"
#include "Visitors/InterpreterErrorInterface.h"
#include "Driver/InterpreterContext.h"
#include "Nodes/InterpreterQuitNode.h"

#include "DebugMemory/DebugMemory.h"

// #define METRICS 0

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

bool ExecuteNodes(InterpreterDriver& driver, bool interactive)
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

        // If we're interactive, we want to keep going...
        if (interactive)
        {
            return true;
        }

        return false;
    }

    Interpreter::Node* pNode = driver.GetResult();
    while (pNode != nullptr)
    {
        // If we see the quit node, we're done.
        Interpreter::QuitNode* pQuitNode = dynamic_cast<Interpreter::QuitNode*>(pNode);
        if (pQuitNode != nullptr)
        {
            // Do not continue
            pNode->FreeList();
            return false;
        }

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

    // Continue
    return true;
}

int PerformInteractive()
{
    // Prompt
    Interpreter::Log::GetInst()->AddNoNewlineMessage(">");

    // Create a global symbol table.
    Interpreter::SymbolTable* pGlobalSymbols = Interpreter::SymbolTable::CreateGlobalSymbols();

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
        InterpreterDriver driver(pGlobalSymbols);
        driver.Parse(input);

        bool cont = ExecuteNodes(driver, true);
        delete Interpreter::contextStack.back();
        Interpreter::contextStack.pop_back();

        if (!cont)
        {
            break;
        }

        // Prompt
        Interpreter::Log::GetInst()->AddNoNewlineMessage(">");
        pRet = gets_s(input, sizeof(input));
    }

    Interpreter::SymbolTable::DeleteGlobalSymbols(pGlobalSymbols);
    Interpreter::FunctionTable::Shutdown();
    return 0;
}

int PerformFromFile(const std::string filename)
{
    // Check if file exists first.
    struct stat info;
    if (stat(filename.c_str(), &info) != 0)
    {
        char buf[512];
        sprintf_s(buf, sizeof(buf), "%s file not found.", filename.c_str());
        Interpreter::Log::GetInst()->AddMessage(buf);
        return 0;
    }

    Interpreter::SymbolTable* pGlobalSymbols = Interpreter::SymbolTable::CreateGlobalSymbols();

    InterpreterDriver driver(pGlobalSymbols);

    Interpreter::Context* pContext = new Interpreter::Context;
    pContext->SetFile(filename);
    pContext->SetLine(1);
    pContext->SetColumn(1);
    Interpreter::contextStack.push_back(pContext);

#if defined(METRICS)
    auto t1 = std::chrono::high_resolution_clock::now();
#endif
    driver.ParseFromFile(filename);

#if defined(METRICS)
    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1);
    printf("Time spent parsing:  %d msec\n", ms_int);

    t1 = std::chrono::high_resolution_clock::now();
#endif

    ExecuteNodes(driver, false);

#if defined(METRICS)
    t2 = std::chrono::high_resolution_clock::now();
    ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    printf("Time spent executing:  %d msec\n", ms_int);
#endif

    delete Interpreter::contextStack.back();
    Interpreter::contextStack.pop_back();

    Interpreter::SymbolTable::DeleteGlobalSymbols(pGlobalSymbols);
    Interpreter::FunctionTable::Shutdown();
    return true;
}

int main(int argc, char* argv[])
{
    bool interactive = false;
    bool file = false;
    bool memleak = false;
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
        else if (strcmp(argv[i], "--debug") == 0)
        {
            Interpreter::Log::GetInst()->SetType(Interpreter::Log::DEBUG);
        }
    }

    if (!interactive && !file)
    {
        interactive = true;
    }

    if (interactive)
    {
        PerformInteractive();
    }

    if (file)
    {
        PerformFromFile(filename);
    }

    Interpreter::Log::Shutdown();
    Interpreter::AlgorithmRepository::Shutdown();

#if defined(DEBUG_MEMORY)
    PrintStatistics();
#endif


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
