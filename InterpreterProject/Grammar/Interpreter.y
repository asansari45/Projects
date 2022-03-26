/* Interpreter. */

// Create a C++ parser.
%language "c++"
%debug
%defines
%define parser_class_name InterpreterParser

// Returns only genuine tokens, not '+'.
// %define api.token.raw

// This example uses genuine C++ objects as semantic values, therefore, we require the variant-based storage of 
// semantic values. To make sure we properly use it, we enable assertions. To fully benefit from type-safety and 
// more natural definition of �symbol�, we enable api.token.constructor.

// %define api.token.constructor
// %define api.value.type variant
// %define parse.assert

%code requires {
class InterpreterDriver;
class InterpreterScanner;
namespace Interpreter
{
class Node;
};

// The following definitions is missing when %locations isn't used
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#define YYDEBUG 1

}

// The parsing context
%parse-param { InterpreterScanner& scanner }
%parse-param { InterpreterDriver& driver }
%locations

%code {
#include <stdio.h>
#include <string.h>
#include "Driver/InterpreterDriver.hpp"
#include "Nodes/InterpreterBinaryNode.h"
#include "Nodes/InterpreterBreakNode.h"
#include "Nodes/InterpreterClearNode.h"
#include "Nodes/InterpreterDimNode.h"
#include "Nodes/InterpreterForNode.h"
#include "Nodes/InterpreterFunctionDefNode.h"
#include "Nodes/InterpreterFunctionCallNode.h"
#include "Nodes/InterpreterFunctionsNode.h"
#include "Nodes/InterpreterHelpNode.h"
#include "Nodes/InterpreterIfNode.h"
#include "Nodes/InterpreterLenNode.h"
#include "Nodes/InterpreterLoadNode.h"
#include "Nodes/InterpreterPrintNode.h"
#include "Nodes/InterpreterPwdNode.h"
#include "Nodes/InterpreterQuitNode.h"
#include "Nodes/InterpreterStopNode.h"
#include "Nodes/InterpreterValueNode.h"
#include "Nodes/InterpreterWhileNode.h"
#include "Nodes/InterpreterRandNode.h"
#include "Nodes/InterpreterRefNode.h"
#include "Nodes/InterpreterReturnNode.h"
#include "Nodes/InterpreterSrandNode.h"
#include "Nodes/InterpreterVarListNode.h"
#include "Nodes/InterpreterVarsCmdNode.h"
#include "Nodes/InterpreterFileOpenNode.h"
#include "Nodes/InterpreterFileCloseNode.h"
#include "Nodes/InterpreterFileReadNode.h"
#include "Nodes/InterpreterFileWriteNode.h"
#include "Nodes/InterpreterFileEofNode.h"
#include "Nodes/InterpreterTypeIdNode.h"

#include "DebugMemory/DebugMemory.h"

#undef yylex
#define yylex scanner.yylex
}

%union {
    Interpreter::Node* m_pNode;
};

