#include <assert.h>
#include <optional>
#include "InterpreterAlgorithmRepository.h"
#include "InterpreterSymbolTable.hpp"
#include "InterpreterLog.h"
#include "InterpreterContext.h"
#include "InterpreterErrorInterface.h"

namespace Interpreter
{

AlgorithmRepository* AlgorithmRepository::m_pInst = nullptr;

AlgorithmRepository* AlgorithmRepository::GetInst()
{
    if (m_pInst == nullptr)
    {
        m_pInst = new AlgorithmRepository;
        assert(m_pInst != nullptr);
    }
    return m_pInst;
}

BinaryFunc* AlgorithmRepository::Lookup(BinaryNode::Operator oper)
{
    return m_pFunctionTable[oper];
}

std::optional<Value> BinaryFunc::GetRvalue(Node* pNode, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols)
{
    ValueNode* pValueNode = dynamic_cast<ValueNode*>(pNode);
    if (pValueNode != nullptr)
    {
        return pValueNode->GetValue();
    }

    VarNode* pVarNode = dynamic_cast<VarNode*>(pNode);
    if (pVarNode != nullptr)
    {
        std::string name = pVarNode->GetName();
        std::vector<int> arraySpecifier = pVarNode->GetArraySpecifier();
        if (pLocalSymbols != nullptr)
        {
            if (pLocalSymbols->IsSymbolPresent(name))
            {
                return pLocalSymbols->GetSymbolValue(name, arraySpecifier.size() != 0 ? &arraySpecifier : nullptr);
            }

            return pGlobalSymbols->GetSymbolValue(name, arraySpecifier.size() != 0 ? &arraySpecifier : nullptr);
        }
        else
        {
            return pGlobalSymbols->GetSymbolValue(name, arraySpecifier.size() != 0 ? &arraySpecifier : nullptr);
        }
    }

    return {};
}

std::optional<VarNode*> BinaryFunc::GetLvalue(Node* pNode)
{
    VarNode* pVarNode = dynamic_cast<VarNode*>(pNode);
    if (pVarNode != nullptr)
    {
        return pVarNode;
    }
    return {};
}

std::optional<BinaryFunc::RvalueProperties> BinaryFunc::GetRvalueProperties(Node* pNode, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, 
                                                                            ErrorInterface* pErrorInterface)
{
    RvalueProperties r;

    // Literals processing
    ValueNode* pValueNode = dynamic_cast<ValueNode*>(pNode);
    if (pValueNode != nullptr)
    {
        if (pValueNode->IsArray())
        {
            r.m_ArraySel = true;
            r.m_ArrayValue = pValueNode->GetArrayValue();
            return r;
        }

        r.m_ArraySel = false;
        r.m_Value = pValueNode->GetValue();
        return r;
    }


    // Symbols processing
    VarNode* pVarNode = dynamic_cast<VarNode*>(pNode);
    assert(pVarNode != nullptr);

    std::string name = pVarNode->GetName();
    r.m_Name = name;
    std::vector<int> arraySpecifier = pVarNode->GetArraySpecifier();

    // Lookup the symbol name in the symbol table.
    SymbolTable* pSymbolTable = pGlobalSymbols;
    if (pLocalSymbols != nullptr)
    {
        if (pLocalSymbols->IsSymbolPresent(name))
        {
            pSymbolTable = pLocalSymbols;
        } 
    }

    if (pSymbolTable->IsSymbolPresent(name))
    {
        if (arraySpecifier.size() != 0)
        {
            r.m_ArraySel = false;
            std::optional<Value> v = pSymbolTable->GetSymbolValue(name, &arraySpecifier);
            if (v == std::nullopt)
            {
                ErrorInterface::ErrorInfo err(pNode);
                char buf[512];
                if (pSymbolTable->IsSymbolArray(name))
                {
                    sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_INCORRECT_ARRAY_SPECIFIER, name.c_str());
                }
                else
                {
                    sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_UNEXPECTED_ARRAY_SPECIFIER, name.c_str());
                }
                err.m_Msg = buf;
                pErrorInterface->SetErrorFlag(true);
                pErrorInterface->SetErrorInfo(err);
                return {};
            }

            r.m_Value = v.value();
            return r;
        }

