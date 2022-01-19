#include <assert.h>
#include <optional>
#include "InterpreterAlgorithmRepository.h"
#include "InterpreterSymbolTable.hpp"
#include "InterpreterLog.h"
#include "InterpreterContext.h"
#include "InterpreterErrorInterface.h"
#include "InterpreterLvalues.h"

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

    std::optional<Rvalue> BinaryFunc::GetRvalue(Node* pNode,
                                                SymbolTable* pGlobalSymbols, 
                                                SymbolTable* pLocalSymbols, 
                                                ErrorInterface* pErrorInterface)
    {
        // Literals processing
        ValueNode* pValueNode = dynamic_cast<ValueNode*>(pNode);
        if (pValueNode != nullptr)
        {
            Rvalue r(pValueNode);
            return r;
        }

        // Symbols processing
        VarNode* pVarNode = dynamic_cast<VarNode*>(pNode);
        assert(pVarNode != nullptr);

        std::string name = pVarNode->GetName();
        std::vector<int> arraySpecifier = pVarNode->GetArraySpecifier();

        if (pVarNode->GetSymbolTable() != nullptr)
        {
            std::optional<SymbolTable::SymbolInfo> info = pVarNode->GetSymbolTable()->ReadSymbol(name);
            assert(info != std::nullopt);
            if (info->m_IsArray)
            {
                if (arraySpecifier.size() != 0)
                {
                    std::optional<Value> v = info->m_ArrayValue.GetValue(arraySpecifier);
                    if (v == std::nullopt)
                    {
                        ErrorInterface::ErrorInfo err(pNode);
                        char buf[512];
                        sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_INCORRECT_ARRAY_SPECIFIER, name.c_str());
                        err.m_Msg = buf;
                        pErrorInterface->SetErrorFlag(true);
                        pErrorInterface->SetErrorInfo(err);
                        return {};
                    }

                    Rvalue r(*v);
                    return r;
                }

                Rvalue r(info->m_ArrayValue);
                return r;
            }

            if (arraySpecifier.size() != 0)
            {
                ErrorInterface::ErrorInfo err(pNode);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_INCORRECT_ARRAY_SPECIFIER, name.c_str());
                err.m_Msg = buf;
                pErrorInterface->SetErrorFlag(true);
                pErrorInterface->SetErrorInfo(err);
                return {};
            }

            Rvalue r(info->m_Value);
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

    Lvalue* BinaryFunc::GetLvalue(Node* pNode, 
                                  SymbolTable* pGlobalSymbols, 
                                  SymbolTable* pLocalSymbols,
                                  Rvalue& rRvalue,
                                  ErrorInterface* pErrorInterface)
    {
        ErrorInterface::ErrorInfo errInfo(pNode);
        VarNode* pVarNode = dynamic_cast<VarNode*>(pNode);

        if (pVarNode != nullptr)
        {
            std::string name = pVarNode->GetName();
            std::vector<int> arraySpecifier = pVarNode->GetArraySpecifier();
            SymbolTable* pSymbols = pVarNode->GetSymbolTable();

            if (pSymbols == nullptr)
            {
                return new NoSymbolLvalue(name, pLocalSymbols != nullptr ? pLocalSymbols : pGlobalSymbols,
                                          pErrorInterface, errInfo);
            }

            std::optional<SymbolTable::SymbolInfo> symbolInfo = pSymbols->ReadSymbol(name);

            // Symbol not found
            if (symbolInfo == std::nullopt)
            {
                return new NoSymbolLvalue(name, pSymbols,
                                          pErrorInterface, errInfo);
            }

            // Ref checking
            if (symbolInfo->m_IsRef)
            {
                if (arraySpecifier.size() != 0)
                {
                    return new RefArraySpecifierLvalue(name, pSymbols, pErrorInterface, errInfo,
                                                       arraySpecifier);
                }

                return new RefLvalue(name, pSymbols, pErrorInterface, errInfo);
            }


            // Array element checking
            if (arraySpecifier.size() != 0)
            {
                return new ArrayElementLvalue(name, pSymbols, pErrorInterface, errInfo, arraySpecifier);
            }

            // Whole arrays
            if (rRvalue.IsArray())
            {
                return new WholeArrayLvalue(name, pSymbols, pErrorInterface, errInfo);
            }

            return new VariableLvalue(name, pSymbols, pErrorInterface, errInfo);
        }

        return nullptr;
    }

    class EquFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, 
                              ErrorInterface* pErrorInterface)
        {
            // Check for variable lists.
            VarListNode* pLeftVarList = dynamic_cast<VarListNode*>(pLeft);
            VarListNode* pRightVarList = dynamic_cast<VarListNode*>(pRight);
            if (pLeftVarList == nullptr && pRightVarList != nullptr || 
                pLeftVarList != nullptr && pRightVarList == nullptr)
            {
                pErrorInterface->SetErrorFlag(true);
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_VARLIST_BOTH_SIDES);
                err.m_Msg = buf;
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            if (pLeftVarList != nullptr)
            {
                ProcessVarLists(pLeftVarList, pRightVarList, pGlobalSymbols, pLocalSymbols, pErrorInterface);
                return nullptr;
            }

            // Figure it if we're operating with arrays or not.
            // Either both have to represent arrays or not.
            std::optional<Rvalue> rval = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (rval == std::nullopt)
            {
                return nullptr;
            }

            Lvalue* pLval = GetLvalue(pLeft, pGlobalSymbols, pLocalSymbols, *rval, pErrorInterface);
            if (pLval == nullptr)
            {
                return nullptr;
            }

            pLval->Equ(*rval);
            return nullptr;
        }
    private:
        void ProcessVarLists(VarListNode* pLeft, VarListNode* pRight, SymbolTable* pGlobalSymbols, 
                             SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
        {
            // Argument count check
            if (pLeft->GetListCount() != pRight->GetListCount())
            {
                pErrorInterface->SetErrorFlag(true);
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_VARLIST_ARGS, pLeft->GetListCount(), pRight->GetListCount());
                err.m_Msg = buf;
                pErrorInterface->SetErrorInfo(err);
                return;
            }

            Node* pLeftNodes = pLeft->GetList();
            Node* pRightNodes = pRight->GetList();
            while (pLeftNodes != nullptr && pRightNodes != nullptr)
            {
                // Perform the copies one by one.
                Perform(pLeftNodes, pRightNodes, pGlobalSymbols, pLocalSymbols, pErrorInterface);
                if (pErrorInterface->IsErrorFlagSet())
                {
                    // We're done, let's stop.
                    return;
                }

                // Advance pointers
                pLeftNodes = pLeftNodes->GetNext();
                pRightNodes = pRightNodes->GetNext();
            }
        }
    };

    class AddFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
        {
            std::optional<Rvalue> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (lhs == std::nullopt)
            {
                return nullptr;
            }

            std::optional<Rvalue> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (rhs == std::nullopt)
            {
                return nullptr;
            }

            // If one side is an array, the other must be as well.
            if (lhs->IsArray())
            {
                if (rhs->IsArray())
                {
                    // The dimensions must match.
                    bool status = lhs->GetArrayValue().Add(rhs->GetArrayValue());
                    if (!status)
                    {
                        ErrorInterface::ErrorInfo err(pLeft);
                        char buf[512];
                        sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_ARRAY_OPERATION_FAILED);
                        err.m_Msg = buf;
                        pErrorInterface->SetErrorFlag(true);
                        pErrorInterface->SetErrorInfo(err);
                        return nullptr;
                    }

                    ValueNode* pValueNode = new ValueNode(lhs->GetArrayValue());
                    assert(pValueNode != nullptr);
                    return pValueNode;
                }

                ErrorInterface::ErrorInfo err(pRight);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_ARRAY_OPERATION_FAILED);
                err.m_Msg = buf;
                pErrorInterface->SetErrorFlag(true);
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            if (rhs->IsArray())
            {
                // lhs is not an array, error.
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_ENTIRE_ARRAY_EXPECTED);
                err.m_Msg = buf;
                pErrorInterface->SetErrorFlag(true);
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            bool status = lhs->GetValue().Add(rhs->GetValue());
            if (!status)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                err.m_Msg = pErrorInterface->ERROR_BINARY_OPERATION_FAILED;
                pErrorInterface->SetErrorFlag(true);
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            return new ValueNode(lhs->GetValue());
        }

    };

    class SubFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, 
                              ErrorInterface* pErrorInterface)
        {
            std::optional<Rvalue> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (lhs == std::nullopt)
            {
                return nullptr;
            }

            std::optional<Rvalue> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (rhs == std::nullopt)
            {
                return nullptr;
            }

            bool status = lhs->GetValue().Sub(rhs->GetValue());
            if (!status)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_BINARY_OPERATION_FAILED);
                err.m_Msg = buf;
                pErrorInterface->SetErrorFlag(true);
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            return new ValueNode(lhs->GetValue());
        }
    };

    class MulFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, 
                              ErrorInterface* pErrorInterface)
        {
            std::optional<Rvalue> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (lhs == std::nullopt)
            {
                return nullptr;
            }

            std::optional<Rvalue> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (rhs == std::nullopt)
            {
                return nullptr;
            }

            bool status = lhs->GetValue().Mul(rhs->GetValue());
            if (!status)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_BINARY_OPERATION_FAILED);
                err.m_Msg = buf;
                pErrorInterface->SetErrorFlag(true);
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            return new ValueNode(lhs->GetValue());
        }
    };

    class DivFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
        {
            std::optional<Rvalue> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (lhs == std::nullopt)
            {
                return nullptr;
            }

            std::optional<Rvalue> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (rhs == std::nullopt)
            {
                return nullptr;
            }

            bool status = lhs->GetValue().Div(rhs->GetValue());
            if (!status)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_BINARY_OPERATION_FAILED);
                err.m_Msg = buf;
                pErrorInterface->SetErrorFlag(true);
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            return new ValueNode(lhs->GetValue());
        }
    };

    class LesFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
        {
            std::optional<Rvalue> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (lhs == std::nullopt)
            {
                return nullptr;
            }

            std::optional<Rvalue> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (rhs == std::nullopt)
            {
                return nullptr;
            }

            std::optional<bool> status = lhs->GetValue().Les(rhs->GetValue());
            if (status == std::nullopt)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_BINARY_OPERATION_FAILED);
                err.m_Msg = buf;
                pErrorInterface->SetErrorFlag(true);
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            Value v;
            v.SetIntValue(*status);
            return new ValueNode(v);
        }
    };

    class LeqFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
        {
            std::optional<Rvalue> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (lhs == std::nullopt)
            {
                return nullptr;
            }

            std::optional<Rvalue> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (rhs == std::nullopt)
            {
                return nullptr;
            }

            std::optional<bool> status = lhs->GetValue().Leq(rhs->GetValue());
            if (status == std::nullopt)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_BINARY_OPERATION_FAILED);
                err.m_Msg = buf;
                pErrorInterface->SetErrorFlag(true);
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            Value v;
            v.SetIntValue(*status);
            return new ValueNode(v);
        }
    };

    class GrtFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
        {
            std::optional<Rvalue> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (lhs == std::nullopt)
            {
                return nullptr;
            }

            std::optional<Rvalue> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (rhs == std::nullopt)
            {
                return nullptr;
            }

            std::optional<bool> status = lhs->GetValue().Grt(rhs->GetValue());
            if (status == std::nullopt)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_BINARY_OPERATION_FAILED);
                err.m_Msg = buf;
                pErrorInterface->SetErrorFlag(true);
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            Value v;
            v.SetIntValue(*status);
            return new ValueNode(v);
        }
    };

    class GeqFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
        {
            std::optional<Rvalue> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (lhs == std::nullopt)
            {
                return nullptr;
            }

            std::optional<Rvalue> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (rhs == std::nullopt)
            {
                return nullptr;
            }

            std::optional<bool> status = lhs->GetValue().Geq(rhs->GetValue());
            if (status == std::nullopt)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_BINARY_OPERATION_FAILED);
                err.m_Msg = buf;
                pErrorInterface->SetErrorFlag(true);
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            Value v;
            v.SetIntValue(*status);
            return new ValueNode(v);
        }
    };

    class DeqFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
        {
            std::optional<Rvalue> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (lhs == std::nullopt)
            {
                return nullptr;
            }

            std::optional<Rvalue> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (rhs == std::nullopt)
            {
                return nullptr;
            }

            std::optional<bool> status = lhs->GetValue().Deq(rhs->GetValue());
            if (status == std::nullopt)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_BINARY_OPERATION_FAILED);
                err.m_Msg = buf;
                pErrorInterface->SetErrorFlag(true);
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            Value v;
            v.SetIntValue(*status);
            return new ValueNode(v);
        }
    };


    class NeqFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
        {
            std::optional<Rvalue> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (lhs == std::nullopt)
            {
                return nullptr;
            }

            std::optional<Rvalue> rhs = GetRvalue(pRight, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (rhs == std::nullopt)
            {
                return nullptr;
            }

            std::optional<bool> status = lhs->GetValue().Neq(rhs->GetValue());
            if (status == std::nullopt)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_BINARY_OPERATION_FAILED);
                err.m_Msg = buf;
                pErrorInterface->SetErrorFlag(true);
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            Value v;
            v.SetIntValue(*status);
            return new ValueNode(v);
        }
    };

    class NegFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, ErrorInterface* pErrorInterface)
        {
            assert(pRight == nullptr);
            std::optional<Rvalue> lhs = GetRvalue(pLeft, pGlobalSymbols, pLocalSymbols, pErrorInterface);
            if (lhs == std::nullopt)
            {
                return nullptr;
            }

            bool status = lhs->GetValue().Neg();
            if (status == false)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_UNARY_OPERATION_FAILED);
                err.m_Msg = buf;
                pErrorInterface->SetErrorFlag(true);
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            return new ValueNode(lhs->GetValue());
        }
    };

    // Make a new symbol
    // a = dim[10]
    class DimFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols, 
                              ErrorInterface* pErrorInterface)
        {
            DimNode* pDimNode = dynamic_cast<DimNode*>(pRight);
            assert(pDimNode != nullptr);

            // Make the dimensions
            std::vector<int> dims;
            for (Node* pValNode = pDimNode->GetDim(); pValNode != nullptr; pValNode = pValNode->GetNext())
            {
                std::optional<Rvalue> rval = GetRvalue(pValNode, pGlobalSymbols, pLocalSymbols, pErrorInterface);
                if (rval == std::nullopt)
                {
                    return nullptr;
                }

                if (rval->IsArray())
                {
                    ErrorInterface::ErrorInfo err(pDimNode);
                    err.m_Msg = pErrorInterface->ERROR_ARRAY_UNEXPECTED;
                    pErrorInterface->SetErrorInfo(err);
                    pErrorInterface->SetErrorFlag(true);
                    return nullptr;
                }

                if (rval->GetValue().GetType() != typeid(int))
                {
                    ErrorInterface::ErrorInfo err(pDimNode);
                    err.m_Msg = pErrorInterface->ERROR_INCORRECT_TYPE;
                    pErrorInterface->SetErrorInfo(err);
                    pErrorInterface->SetErrorFlag(true);
                    return nullptr;
                }

                dims.push_back(rval->GetValue().GetIntValue());
            }

            ArrayValue arrayVal;
            arrayVal.SetDims(dims);
            Rvalue rValue(arrayVal);

            Lvalue* pLval = GetLvalue(pLeft, pGlobalSymbols, pLocalSymbols, rValue, pErrorInterface);
            assert(pLval != nullptr);
            pLval->Dim(rValue);

            // No Result
            return nullptr;
        }

    };
    class AryFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, SymbolTable* pGlobalSymbols, SymbolTable* pLocalSymbols,
            ErrorInterface* pErrorInterface)
        {
            DimNode* pDimNode = dynamic_cast<DimNode*>(pRight);
            assert(pDimNode != nullptr);

            // Make the dimensions
            std::vector<int> dims;
            for (Node* pValNode = pDimNode->GetDim(); pValNode != nullptr; pValNode = pValNode->GetNext())
            {
                std::optional<Rvalue> v = GetRvalue(pValNode, pGlobalSymbols, pLocalSymbols, pErrorInterface);
                if (v != std::nullopt)
                {
                    if (v->IsArray())
                    {
                        ErrorInterface::ErrorInfo err(pRight);
                        err.m_Msg = pErrorInterface->ERROR_INCORRECT_DIM;
                        pErrorInterface->SetErrorInfo(err);
                        pErrorInterface->SetErrorFlag(true);
                        return nullptr;
                    }

                    if (v->GetValue().GetType() != typeid(int))
                    {
                        ErrorInterface::ErrorInfo err(pRight);
                        err.m_Msg = pErrorInterface->ERROR_INCORRECT_DIM;
                        pErrorInterface->SetErrorInfo(err);
                        pErrorInterface->SetErrorFlag(true);
                        return nullptr;
                    }

                    dims.push_back(v->GetValue().GetIntValue());
                }
            }

            // The left node must be a VarNode based on the grammar.
            VarNode* pVarNode = dynamic_cast<VarNode*>(pLeft->Clone());
            assert(pVarNode != nullptr);
            pVarNode->SetArraySpecifier(dims);
            return pVarNode;
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
