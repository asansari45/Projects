#pragma once
// This class defines the entire Interpreter, both parsing and tokenizing.
#include "InterpreterScanner.hpp" 
#include "InterpreterParser.hpp"
#include "InterpreterErrorInterface.h"

// Give Flex the prototype of yylex we want ...
// # define YY_DECL \
// yy::parser::symbol_type yylex (Interpreter& intr)
// ... and declare it for the parser's sake.
// YY_DECL;

// Forward declarations
namespace Interpreter
{
class Node;
class SymbolTable;
};

class InterpreterDriver
{
public:

    InterpreterDriver(Interpreter::SymbolTable* pGlobalSymbols);
    virtual ~InterpreterDriver();

    // Parse the input from a string
    int Parse(const std::string input);

    // Parse from a file.
    int ParseFromFile(const std::string filename);

    // Get/Set for the final node
    void SetResult(Interpreter::Node* pResult) { m_pResult = pResult; }
    Interpreter::Node* GetResult() { return m_pResult; }
    Interpreter::ErrorInterface GetErrorInfo() { return m_ErrorInfo; }
    Interpreter::SymbolTable* GetGlobalSymbols() { return m_pGlobalSymbols; }
    void Error(std::string file, int line, int column, std::string msg);

private:

    yy::InterpreterParser* m_pParser;
    InterpreterScanner* m_pScanner;

    Interpreter::Node* m_pResult;
    Interpreter::SymbolTable* m_pGlobalSymbols;
    Interpreter::ErrorInterface m_ErrorInfo;
};