%token <m_pNode> NUM_
%token PLUS_
%token MINUS_
%token MULTIPLY_
%token DIVIDE_
%token EQUALS_
%token LES_
%token LEQ_
%token GRT_
%token GEQ_
%token DEQ_
%token NEQ_
%token OR_
%token AND_
%token XOR_
%token LSH_
%token RSH_
%token BNEG_
%token LOR_
%token LAND_
%token LPAREN_
%token RPAREN_
%token IF_
%token ELIF_
%token ELSE_
%token WHILE_
%token FOR_
%token LBRACKET_
%token RBRACKET_
%token FUNCTION_
%token ERR_
%token CLEAR_
%token RETURN_
%token COMMA_
%token DIM_
%token LARRAY_
%token RARRAY_
%token PWD_
%token FUNCTIONS_
%token VARS_
%token SRAND_
%token RAND_
%token LEN_
%token BREAK_
%token QUIT_
%token PRINT_
%token LOAD_
%token HELP_
%token FOPEN_
%token FWRITE_
%token FREAD_
%token FCLOSE_
%token FEOF_
%token HEX_
%token DEC_
%token OCT_
%token ENDL_
%token TYPEID_
%token INTERACTIVE_
%token <m_pNode> WIDTH_
%token <m_pNode> FILL_
%token <m_pNode> STRING_
%token <m_pNode> NAME_
%nterm <m_pNode> expression
%nterm <m_pNode> assignment
%nterm <m_pNode> command
%nterm <m_pNode> line
%nterm <m_pNode> lines
%nterm <m_pNode> if
%nterm <m_pNode> elif
%nterm <m_pNode> elif_list
%nterm <m_pNode> print
%nterm <m_pNode> while
%nterm <m_pNode> for
%nterm <m_pNode> funcdef
%nterm <m_pNode> funccall
%nterm <m_pNode> param_list
%nterm <m_pNode> param_comma_list
%nterm <m_pNode> param_comma
%nterm <m_pNode> funclines
%nterm <m_pNode> funcline
%nterm <m_pNode> return
%nterm <m_pNode> expression_list
%nterm <m_pNode> expression_comma_list
%nterm <m_pNode> expression_comma
%nterm <m_pNode> array_specifier
%nterm <m_pNode> lval_list
%nterm <m_pNode> rval_list
%nterm <m_pNode> srand
%nterm <m_pNode> rand
%nterm <m_pNode> len
%nterm <m_pNode> break
%nterm <m_pNode> param
%nterm <m_pNode> func_param
%nterm <m_pNode> func_param_comma
%nterm <m_pNode> func_param_list
%nterm <m_pNode> func_param_comma_list
%nterm <m_pNode> print_param_list
%nterm <m_pNode> print_param_comma_list
%nterm <m_pNode> print_param_comma
%nterm <m_pNode> print_param
%nterm <m_pNode> interactive_line
%nterm <m_pNode> typeid

%left DEQ_ NEQ_
%left LES_ LEQ_ GRT_ GEQ_
%left OR_ AND_ XOR_ LSH_ RSH_ LOR_ LAND_
%left PLUS_ MINUS_
%left MULTIPLY_ DIVIDE_
%right UMINUS_
%left LPAREN_ RPAREN_

%% /* Grammar rules and actions follow. */

%start program;
program :
    lines
    {
        driver.SetResult($1);
    }
    |
    INTERACTIVE_ interactive_line
    {
        driver.SetResult($2);
    }
    |
    INTERACTIVE_
    |
    ;

interactive_line:
    command
    |
    assignment
    |
    expression
    ;

lines:
    lines line
    {
        // Get the last node
        Interpreter::Node* pLast = nullptr;
        for (pLast=$1; pLast->GetNext() != nullptr; pLast = pLast->GetNext())
        {
        }

        pLast->SetNext($2);
    }
    |
    line
    ;

line:
    command
    |
    assignment
    |
    funcdef
    |
    while
    |
    for
    |
    if
    |
    funccall
    |
    srand
    |
    break
    ;

funclines : funclines funcline
    {
        // Get the last node
        Interpreter::Node* pLast = nullptr;
        for (pLast=$1; pLast->GetNext() != nullptr; pLast = pLast->GetNext())
        {
        }

        pLast->SetNext($2);
        $$ = $1;
    }
    |
    funcline
    ;

funcline :
    command
    |
    assignment
    |
    while
    |
    for
    |
    if
    |
    return
    |
    funccall
    |
    srand
    |
    break
    ;

srand :
    SRAND_ LPAREN_ expression RPAREN_
    {
        Interpreter::SrandNode* pNode = new Interpreter::SrandNode;
        pNode->SetExpr($3);
        $$ = pNode;
    }
    ;

rand :
    RAND_ LPAREN_ RPAREN_
    {
        Interpreter::RandNode* pNode = new Interpreter::RandNode;
        $$ = pNode;
    }
    ;

if:
    IF_ LPAREN_ expression RPAREN_ LBRACKET_ funclines RBRACKET_
    {
        Interpreter::IfNode* pNode = new Interpreter::IfNode;
        pNode->SetExpr($3);
        pNode->SetThen($6);
        $$ = pNode;
    }
    |
    IF_ LPAREN_ expression RPAREN_ LBRACKET_ funclines RBRACKET_ ELSE_ LBRACKET_ funclines RBRACKET_
    {
        Interpreter::IfNode* pNode = new Interpreter::IfNode;
        pNode->SetExpr($3);
        pNode->SetThen($6);
        pNode->SetElse($10);
        $$ = pNode;
    }
    |
    IF_ LPAREN_ expression RPAREN_ LBRACKET_ funclines RBRACKET_ elif_list ELSE_ LBRACKET_ funclines RBRACKET_
    {
        Interpreter::IfNode* pNode = new Interpreter::IfNode;
        pNode->SetExpr($3);
        pNode->SetThen($6);
        pNode->SetElif(dynamic_cast<Interpreter::IfNode*>($8));
        pNode->SetElse($11);
        $$ = pNode;
    }
    ;