        r.m_ArraySel = pSymbolTable->IsSymbolArray(name);
        if (r.m_ArraySel)
        {
            std::optional<ArrayValue> arrayValue = pSymbolTable->GetArraySymbolValue(name);
            assert(arrayValue != std::nullopt);
            r.m_ArrayValue = arrayValue.value();
            return r;
        }

        std::optional<Value> v = pSymbolTable->GetSymbolValue(name);
        assert(v != std::nullopt);
        r.m_Value = v.value();
        return r;
    }

    // Symbol is not present in the symbol table.
    ErrorInterface::ErrorInfo err(pNode);
    char buf[512];
    sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_MISSING_SYMBOL, name.c_str());
    err.m_Msg = buf;
    pErrorInterface->SetErrorFlag(true);
    pErrorInterface->SetErrorInfo(err);
    return {};
}

std::optional<BinaryFunc::LvalueProperties> BinaryFunc::GetLvalueProperties(Node* pNode, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols,
                                                                            ErrorInterface* pErrorInterface)
{
    LvalueProperties l;

    VarNode* pVarNode = dynamic_cast<VarNode*>(pNode);
    if (pVarNode != nullptr)
    {
        l.m_SymbolName = pVarNode->GetName();
        l.m_pSymbolTable = pGlobalSymbols;
        if (pLocalSymbols != nullptr)
        {
            if (pLocalSymbols->IsSymbolPresent(l.m_SymbolName))
            {
                l.m_SymbolPresent = true;
                l.m_pSymbolTable = pLocalSymbols;
            }
        }

        if (!l.m_SymbolPresent)
        {
            l.m_SymbolPresent = l.m_pSymbolTable->IsSymbolPresent(l.m_SymbolName);
        }

        l.m_SymbolArraySpecifier = pVarNode->GetArraySpecifier();
        l.m_SymbolArray = l.m_pSymbolTable->IsSymbolArray(l.m_SymbolName);

        // a[0] only allowed if we have the symbol.
        // The symbol must be an array.
        if (l.m_SymbolArraySpecifier.size() != 0)
        {
            if (!l.m_SymbolPresent)
            {
                ErrorInterface::ErrorInfo err(pNode);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_MISSING_SYMBOL, l.m_SymbolName.c_str());
                err.m_Msg = buf;
                pErrorInterface->SetErrorFlag(true);
                pErrorInterface->SetErrorInfo(err);
                return {};
            }

            if (!l.m_pSymbolTable->IsSymbolArray(l.m_SymbolName))
            {
                ErrorInterface::ErrorInfo err(pNode);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_UNEXPECTED_ARRAY_SPECIFIER, l.m_SymbolName.c_str());
                err.m_Msg = buf;
                pErrorInterface->SetErrorFlag(true);
                pErrorInterface->SetErrorInfo(err);
                return {};
            }
        }

        // a = 3
        // a[0] = 4
        if (!l.m_SymbolArray && l.m_SymbolArraySpecifier.size() != 0)
        {
            // The symbol is not an array, but we have an array specifier.
            ErrorInterface::ErrorInfo err(pNode);
            char buf[512];
            sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_UNEXPECTED_ARRAY_SPECIFIER, l.m_SymbolName.c_str());
            err.m_Msg = buf;
            pErrorInterface->SetErrorFlag(true);
            pErrorInterface->SetErrorInfo(err);
            return {};
        }

        return l;
    }

    return {};
}

