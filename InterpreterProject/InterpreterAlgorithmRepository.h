#pragma once
#include <vector>
#include <optional>
#include "InterpreterNode.hpp"
#include "InterpreterSymbolTable.hpp"
#include "InterpreterLog.h"

namespace Interpreter
{
class SymbolTable;
class ErrorInterface;

class BinaryFunc
{
public:
    virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface) = 0;

protected:
    struct RvalueProperties
    {
        RvalueProperties() :
            m_Name(),
            m_ArraySel(false),
            m_Value(),
            m_ArrayValue()
        {
        }
        std::string m_Name;
        bool m_ArraySel;
        Value m_Value;
        ArrayValue m_ArrayValue;
    };

    struct LvalueProperties
    {
        LvalueProperties() :
            m_SymbolPresent(false),
            m_SymbolArray(false),
            m_pSymbolTable(nullptr),
            m_SymbolName(),
            m_SymbolArraySpecifier()
        {
        }

        bool m_SymbolPresent;
        bool m_SymbolArray;
        SymbolTable* m_pSymbolTable;
        std::string m_SymbolName;
        std::vector<int> m_SymbolArraySpecifier;
    };

    bool IsArray(const RvalueProperties r) { return r.m_ArraySel; }
    bool IsArray(const LvalueProperties l) { return l.m_SymbolArray && l.m_SymbolArraySpecifier.size() == 0; }

    std::optional<RvalueProperties> GetRvalueProperties(Node * pNode, SymbolTable * pGlobalSymbols, SymbolTable * pLocalSymbols, ErrorInterface* pErrorInterface);
    std::optional<LvalueProperties> GetLvalueProperties(Node* pNode, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface);

    std::optional<Value> GetRvalue(Node* pNode, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols);
    std::optional<VarNode*> GetLvalue(Node* pNode);
};

class AlgorithmRepository
{
public:
    static AlgorithmRepository* GetInst();
    BinaryFunc* Lookup(BinaryNode::Operator oper);

private:
    AlgorithmRepository();
    ~AlgorithmRepository();
    static AlgorithmRepository* m_pInst;
    static BinaryFunc* m_pFunctionTable[];
};


};