elif_list : elif_list elif
    {
        Interpreter::Node* pNode = $1;
        for (; pNode->GetNext() != nullptr; pNode = pNode->GetNext());
        pNode->SetNext($2);
    }
    |
    elif
    ;

elif : ELIF_ expression LBRACKET_ funclines RBRACKET_
    {
        Interpreter::IfNode* pNode = new Interpreter::IfNode;
        pNode->SetExpr($2);
        pNode->SetThen($4);
        $$ = pNode;
    }
    ;

while : WHILE_ LPAREN_ expression RPAREN_ LBRACKET_ funclines RBRACKET_
    {
        Interpreter::WhileNode* pNode = new Interpreter::WhileNode;
        pNode->SetExpr($3);
        pNode->SetThen($6);
        $$ = pNode;
    }
    ;

for : FOR_ LPAREN_ assignment COMMA_ expression COMMA_ assignment RPAREN_ LBRACKET_ funclines RBRACKET_
    {
        Interpreter::ForNode* pNode = new Interpreter::ForNode;
        pNode->SetInit($3);
        pNode->SetExpr($5);
        pNode->SetThen($10);

        // Append the increment to the statements.
        Interpreter::Node* pLines = $10;
        for (; pLines->GetNext() != nullptr; pLines = pLines->GetNext());
        pLines->SetNext($7);
        $$ = pNode;
    }
    |
    FOR_ LPAREN_ assignment COMMA_ expression COMMA_ assignment RPAREN_ LBRACKET_ RBRACKET_
    {
        Interpreter::ForNode* pNode = new Interpreter::ForNode;
        pNode->SetInit($3);
        pNode->SetExpr($5);
        pNode->SetThen($7);
        $$ = pNode;
    }
    ;

funcdef : FUNCTION_ NAME_ LPAREN_ func_param_list RPAREN_ LBRACKET_ funclines RBRACKET_
         {
            std::string name = dynamic_cast<Interpreter::VarNode*>($2)->GetName();
            delete $2;
            Interpreter::FunctionDefNode* pNode = new Interpreter::FunctionDefNode(name);
            pNode->SetInputVars($4);
            pNode->SetCode($7);
            $$ = pNode;
         }
         |
         FUNCTION_ NAME_ LPAREN_ RPAREN_ LBRACKET_ funclines RBRACKET_
         {
            std::string name = dynamic_cast<Interpreter::VarNode*>($2)->GetName();
            delete $2;
            Interpreter::FunctionDefNode* pNode = new Interpreter::FunctionDefNode(name);
            pNode->SetCode($6);
            $$ = pNode;
         }
         |
         FUNCTION_ NAME_ LPAREN_ func_param_list RPAREN_ LBRACKET_ RBRACKET_
         {
            std::string name = dynamic_cast<Interpreter::VarNode*>($2)->GetName();
            delete $2;
            Interpreter::FunctionDefNode* pNode = new Interpreter::FunctionDefNode(name);
            pNode->SetInputVars($4);
            $$ = pNode;
         }
         |
         FUNCTION_ NAME_ LPAREN_ RPAREN_ LBRACKET_ RBRACKET_
         {
            std::string name = dynamic_cast<Interpreter::VarNode*>($2)->GetName();
            delete $2;
            Interpreter::FunctionDefNode* pNode = new Interpreter::FunctionDefNode(name);
            $$ = pNode;
         }
         ;