// a = 1 ; legal
// 
// a = b ; legal when b defined, illegal otherwise.
// 
// a = b[0] ; legal when b is an array, illegal otherwise.
// 
// a = dim[10]
// a[0] = 1 ; legal when a is defined.
// 
// a[0] = b ; legal when b is defined and a is defined.
// a[0] = b[0] ; legal when a and b are defined and arrays.
class EquFunc : public BinaryFunc
{
    virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
    {
        // Figure it if we're operating with arrays or not.
        // Either both have to represent arrays or not.
        std::optional<RvalueProperties> rval = GetRvalueProperties(pRight, pGlobalSymbols, pLocalSymbols, pErrorInterface);
        if (rval == std::nullopt)
        {
            return nullptr;
        }

        std::optional<LvalueProperties> lval = GetLvalueProperties(pLeft, pGlobalSymbols, pLocalSymbols, pErrorInterface);
        if (lval == std::nullopt)
        {
            return nullptr;
        }

        if (IsArray(*rval))
        {
            // The right side represents an entire array.
            // If there is an array specifier on the left side, then we're done.
            // a=dim[10]
            // b=dim[10]
            // a[0] = b, not allowed
            if (lval->m_SymbolArraySpecifier.size() != 0)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_UNEXPECTED_ARRAY_SPECIFIER, lval->m_SymbolName.c_str());
                err.m_Msg = buf;
                pErrorInterface->SetErrorFlag(true);
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            // Copy the entire array from the right to the left.
            // Remove the symbol whether its there or not.
            lval->m_pSymbolTable->DelSymbol(lval->m_SymbolName);

            // Add another array symbol.
            lval->m_pSymbolTable->AddSymbol(lval->m_SymbolName, rval->m_ArrayValue);
            return nullptr;
        }

        // Right side is not array but a single element.
        if (IsArray(*lval))
        {
            // Left side is an entire array.
            // a=dim[10]
            // a = 3
            // Error
            ErrorInterface::ErrorInfo err(pLeft);
            char buf[512];
            sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_UNEXPECTED_ARRAY_SPECIFIER, lval->m_SymbolName.c_str());
            err.m_Msg = buf;
            pErrorInterface->SetErrorFlag(true);
            pErrorInterface->SetErrorInfo(err);
            return nullptr;
        }

        // Left side is not an array but a single element.
        // a
        // a[0]
        if (lval->m_SymbolArraySpecifier.size() != 0)
        {
            // a[0]
            bool status = lval->m_pSymbolTable->ChangeSymbol(lval->m_SymbolName, rval->m_Value,
                &lval->m_SymbolArraySpecifier);
            if (!status)
            {
                // Failed to update, array specifier could be bad.
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_INCORRECT_ARRAY_SPECIFIER, lval->m_SymbolName.c_str());
                err.m_Msg = buf;
                pErrorInterface->SetErrorFlag(true);
                pErrorInterface->SetErrorInfo(err);
            }

            return nullptr;
        }

        // Left side is not an array, but a variable without an array specifier.
        // a = 3
        lval->m_pSymbolTable->DelSymbol(lval->m_SymbolName);
        bool status = lval->m_pSymbolTable->AddSymbol(lval->m_SymbolName, rval->m_Value);
        assert(status);
        return nullptr;
    }
};

class AddFunc : public BinaryFunc
{
#if 0
    virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
    {
        std::optional<Value> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols);
        if (lhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        std::optional<Value> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols);
        if (rhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        bool status = lhs.value().Add(rhs.value());
        if (!status)
        {
            Log::GetInst()->AddMessage("Could not add 2 values.");
            return nullptr;
        }

        return new ValueNode(lhs.value());
    }
#endif
    virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
    {
        std::optional<RvalueProperties> lhs = GetRvalueProperties(pLeft, pGlobalSymbols, pLocalSymbols, pErrorInterface);
        if (lhs == std::nullopt)
        {
            return nullptr;
        }

        std::optional<RvalueProperties> rhs = GetRvalueProperties(pRight, pGlobalSymbols, pLocalSymbols, pErrorInterface);
        if (rhs == std::nullopt)
        {
            return nullptr;
        }

        // If one side is an array, the other must be as well.
        if (lhs->m_ArraySel)
        {
            if (rhs->m_ArraySel)
            {
                // The dimensions must match.
                bool status = lhs->m_ArrayValue.Add(rhs->m_ArrayValue);
                if (!status)
                {
                    ErrorInterface::ErrorInfo err(pLeft);
                    char buf[512];
                    sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_ARRAY_OPERATION_FAILED, lhs->m_Name.c_str());
                    err.m_Msg = buf;
                    pErrorInterface->SetErrorFlag(true);
                    pErrorInterface->SetErrorInfo(err);
                    return nullptr;
                }

                ValueNode* pValueNode = new ValueNode(lhs->m_ArrayValue);
                assert(pValueNode != nullptr);
                return pValueNode;
            }

            ErrorInterface::ErrorInfo err(pRight);
            char buf[512];
            sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_ARRAY_OPERATION_FAILED, rhs->m_Name.c_str());
            err.m_Msg = buf;
            pErrorInterface->SetErrorFlag(true);
            pErrorInterface->SetErrorInfo(err);
            return nullptr;
        }

        if (rhs->m_ArraySel)
        {
            // lhs is not an array, error.
            ErrorInterface::ErrorInfo err(pLeft);
            char buf[512];
            sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_ENTIRE_ARRAY_EXPECTED, lhs->m_Name.c_str());
            err.m_Msg = buf;
            pErrorInterface->SetErrorFlag(true);
            pErrorInterface->SetErrorInfo(err);
            return nullptr;
        }

        bool status = lhs->m_Value.Add(rhs->m_Value);
        if (!status)
        {
            ErrorInterface::ErrorInfo err(pLeft);
            err.m_Msg = pErrorInterface->ERROR_BINARY_OPERATION_FAILED;
            pErrorInterface->SetErrorFlag(true);
            pErrorInterface->SetErrorInfo(err);
            return nullptr;
        }

        return new ValueNode(lhs->m_Value);
    }

};

