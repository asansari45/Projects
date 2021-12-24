#pragma once

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "InterpreterParser.hpp"
#include "InterpreterLog.h"
#include "location.hh"

class InterpreterScanner : public yyFlexLexer
{
public:

    InterpreterScanner(std::istream* in) :
        yyFlexLexer(in),
        m_pYylval(nullptr),
        m_Pos()
    {
    }

    using FlexLexer::yylex;

    virtual int yylex(yy::InterpreterParser::semantic_type* const lval,
        yy::InterpreterParser::location_type* location);

    void Error(const std::string message)
    {
        char buf[512];
        sprintf_s(buf, sizeof(buf), "###Error:  %s Line=%d Column=%d", message.c_str(), m_Pos.line, m_Pos.column);
        Interpreter::Log::GetInst()->AddMessage(buf);
    }

    int GetLine() { return m_Pos.line; }

private:
    yy::InterpreterParser::semantic_type* m_pYylval;
    yy::position m_Pos;
};