funccall : NAME_ LPAREN_ expression_list RPAREN_
         {
            Interpreter::FunctionCallNode* pNode = new Interpreter::FunctionCallNode;
            pNode->SetNameVar(dynamic_cast<Interpreter::VarNode*>($1));
            pNode->SetInputVars($3);
            $$ = pNode;
         }
         |
         NAME_ LPAREN_ RPAREN_
         {
            Interpreter::FunctionCallNode* pNode = new Interpreter::FunctionCallNode;
            pNode->SetNameVar(dynamic_cast<Interpreter::VarNode*>($1));
            $$ = pNode;
         }
         |
         FOPEN_ LPAREN_ expression COMMA_ expression RPAREN_
         {
             Interpreter::FileOpenNode* pFileNode = new Interpreter::FileOpenNode;
             pFileNode->SetFilenameNode($3);
             pFileNode->SetModeNode($5);
             $$ = pFileNode;
         }
         |
         FWRITE_ LPAREN_ NAME_ COMMA_ expression RPAREN_
         {
             Interpreter::FileWriteNode* pFileNode = new Interpreter::FileWriteNode;
             pFileNode->SetFileNode($3);
             pFileNode->SetWriteNode($5);
             $$ = pFileNode;
         }
         |
         FCLOSE_ LPAREN_ NAME_ RPAREN_
         {
             Interpreter::FileCloseNode* pFileNode = new Interpreter::FileCloseNode;
             pFileNode->SetFileNode($3);
             $$ = pFileNode;
         }
         |
         FREAD_ LPAREN_ NAME_ RPAREN_
         {
             Interpreter::FileReadNode* pFileNode = new Interpreter::FileReadNode;
             pFileNode->SetFileNode($3);
             $$ = pFileNode;
         }
         |
         FEOF_ LPAREN_ NAME_ RPAREN_
         {
             Interpreter::FileEofNode* pFileNode = new Interpreter::FileEofNode;
             pFileNode->SetFileNode($3);
             $$ = pFileNode;
         }
         ;

func_param_list : func_param_comma_list func_param
          {
              Interpreter::Node* pNode = $1;
              for (; pNode->GetNext() != nullptr; pNode = pNode->GetNext());
              pNode->SetNext($2);
          }
          |
          func_param
          ;

func_param_comma_list : func_param_comma_list func_param_comma
    {
        Interpreter::Node* pNode = $1;
        for (; pNode->GetNext() != nullptr; pNode = pNode->GetNext());
        pNode->SetNext($2);
    }
    |
    func_param_comma
    ;

func_param_comma : func_param COMMA_
    ;

func_param :
    AND_ NAME_
    {
        Interpreter::VarNode* pNode = dynamic_cast<Interpreter::VarNode*>($2);
        assert(pNode != nullptr);
        std::string name = pNode->GetName();
        delete pNode;

        Interpreter::RefNode* pRefNode = new Interpreter::RefNode;
        pRefNode->SetName(name);
        $$ = pRefNode;
    }
    |
    NAME_
    ;

param_list : param_comma_list param
          {
              Interpreter::Node* pNode = $1;
              for (; pNode->GetNext() != nullptr; pNode = pNode->GetNext());
              pNode->SetNext($2);
          }
          |
          param
          ;

param_comma_list : param_comma_list param_comma
    {
        Interpreter::Node* pNode = $1;
        for (; pNode->GetNext() != nullptr; pNode = pNode->GetNext());
        pNode->SetNext($2);
    }
    |
    param_comma
    ;

param_comma : param COMMA_
    ;

param :
    NAME_
    ;

expression_list : expression_comma_list expression
    {
        Interpreter::Node* pNode = $1;
        for (; pNode->GetNext() != nullptr; pNode = pNode->GetNext());
        pNode->SetNext($2);
        $$ = $1;
    }
    |
    expression
    {
        $$ = $1;
    }
    ;

expression_comma_list : expression_comma_list expression_comma
    {
        Interpreter::Node* pNode = $1;
        for (; pNode->GetNext() != nullptr; pNode = pNode->GetNext());
        pNode->SetNext($2);
        $$ = $1;
    }
    |
    expression_comma
    ;

expression_comma : expression COMMA_
    {
        $$ = $1;
    }
    ;

lval_list : LBRACKET_ param_list RBRACKET_
    {
        Interpreter::VarListNode* pNode = new Interpreter::VarListNode;
        pNode->SetList($2);
        pNode->SetLine($2->GetLine());
        $$ = pNode;
    }
    ;

