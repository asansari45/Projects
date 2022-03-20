%{
#include <stdio.h>
#include <string>

// Implementation of yyFlexScanner
#include "InterpreterScanner.hpp"
#include "Nodes/InterpreterLoadNode.h"
#include "Nodes/InterpreterQuitNode.h"
#include "Nodes/InterpreterValueNode.h"
#include "Nodes/InterpreterVarNode.h"
#include "Tables/InterpreterFunctionTable.h"
#include "Driver/InterpreterContext.h"
#include "DebugMemory/DebugMemory.h"

// This global variable is used to determine the appropriate start grammar rule.
int f_StartToken = 0;

#undef  YY_DECL
#define YY_DECL int InterpreterScanner::yylex( yy::InterpreterParser::semantic_type * const lval, yy::InterpreterParser::location_type *location )

/* using "token" to make the returns for the tokens shorter to type */
using token = yy::InterpreterParser::token;

/* define yyterminate as this instead of NULL */
// #define yyterminate() return( token::END )

// update location on matching
int yycolumn = 1;

int print_parens = 0;

#define YY_USER_ACTION location->columns(yyleng); Interpreter::contextStack.back()->IncColumn(yyleng);
#define YYABORT        goto yyabortlab

%}

%option debug
%option nodefault
%option yyclass="InterpreterScanner"
%option noyywrap
%option c++
%option yylineno
%s print
%%
%{
    m_pYylval = lval;
    if (f_StartToken != 0)
    {
        f_StartToken = 0;
        return token::INTERACTIVE_;
    }
%}

".pwd" {return token::PWD_;}
".functions" { return token::FUNCTIONS_;}
".vars" { return token::VARS_;}
".quit" { return token::QUIT_;}
".help" { return token::HELP_;}
"load" { return token::LOAD_;}
"print" {
            BEGIN print;
            return token::PRINT_;
}

"clear" { return token::CLEAR_;}
"if" {return token::IF_;}
"elif" {    return token::ELIF_;}
"else" { return token::ELSE_;}
"while" { return token::WHILE_;}
"for" { return token::FOR_;}
"return" {return token::RETURN_;}
"dim" { return token ::DIM_;}
"[" { return token::LARRAY_;}
"]" { return token::RARRAY_;}
"srand" { return token::SRAND_;}
"rand" { return token::RAND_;}
"function" { return token::FUNCTION_;}
"len" { return token::LEN_;}
"break" { return token::BREAK_;}
"fopen" { return token::FOPEN_;}
"fwrite" { return token::FWRITE_;}
"fread" { return token::FREAD_;}
"fclose" { return token::FCLOSE_;}
"feof" { return token::FEOF_;}
","  { return token::COMMA_;}
"~"  {return token::BNEG_;}
"<<" {return token::LSH_;}
">>" {return token::RSH_;}
"||" {return token::LOR_;}
"&&" {return token::LAND_;}
"<=" { return token::LEQ_; }
"=<" { return token::LEQ_; }
"==" { return token::DEQ_; }
"!=" { return token::NEQ_; }
">=" { return token::GEQ_; }
"=>" { return token::GEQ_; }
"^"  { return token::XOR_; }
"\+" {return token::PLUS_;}
"\-" {return token::MINUS_;}
"\*" {return token::MULTIPLY_;}
"\/" {return token::DIVIDE_;}
"\=" {return token::EQUALS_;}
">" { return token::GRT_; }
"<" { return token::LES_; }
"|"  { return token::OR_; }
"&" { return token::AND_;}
"\(" { print_parens++; return token::LPAREN_; }
"\)" { 
    print_parens--;
    if (print_parens == 0)
    {
        BEGIN INITIAL;
    }
    return token::RPAREN_; 
}

"\{" { return token::LBRACKET_; }
"\}" { return token::RBRACKET_; }

<print>"hex" { return token::HEX_; }
<print>"dec" { return token::DEC_; }
<print>"oct" { return token::OCT_; }
<print>"endl" { return token::ENDL_; }
<print>"width="[0-9]+ { char* ptr = strchr(yytext, '=');
                        ptr++;
                        Interpreter::Value v;
                        v.SetIntValue(atoi(ptr));
                        Interpreter::ValueNode* pNode = new Interpreter::ValueNode(v);
                        m_pYylval->m_pNode = pNode;
                        return token::WIDTH_; }
<print>"fill="[a-zA-Z0-9\*] { char* ptr = strchr(yytext, '=');
                        ptr++;
                        Interpreter::Value v;
                        v.SetIntValue(static_cast<int>(*ptr));
                        Interpreter::ValueNode* pNode = new Interpreter::ValueNode(v);
                        m_pYylval->m_pNode = pNode;
                        return token::FILL_; }

b[0-1]+ {
    int result = Interpreter::Value::ConvertBinary(&yytext[1]);
    Interpreter::Value v;
    v.SetIntValue(result); 
    Interpreter::ValueNode* pNode = new Interpreter::ValueNode(v);
    m_pYylval->m_pNode = pNode;
    return token::NUM_;
}

[a-zA-Z][a-zA-Z0-9_]* {
            Interpreter::VarNode* pNode = new Interpreter::VarNode;
            pNode->SetName(yytext);
            m_pYylval->m_pNode = pNode;
            return token::NAME_;
          }

\"[^"]*\" {
            char noquotes[256];
            strcpy_s(noquotes, sizeof(noquotes), &yytext[1]);
            noquotes[strlen(noquotes)-1] = '\0';
            Interpreter::Value v;
            v.SetStringValue(Interpreter::Value::HarvestControlChars(noquotes));
            m_pYylval->m_pNode = new Interpreter::ValueNode(v);
            return token::STRING_;
}

0[xX][0-9a-fA-F]+ {
            Interpreter::Value v;
            v.SetIntValue(std::stoul(yytext, nullptr, 16));
            Interpreter::ValueNode* pNode = new Interpreter::ValueNode(v);
            m_pYylval->m_pNode = pNode;
            return token::NUM_;
        }

[0-9]*[\.][0-9]+ {
            Interpreter::Value v;
            v.SetFloatValue(std::stof(yytext));
            Interpreter::ValueNode* pNode = new Interpreter::ValueNode(v);
            m_pYylval->m_pNode = pNode;
            return token::NUM_;
        }

[0-9]+   {
            Interpreter::Value v;
            v.SetIntValue(atoi(yytext));
            Interpreter::ValueNode* pNode = new Interpreter::ValueNode(v);
            m_pYylval->m_pNode = pNode;
            return token::NUM_;
        }


[ \t\r]+ /* eat up whitespace */
[\n] { 
        location->lines(); 
        Interpreter::contextStack.back()->IncLine();
        Interpreter::contextStack.back()->SetColumn(1);
     }

"//".* { } /* one-line comments */ 

.          {
             Error("invalid character: " + std::string(yytext));
}

%%