class SubFunc : public BinaryFunc
{
    virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
    {
        std::optional<Value> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols);
        if (lhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        std::optional<Value> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols);
        if (rhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        bool status = lhs.value().Sub(rhs.value());
        if (!status)
        {
            Log::GetInst()->AddMessage("Could not subtract 2 values.");
            return nullptr;
        }

        return new ValueNode(lhs.value());
    }
};

class MulFunc : public BinaryFunc
{
    virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
    {
        std::optional<Value> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols);
        if (lhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        std::optional<Value> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols);
        if (rhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        bool status = lhs.value().Mul(rhs.value());
        if (!status)
        {
            Log::GetInst()->AddMessage("Could not multiply 2 values.");
            return nullptr;
        }

        return new ValueNode(lhs.value());
    }
};

class DivFunc : public BinaryFunc
{
    virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
    {
        std::optional<Value> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols);
        if (lhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        std::optional<Value> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols);
        if (rhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        bool status = lhs.value().Div(rhs.value());
        if (!status)
        {
            Log::GetInst()->AddMessage("Could not divide 2 values.");
            return nullptr;
        }

        return new ValueNode(lhs.value());
    }
};

class LesFunc : public BinaryFunc
{
    virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
    {
        std::optional<Value> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols);
        if (lhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        std::optional<Value> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols);
        if (rhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        bool status = lhs.value().Les(rhs.value());
        Value v;
        v.SetIntValue(status);

        return new ValueNode(v);
    }
};

class LeqFunc : public BinaryFunc
{
    virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
    {
        std::optional<Value> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols);
        if (lhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        std::optional<Value> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols);
        if (rhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        bool status = lhs.value().Leq(rhs.value());
        Value v;
        v.SetIntValue(status);

        return new ValueNode(v);
    }
};


class GrtFunc : public BinaryFunc
{
    virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
    {
        std::optional<Value> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols);
        if (lhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        std::optional<Value> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols);
        if (rhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        bool status = lhs.value().Grt(rhs.value());
        Value v;
        v.SetIntValue(status);
        return new ValueNode(v);
    }
};

class GeqFunc : public BinaryFunc
{
    virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
    {
        std::optional<Value> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols);
        if (lhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        std::optional<Value> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols);
        if (rhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        bool status = lhs.value().Geq(rhs.value());
        Value v;
        v.SetIntValue(status);
        return new ValueNode(v);
    }
};

class DeqFunc : public BinaryFunc
{
    virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
    {
        std::optional<Value> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols);
        if (lhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        std::optional<Value> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols);
        if (rhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        bool status = lhs.value().Deq(rhs.value());
        Value v;
        v.SetIntValue(status);
        return new ValueNode(v);
    }
};


