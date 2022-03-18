#pragma once
#include <vector>
#include <string>
#include "Visitors/InterpreterErrorInterface.h"

namespace Interpreter
{
    class SymbolTable;
    class ErrorInterface;
    class Node;
    class VarNode;
    class ValueNode;
    class VarList;

    class Lvalue
    {
    public:

        virtual ~Lvalue()
        {
        }

        // Perform the equals operation a=b
        virtual void Equ(ValueNode* pRvalue) = 0;

        // Perform the equals operation a={0,1,2,...}
        virtual void EquList(ArrayValue* pArrayValue) = 0;

        // Perfrom the dim function.
        // a = dim[9+8]
        virtual void Dim(ValueNode* pRvalue) = 0;

    protected:
        Lvalue(std::string name, SymbolTable* pSymbolTable, ErrorInterface* pErrIf, ErrorInterface::ErrorInfo& rErrInfo) :
            m_Name(name),
            m_pSymbolTable(pSymbolTable),
            m_pErrIf(pErrIf),
            m_ErrInfo(rErrInfo)
        {
        }

    protected:
        std::string m_Name;
        SymbolTable* m_pSymbolTable;
        ErrorInterface* m_pErrIf;
        ErrorInterface::ErrorInfo m_ErrInfo;
        char m_Buf[256];
    };

    class NoSymbolLvalue : public Lvalue
    {
    public:
        NoSymbolLvalue(std::string name,
                       std::vector<int> arraySpecifier,
                       SymbolTable* pSymbolTable, 
                       ErrorInterface* pErrorInterface, 
                       ErrorInterface::ErrorInfo& rErrInfo) :
            Lvalue(name, pSymbolTable, pErrorInterface, rErrInfo),
            m_ArraySpecifier(arraySpecifier)
        {
        }

        virtual ~NoSymbolLvalue()
        {
        }
        void Equ(ValueNode* pRvalue) override;
        void EquList(ArrayValue* pArrayValue) override;
        void Dim(ValueNode* pRvalue) override;

    private:
        std::vector<int> m_ArraySpecifier;
    };

    class ArrayElementLvalue : public Lvalue
    {
    public:
        ArrayElementLvalue(std::string name, SymbolTable* pSymbolTable, ErrorInterface* pErrorInterface,
            ErrorInterface::ErrorInfo& rErrInfo, std::vector<int>& rArraySpecifier) :
            Lvalue(name, pSymbolTable, pErrorInterface, rErrInfo),
            m_ArraySpecifier(rArraySpecifier)
        {
        }

        virtual ~ArrayElementLvalue()
        {
        }

        void Equ(ValueNode* pRvalue) override;
        void EquList(ArrayValue* pArrayValue) override
        {
            assert(false);
        }
        void Dim(ValueNode* pRvalue) override
        {
            assert(false);
        }

    private:
        std::vector<int> m_ArraySpecifier;
    };

    class WholeArrayLvalue : public Lvalue
    {
    public:
        WholeArrayLvalue(std::string name, SymbolTable* pSymbolTable, ErrorInterface* pErrorInterface, ErrorInterface::ErrorInfo& rErrInfo) :
            Lvalue(name, pSymbolTable, pErrorInterface, rErrInfo)
        {
        }

        virtual ~WholeArrayLvalue()
        {
        }

        void Equ(ValueNode* pRvalue);
        void EquList(ArrayValue* pArrayValue) override;
        void Dim(ValueNode* pRvalue);
    private:
    };

    class VariableLvalue : public Lvalue
    {
    public:
        VariableLvalue(std::string name, SymbolTable* pSymbolTable, ErrorInterface* pErrorInterface, ErrorInterface::ErrorInfo& rErrInfo) :
            Lvalue(name, pSymbolTable, pErrorInterface, rErrInfo)
        {
        }

        virtual ~VariableLvalue()
        {
        }

        void Equ(ValueNode* pRvalue) override;
        void EquList(ArrayValue* pArrayValue) override
        {
            assert(false);
        }
        void Dim(ValueNode* pRvalue) override;

    private:
    };

}
