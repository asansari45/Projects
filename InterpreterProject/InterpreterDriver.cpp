#include <sstream>
#include <assert.h>
#include <fstream>
#include "InterpreterDriver.hpp"
#include "InterpreterSymbolTable.hpp"
#include "InterpreterContext.h"

InterpreterDriver::InterpreterDriver(Interpreter::SymbolTable* pGlobalSymbols) :
    m_pParser(nullptr),
    m_pScanner(nullptr),
    m_pResult(nullptr),
    m_pGlobalSymbols(pGlobalSymbols),
    m_ErrorInfo()
{
}

InterpreterDriver::~InterpreterDriver()
{
}

int InterpreterDriver::Parse(const std::string input)
{
    std::istringstream ss(input);
    m_pScanner = new InterpreterScanner(&ss);
    assert(m_pScanner != nullptr);

    m_pParser = new yy::InterpreterParser(*m_pScanner, *this);
    assert(m_pParser != nullptr);

    int status = m_pParser->parse();
    delete m_pScanner;
    delete m_pParser;
    return status;
}

int InterpreterDriver::ParseFromFile(const std::string filename)
{
    char buf[512];
    sprintf_s(buf, sizeof(buf), "Parsing from file:  %s", filename.c_str());
    Interpreter::Log::GetInst()->AddMessage(buf);

    std::ifstream stream(filename.c_str());
    m_pScanner = new InterpreterScanner(&stream);
    assert(m_pScanner != nullptr);

    m_pParser = new yy::InterpreterParser(*m_pScanner, *this);
    assert(m_pParser != nullptr);

    int status = m_pParser->parse();
    delete m_pScanner;
    delete m_pParser;
    return status;
}

void InterpreterDriver::Error(std::string file, int line, int column, std::string msg)
{
    Interpreter::Context* pContext = Interpreter::contextStack.back();

    Interpreter::ErrorInterface::ErrorInfo err;
    err.m_File = pContext->GetFile();
    err.m_Line = line;
    err.m_Column = column;
    err.m_Msg = msg;
    m_ErrorInfo.SetErrorInfo(err);
}