rval_list : LBRACKET_ expression_list RBRACKET_
    {
        Interpreter::VarListNode* pNode = new Interpreter::VarListNode;
        pNode->SetList($2);
        $$ = pNode;
    }
    ;

assignment:
    NAME_ EQUALS_ expression
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::EQU);
        pNode->SetLeft($1);
        pNode->SetRight($3);
        $$ = pNode;
    }
    |
    NAME_ EQUALS_ DIM_ array_specifier
    {
        Interpreter::BinaryNode* pBinNode = new Interpreter::BinaryNode;
        pBinNode->SetOperator(Interpreter::BinaryNode::DIM);
        pBinNode->SetLeft($1);
        pBinNode->SetRight($4);
        $$ = pBinNode;
    }
    |
    NAME_ array_specifier EQUALS_ expression
    {
        Interpreter::BinaryNode* pAryNode = new Interpreter::BinaryNode;
        pAryNode->SetOperator(Interpreter::BinaryNode::ARY);
        pAryNode->SetLeft($1);
        pAryNode->SetRight($2);

        Interpreter::BinaryNode* pEquNode = new Interpreter::BinaryNode;
        pEquNode->SetOperator(Interpreter::BinaryNode::EQU);
        pEquNode->SetLeft(pAryNode);
        pEquNode->SetRight($4);
        $$ = pEquNode;
    }
    |
    NAME_ EQUALS_ rval_list
    {
        Interpreter::BinaryNode* pEquNode = new Interpreter::BinaryNode;
        pEquNode->SetOperator(Interpreter::BinaryNode::EQU);
        pEquNode->SetLeft($1);
        pEquNode->SetRight($3);
        $$ = pEquNode;
    }
    |
    lval_list EQUALS_ rval_list
    {
        Interpreter::BinaryNode* pEquNode = new Interpreter::BinaryNode;
        pEquNode->SetOperator(Interpreter::BinaryNode::EQU);
        pEquNode->SetLeft($1);
        pEquNode->SetRight($3);
        $$ = pEquNode;
    }
    |
    lval_list EQUALS_ funccall
    {
        Interpreter::BinaryNode* pEquNode = new Interpreter::BinaryNode;
        pEquNode->SetOperator(Interpreter::BinaryNode::EQU);
        pEquNode->SetLeft($1);
        pEquNode->SetRight($3);
        $$ = pEquNode;
    }
    ;

expression:
    MINUS_ expression %prec UMINUS_
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::NEG);
        pNode->SetLeft($2);
        $$ = pNode;
    }
    |
    BNEG_ expression %prec UMINUS_
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::BNEG);
        pNode->SetLeft($2);
        $$ = pNode;
    }
    |
    LPAREN_ expression RPAREN_
    {
        $$ = $2;
    }
    |
    expression PLUS_ expression
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::ADD);
        pNode->SetLeft($1);
        pNode->SetRight($3);
        $$ = pNode;
    }
    |
    expression MINUS_ expression
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::SUB);
        pNode->SetLeft($1);
        pNode->SetRight($3);
        $$ = pNode;
    }
    |
    expression MULTIPLY_ expression
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::MUL);
        pNode->SetLeft($1);
        pNode->SetRight($3);
        $$ = pNode;
    }
    |
    expression DIVIDE_ expression
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::DIV);
        pNode->SetLeft($1);
        pNode->SetRight($3);
        $$ = pNode;
    }
    |
    expression LES_ expression
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::LES);
        pNode->SetLeft($1);
        pNode->SetRight($3);
        $$ = pNode;
    }
    |
    expression LEQ_ expression
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::LEQ);
        pNode->SetLeft($1);
        pNode->SetRight($3);
        $$ = pNode;
    }
    |
    expression GRT_ expression
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::GRT);
        pNode->SetLeft($1);
        pNode->SetRight($3);
        $$ = pNode;
    }
    |
    expression GEQ_ expression
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::GEQ);
        pNode->SetLeft($1);
        pNode->SetRight($3);
        $$ = pNode;
    }
    |
    expression DEQ_ expression
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::DEQ);
        pNode->SetLeft($1);
        pNode->SetRight($3);
        $$ = pNode;
    }
    |
    expression NEQ_ expression
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::NEQ);
        pNode->SetLeft($1);
        pNode->SetRight($3);
        $$ = pNode;
    }
    |
    expression OR_ expression
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::OR);
        pNode->SetLeft($1);
        pNode->SetRight($3);
        $$ = pNode;
    }
    |
    expression AND_ expression
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::AND);
        pNode->SetLeft($1);
        pNode->SetRight($3);
        $$ = pNode;
    }
    |
    expression XOR_ expression
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::XOR);
        pNode->SetLeft($1);
        pNode->SetRight($3);
        $$ = pNode;
    }
    |
    expression LSH_ expression
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::LSH);
        pNode->SetLeft($1);
        pNode->SetRight($3);
        $$ = pNode;
    }
    |
    expression RSH_ expression
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::RSH);
        pNode->SetLeft($1);
        pNode->SetRight($3);
        $$ = pNode;
    }
    |
    expression LOR_ expression
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::LOR);
        pNode->SetLeft($1);
        pNode->SetRight($3);
        $$ = pNode;
    }
    |
    expression LAND_ expression
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::LAND);
        pNode->SetLeft($1);
        pNode->SetRight($3);
        $$ = pNode;
    }
    |
    funccall
    |
    NUM_
    |
    NAME_
    |
    NAME_ array_specifier
    {
        Interpreter::BinaryNode* pNode = new Interpreter::BinaryNode;
        pNode->SetOperator(Interpreter::BinaryNode::ARY);
        pNode->SetLeft($1);
        pNode->SetRight($2);
        $$ = pNode;
    }
    |
    STRING_
    |
    rand
    |
    len
    |
    typeid
    ;

