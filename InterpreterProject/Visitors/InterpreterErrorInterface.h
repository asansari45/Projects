#pragma once
#include <string>
#include "Nodes/InterpreterNode.h"

namespace Interpreter
{
    class ErrorInterface
    {
    public:
        const char* ERROR_MISSING_SYMBOL = "%s is not defined.";
        const char* ERROR_INCORRECT_ARRAY_SPECIFIER = "%s contains an incorrect array specifier.";
        const char* ERROR_UNEXPECTED_ARRAY_SPECIFIER = "%s has an unexpected array specifier.";
        const char* ERROR_ARRAY_OPERATION_FAILED = "Array operation failed on array.  Wrong dimensions or element values of different types.";
        const char* ERROR_ARRAY_SPECIFIER_EXPECTED = "Array specifier expected here.";
        const char* ERROR_ARRAY_UNEXPECTED = "Array unexpected here.";
        const char* ERROR_ENTIRE_ARRAY_EXPECTED = "Expected entire array.";
        const char* ERROR_BINARY_OPERATION_FAILED = "Binary operation failed.";
        const char* ERROR_UNARY_OPERATION_FAILED  = "Unary operation failed.";
        const char* ERROR_INVALID_EXPRESSION_IN_IF_STATEMENT = "Invalid expression in if statement.";
        const char* ERROR_INVALID_EXPRESSION_IN_WHILE_STATEMENT = "Invalid expression in while statement.";
        const char* ERROR_INVALID_EXPRESSION_IN_FOR_STATEMENT = "Invalid expression in for statement.";
        const char* ERROR_FUNCTION_NOT_FOUND = "%s function is not found.";
        const char* ERROR_FUNCTION_CREATION = "%s function could not be created.  Duplicate symbol may be present.";
        const char* ERROR_FUNCTION_ARGS = "%s function has %d arguments.  Called with %d arguments.";
        const char* ERROR_FUNCTION_EXPRESSION = "%s function has an invalid expression for an argument.";
        const char* ERROR_VARLIST_ARGS = "%d arguments on left %d arguments on right.";
        const char* ERROR_VARLIST_BOTH_SIDES = "Both sides of equation must be variable lists.";
        const char* ERROR_INCORRECT_TYPE = "Wrong type specified for the operation.";
        const char* ERROR_INCORRECT_LEN_DIMS = "Valid dimensions for %s are 0-%d.";
        const char* ERROR_INCORRECT_DIM = "Incorrect dimension specified.";
        const char* ERROR_FILE_DOES_NOT_EXIST = "%s file does not exist.";
        const char* ERROR_FILE_OPERATION_FAILED = "File operation failed.";
        const char* ERROR_INVALID_REFERENCE_PARAMETER = "Invalid reference found for parameter %s.";
        const char* ERROR_INVALID_HELP_PARAMETER = "Invalid help parameter %s.";

        ErrorInterface() : m_ErrorFlag(false),
                           m_ErrInfo()
        {
        }
        virtual ~ErrorInterface() 
        {
        }

        struct ErrorInfo
        {
            ErrorInfo() :
                m_File(),
                m_Line(0),
                m_Column(0),
                m_Msg()
            {
            }

            ErrorInfo(Node* pNode) :
                m_File(pNode->GetFile()),
                m_Line(pNode->GetLine()),
                m_Column(pNode->GetColumn()),
                m_Msg()
            {
            }
            std::string m_File;
            int m_Line;
            int m_Column;
            std::string m_Msg;
        };
        bool IsErrorFlagSet() { return m_ErrorFlag; }
        ErrorInfo GetErrorInfo() { return m_ErrInfo; }
        void SetErrorInfo(ErrorInfo err) { m_ErrorFlag = true; m_ErrInfo = err; }
    private:
        bool m_ErrorFlag;
        ErrorInfo m_ErrInfo;
    };
};