class NeqFunc : public BinaryFunc
{
    virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
    {
        std::optional<Value> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols);
        if (lhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        std::optional<Value> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols);
        if (rhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        bool status = lhs.value().Neq(rhs.value());
        Value v;
        v.SetIntValue(status);
        return new ValueNode(v);
    }
};

class NegFunc : public BinaryFunc
{
    virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
    {
        assert(pRight == nullptr);
        std::optional<Value> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols);
        if (lhs == std::nullopt)
        {
            Log::GetInst()->AddMessage("Could not acquire rvalue.");
            return nullptr;
        }

        lhs.value().Neg();
        return new ValueNode(lhs.value());
    }
};

class DimFunc : public BinaryFunc
{
    virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
    {
        DimNode* pDimNode = dynamic_cast<DimNode*>(pRight);
        assert(pDimNode != nullptr);

        // Make the dimensions
        std::vector<int> dims;
        for (Node* pValNode = pDimNode->GetDim(); pValNode != nullptr; pValNode = pValNode->GetNext())
        {
            std::optional<Value> v = GetRvalue(pValNode, pGlobalSymbols, pLocalSymbols);
            if (v.has_value())
            {
                dims.push_back(v.value().GetIntValue());
            }
        }

        ArrayValue arrayVal;
        arrayVal.SetDims(dims);
        
        std::optional<VarNode*> lval = GetLvalue(pLeft);
        if (lval == std::nullopt)
        {
            Log::GetInst()->AddMessage("Left hand of equation was not an lvalue.");
            return nullptr;
        }

        std::string name = lval.value()->GetName();
        std::vector<int> arraySpecifier = lval.value()->GetArraySpecifier();

        // Add more logic to figure out where to place symbol.
        SymbolTable* pSymbolTable = nullptr;
        if (pLocalSymbols != nullptr)
        {
            bool status = pLocalSymbols->IsSymbolPresent(name);
            if (status)
            {
                // Found in the local symbol table use it.
                pSymbolTable = pLocalSymbols;
            }
            else
            {
                // Always use the global symbol table.
                pSymbolTable = pGlobalSymbols;
            }
        }
        else
        {
            // No local symbol table.
            pSymbolTable = pGlobalSymbols;
        }

        if (pSymbolTable->IsSymbolPresent(name))
        {
            pSymbolTable->DelSymbol(name);
        }

        bool status = pSymbolTable->AddSymbol(name, arrayVal);
        assert(status);

        // No Result
        return nullptr;
    }

};

class AryFunc : public BinaryFunc
{
    virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
    {
        DimNode* pDimNode = dynamic_cast<DimNode*>(pRight);
        assert(pDimNode != nullptr);

        // Make the dimensions
        std::vector<int> dims;
        for (Node* pValNode = pDimNode->GetDim(); pValNode != nullptr; pValNode = pValNode->GetNext())
        {
            std::optional<Value> v = GetRvalue(pValNode, pGlobalSymbols, pLocalSymbols);
            if (v.has_value())
            {
                dims.push_back(v.value().GetIntValue());
            }
        }

        std::optional<VarNode*> lval = GetLvalue(pLeft);
        if (lval == std::nullopt)
        {
            Log::GetInst()->AddMessage("Left hand of equation was not an lvalue.");
            return nullptr;
        }

        // Create a new VarNode with the same name, and the array specifier.
        VarNode* pNode = dynamic_cast<VarNode*>(lval.value()->Clone());
        pNode->SetArraySpecifier(dims);
        return pNode;
    }

};

BinaryFunc* AlgorithmRepository::m_pFunctionTable[] =
{
    nullptr,
    new EquFunc,
    new AddFunc,
    new SubFunc,
    new MulFunc,
    new DivFunc,
    new LesFunc,
    new LeqFunc,
    new GrtFunc,
    new GeqFunc,
    new DeqFunc,
    new NeqFunc,
    new NegFunc,
    new DimFunc,
    new AryFunc
};

AlgorithmRepository::AlgorithmRepository()
{
}

AlgorithmRepository::~AlgorithmRepository()
{
}

};
