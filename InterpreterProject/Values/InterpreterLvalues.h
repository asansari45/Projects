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
    class Rvalue;

    class Lvalue
    {
    public:

        virtual ~Lvalue()
        {
        }

        // Perform the equals operation a=b
        virtual void Equ(Rvalue& rRvalue) = 0;

        // Perfrom the dim function.
        // a = dim[9+8]
        virtual void Dim(Rvalue& rRvalue) = 0;

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
        virtual void Equ(Rvalue& rValue);
        virtual void Dim(Rvalue& rValue);

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

        virtual void Equ(Rvalue& rValue);
        virtual void Dim(Rvalue& rValue)
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

        virtual void Equ(Rvalue& rValue);
        virtual void Dim(Rvalue& rValue);
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

        virtual void Equ(Rvalue& rValue);
        virtual void Dim(Rvalue& rValue);

    private:
    };


    class RefLvalue : public Lvalue
    {
    public:
        RefLvalue(std::string name, SymbolTable* pSymbolTable, ErrorInterface* pErrorInterface, ErrorInterface::ErrorInfo& rErrInfo) :
            Lvalue(name, pSymbolTable, pErrorInterface, rErrInfo)
        {
        }

        virtual ~RefLvalue()
        {
        }

        virtual void Equ(Rvalue& rValue);
        virtual void Dim(Rvalue& rValue);
    };

    class RefArraySpecifierLvalue : public Lvalue
    {
    public:
        RefArraySpecifierLvalue(std::string name, SymbolTable* pSymbolTable, ErrorInterface* pErrorInterface, 
                                ErrorInterface::ErrorInfo& rErrInfo,
                                std::vector<int>& rArraySpecifier) :
            Lvalue(name, pSymbolTable, pErrorInterface, rErrInfo),
            m_ArraySpecifier(rArraySpecifier)
        {
        }

        virtual ~RefArraySpecifierLvalue()
        {
        }

        virtual void Equ(Rvalue& rValue);
        virtual void Dim(Rvalue& rValue)
        {
            assert(false);
        }
    private:
        std::vector<int> m_ArraySpecifier;
    };
}