typeid :
    TYPEID_ LPAREN_ expression RPAREN_
    {
        Interpreter::TypeIdNode* pNode = new Interpreter::TypeIdNode;
        pNode->SetExpr($3);
        $$ = pNode;
    }
    ;

len :
    LEN_ LPAREN_ NAME_ RPAREN_
    {
        Interpreter::LenNode* pNode = new Interpreter::LenNode;
        Interpreter::VarNode* pNameNode = dynamic_cast<Interpreter::VarNode*>($3);
        assert(pNameNode != nullptr);
        pNode->SetName(pNameNode->GetName());
        delete pNameNode;
        $$ = pNode;
    }
    |
    LEN_ LPAREN_ NAME_ COMMA_ NUM_ RPAREN_
    {
        Interpreter::LenNode* pNode = new Interpreter::LenNode;
        Interpreter::VarNode* pNameNode = dynamic_cast<Interpreter::VarNode*>($3);
        assert(pNameNode != nullptr);
        pNode->SetName(pNameNode->GetName());
        delete pNameNode;
        
        Interpreter::ValueNode* pValueNode = dynamic_cast<Interpreter::ValueNode*>($5);
        assert(pValueNode != nullptr);
        pNode->SetDim(pValueNode->GetValue().GetIntValue());
        delete pValueNode;
        $$ = pNode;
    }
    ;

array_specifier :
    LARRAY_ expression RARRAY_
    {
        Interpreter::DimNode* pDimNode = new Interpreter::DimNode;
        pDimNode->AddDim($2);
        $$ = pDimNode;
    }
    |
    LARRAY_ expression COMMA_ expression RARRAY_
    {
        Interpreter::DimNode* pDimNode = new Interpreter::DimNode;
        pDimNode->AddDim($2);
        pDimNode->AddDim($4);
        $$ = pDimNode;
    }
    |
    LARRAY_ expression COMMA_ expression COMMA_ expression RARRAY_
    {
        Interpreter::DimNode* pDimNode = new Interpreter::DimNode;
        pDimNode->AddDim($2);
        pDimNode->AddDim($4);
        pDimNode->AddDim($6);
        $$ = pDimNode;
    }
    ;

command:
    print
    |
    LOAD_ LPAREN_ STRING_ RPAREN_
    {
        Interpreter::LoadNode* pNode = new Interpreter::LoadNode;
        pNode->SetChild(dynamic_cast<Interpreter::ValueNode*>($3));
        $$ = pNode;
    }
    |
    HELP_ LPAREN_ NAME_ RPAREN_
    {
        Interpreter::HelpNode* pNode = new Interpreter::HelpNode;
        pNode->SetVar(dynamic_cast<Interpreter::VarNode*>($3));
        $$ = pNode;
    }
    |
    HELP_ LPAREN_ RPAREN_
    {
        Interpreter::HelpNode* pNode = new Interpreter::HelpNode;
        $$ = pNode;
    }
    |
    CLEAR_ LPAREN_ RPAREN_
    {
        Interpreter::ClearNode* pNode = new Interpreter::ClearNode;
        $$ = pNode;
    }
    |
    QUIT_
    {
        Interpreter::QuitNode* pNode = new Interpreter::QuitNode;
        $$ = pNode;
    }
    |
    PWD_
    {
        $$ = new Interpreter::PwdNode;
        $$->SetLine(@1.end.line);
        $$->SetColumn(@1.end.column);
    }
    |
    FUNCTIONS_
    {
        $$ = new Interpreter::FunctionsNode;
    }
    |
    VARS_
    {
        $$ = new Interpreter::VarsCmdNode;
    }
    ;

print:
    PRINT_ LPAREN_ print_param_list RPAREN_
    {
        Interpreter::PrintNode* pNode = new Interpreter::PrintNode;
        pNode->SetChild($3);
        $$ = pNode;
    }
    |
    PRINT_ LPAREN_ RPAREN_
    {
        Interpreter::PrintNode* pNode = new Interpreter::PrintNode;
        $$ = pNode;
    }
    ;

print_param_list : print_param_comma_list print_param
    {
        Interpreter::Node* pNode = $1;
        for (; pNode->GetNext() != nullptr; pNode = pNode->GetNext());
        pNode->SetNext($2);
        $$ = $1;
    }
    |
    print_param
    ;

print_param_comma_list : print_param_comma_list print_param_comma
    {
        Interpreter::Node* pNode = $1;
        for (; pNode->GetNext() != nullptr; pNode = pNode->GetNext());
        pNode->SetNext($2);
        $$ = $1;
    }
    |
    print_param_comma
    ;

print_param_comma : print_param COMMA_
    ;

print_param:
    expression
    |
    HEX_
    {
        $$ = new Interpreter::PrintNode(Interpreter::PrintNode::HEX);
    }
    |
    DEC_
    {
        $$ = new Interpreter::PrintNode(Interpreter::PrintNode::DEC);
    }
    |
    OCT_
    {
        $$ = new Interpreter::PrintNode(Interpreter::PrintNode::OCT);
    }
    |
    ENDL_
    {
        $$ = new Interpreter::PrintNode(Interpreter::PrintNode::ENDL);
    }
    |
    WIDTH_
    {
        Interpreter::ValueNode* pValueNode = dynamic_cast<Interpreter::ValueNode*>($1);
        assert(pValueNode != nullptr);
        assert(pValueNode->IsArray() == false);
        int width = pValueNode->GetValue().GetIntValue();
        delete pValueNode;
        Interpreter::PrintNode* pNode = new Interpreter::PrintNode(Interpreter::PrintNode::WIDTH,width);
        $$ = pNode;
    }
    |
    FILL_
    {
        Interpreter::ValueNode* pValueNode = dynamic_cast<Interpreter::ValueNode*>($1);
        assert(pValueNode != nullptr);
        assert(pValueNode->IsArray() == false);
        int fill = pValueNode->GetValue().GetIntValue();
        delete pValueNode;
        Interpreter::PrintNode* pNode = new Interpreter::PrintNode(Interpreter::PrintNode::FILL,fill);
        $$ = pNode;
    }
    ;

return:
    RETURN_ LPAREN_ expression_list RPAREN_
    {
        Interpreter::ReturnNode* pNode = new Interpreter::ReturnNode;
        pNode->SetExpr($3);
        $$ = pNode;
    }
    |
    RETURN_ LPAREN_ RPAREN_
    {
        Interpreter::ReturnNode* pNode = new Interpreter::ReturnNode;
        $$ = pNode;
    }
    ;

break:
    BREAK_
    {
        $$ = new Interpreter::BreakNode;
    }
    ;

%%
// Finally the error member function reports the errors.

void yy::InterpreterParser::error (const location_type& l, const std::string& m)
{
    std::string file = l.end.filename == nullptr ? "" : *l.end.filename;
    driver.Error(file, l.end.line, l.end.column, m);
}
