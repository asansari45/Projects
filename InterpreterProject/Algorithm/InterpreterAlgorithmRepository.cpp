#include <assert.h>
#include <optional>
#include <memory>
#include "InterpreterAlgorithmRepository.h"
#include "Driver/InterpreterContext.h"
#include "Tables/InterpreterSymbolTable.h"
#include "Log/InterpreterLog.h"
#include "Nodes/InterpreterVarNode.h"
#include "Nodes/InterpreterValueNode.h"
#include "Nodes/InterpreterVarListNode.h"
#include "Nodes/InterpreterDimNode.h"
#include "Values/InterpreterLvalues.h"
#include "Visitors/InterpreterErrorInterface.h"
#include "Visitors/InterpreterExecutionNodeVisitorServices.h"
#include "DebugMemory/DebugMemory.h"

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

    void AlgorithmRepository::Shutdown()
    {
        for (int i = 0; i <= BinaryNode::LAND; i++)
        {
            delete m_pFunctionTable[i];
        }

        delete m_pInst;
    }

    BinaryFunc* AlgorithmRepository::Lookup(BinaryNode::Operator oper)
    {
        return m_pFunctionTable[oper];
    }

    ValueNode* BinaryFunc::GetRvalue(Node* pNode,
                                     ExecutionNodeVisitorServices* pServices,
                                     ErrorInterface* pErrorInterface)
    {
        // Literals processing
        ValueNode* pValueNode = dynamic_cast<ValueNode*>(pNode);
        if (pValueNode != nullptr)
        {
            return dynamic_cast<ValueNode*>(pValueNode->Clone());
        }

        // Symbols processing
        VarNode* pVarNode = dynamic_cast<VarNode*>(pNode);
        assert(pVarNode != nullptr);

        std::string name = pVarNode->GetName();
        std::vector<int> arraySpecifier = pVarNode->GetArraySpecifier();

        if (pServices->IsSymbolPresent(name))
        {
            SymbolTable::SymbolInfo* pInfo = pServices->ReadSymbol(name, true);
            assert(pInfo != nullptr);
            assert(!pInfo->m_IsRef);
            if (pInfo->m_IsArray)
            {
                if (arraySpecifier.size() != 0)
                {
                    std::optional<Value> v = pInfo->m_pArrayValue->GetValue(arraySpecifier);
                    if (v == std::nullopt)
                    {
                        ErrorInterface::ErrorInfo err(pNode);
                        char buf[512];
                        sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_INCORRECT_ARRAY_SPECIFIER, name.c_str());
                        err.m_Msg = buf;
                        
                        pErrorInterface->SetErrorInfo(err);
                        return nullptr;
                    }

                    return new ValueNode(*v);
                }

                return new ValueNode(pInfo->m_pArrayValue->Clone());
            }

            if (arraySpecifier.size() != 0)
            {
                ErrorInterface::ErrorInfo err(pNode);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_UNEXPECTED_ARRAY_SPECIFIER, name.c_str());
                err.m_Msg = buf;
                
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            return new ValueNode(pInfo->m_Value);
        }

        // Symbol is not present in the symbol table.
        ErrorInterface::ErrorInfo err(pNode);
        char buf[512];
        sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_MISSING_SYMBOL, name.c_str());
        err.m_Msg = buf;
        
        pErrorInterface->SetErrorInfo(err);
        return nullptr;
    }

    Lvalue* BinaryFunc::GetLvalue(Node* pNode, 
                                  ValueNode* pRvalue,
                                  ExecutionNodeVisitorServices* pServices,
                                  ErrorInterface* pErrorInterface)
    {
        ErrorInterface::ErrorInfo errInfo(pNode);
        VarNode* pVarNode = dynamic_cast<VarNode*>(pNode);

        if (pVarNode != nullptr)
        {
            std::string name = pVarNode->GetName();
            std::vector<int> arraySpecifier = pVarNode->GetArraySpecifier();
            if (!pServices->IsSymbolPresent(name))
            {
                return new NoSymbolLvalue(name,
                                          arraySpecifier,
                                          pServices->GetNoSymbolTable(),
                                          pErrorInterface, 
                                          errInfo);
            }

            SymbolTable::SymbolInfo* pSymbolInfo = pServices->ReadSymbol(name, true);
            assert(!pSymbolInfo->m_IsRef);

            // Array element checking
            if (arraySpecifier.size() != 0)
            {
                return new ArrayElementLvalue(pSymbolInfo->m_Name, pSymbolInfo->m_pTable, pErrorInterface, errInfo, arraySpecifier);
            }

            // Whole arrays
            if (pRvalue->IsArray())
            {
                return new WholeArrayLvalue(pSymbolInfo->m_Name, pSymbolInfo->m_pTable, pErrorInterface, errInfo);
            }

            return new VariableLvalue(pSymbolInfo->m_Name, pSymbolInfo->m_pTable, pErrorInterface, errInfo);
        }

        return nullptr;
    }

    class EquFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            // Check for variable lists.
            VarListNode* pLeftVarList = dynamic_cast<VarListNode*>(pLeft);
            VarListNode* pRightVarList = dynamic_cast<VarListNode*>(pRight);
            if (pLeftVarList != nullptr && pRightVarList != nullptr)
            {
                // {a,b} = {c,d}
                ProcessVarLists(pLeftVarList, pRightVarList, pServices, pErrorInterface);
                return nullptr;
            }

            if (pLeftVarList == nullptr && pRightVarList != nullptr)
            {
                // a ={0,1,3,3,4}
                ProcessInitializer(pLeft, pRightVarList, pServices, pErrorInterface);
                return nullptr;
            }

            if (pLeftVarList != nullptr && pRightVarList == nullptr)
            {
                // {a,b} = c
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_VARLIST_BOTH_SIDES);
                err.m_Msg = buf;
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }


            // Figure if we're operating with arrays or not.
            // Either both have to represent arrays or not.
            std::unique_ptr<ValueNode> pRval(GetRvalue(pRight, pServices, pErrorInterface));
            if (pRval == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<Lvalue> pLval(GetLvalue(pLeft, pRval.get(), pServices, pErrorInterface));
            if (pLval == nullptr)
            {
                return nullptr;
            }

            pLval->Equ(pRval.get());
            return nullptr;
        }
    private:

        // a = {0,1,2,3}
        void ProcessInitializer(Node* pLeft, VarListNode* pRight, 
                                ExecutionNodeVisitorServices* pServices, 
                                ErrorInterface* pErrorInterface)
        {
            ErrorInterface::ErrorInfo errInfo(pLeft);

            // We need to convert the VarList to a linked-list of value nodes.
            ValueNode* pHead = nullptr;
            ValueNode* pCurr = nullptr;
            int count = 0;
            for (Node* pNode = pRight->GetList(); pNode != nullptr; pNode=pNode->GetNext())
            {
                ValueNode* pValueNode = GetRvalue(pNode, pServices, pErrorInterface);
                if (pValueNode == nullptr)
                {
                    if (pHead != nullptr)
                    {
                        pHead->FreeList();
                        return;
                    }
                }

                if (pHead == nullptr)
                {
                    pHead = pValueNode;
                    pCurr = pHead;
                }
                else
                {
                    pCurr->SetNext(pValueNode);
                    pCurr = pValueNode;
                }
                count++;
            }

            std::unique_ptr<Lvalue> pLvalue;
            VarNode* pVarNode = dynamic_cast<VarNode*>(pLeft);
            assert(pVarNode != nullptr);

            SymbolTable::SymbolInfo* pInfo = pServices->ReadSymbol(pVarNode->GetName(), true);
            if (pInfo == nullptr)
            {
                pLvalue.reset(new NoSymbolLvalue(pVarNode->GetName(), 
                                                 pVarNode->GetArraySpecifier(),
                                                 pServices->GetNoSymbolTable(), 
                                                 pErrorInterface, errInfo));
            }
            else
            {
                if (pInfo->m_IsArray)
                {
                    if (pVarNode->GetArraySpecifier().size() != 0)
                    {
                        errInfo.m_Msg = pErrorInterface->ERROR_ARRAY_UNEXPECTED;
                        pErrorInterface->SetErrorInfo(errInfo);
                        pHead->FreeList();
                        return;
                    }

                    pLvalue.reset(new WholeArrayLvalue(pVarNode->GetName(), pInfo->m_pTable, pErrorInterface, 
                                                       errInfo));

                }
                else
                {
                    errInfo.m_Msg = pErrorInterface->ERROR_ENTIRE_ARRAY_EXPECTED;
                    pErrorInterface->SetErrorInfo(errInfo);
                    pHead->FreeList();
                    return;
                }
            }

            assert(pLvalue.get() != nullptr);
            pLvalue->EquList(count, pHead);
            pHead->FreeList();
        }

        // {a,b} = {b,c}
        void ProcessVarLists(VarListNode* pLeft, VarListNode* pRight, ExecutionNodeVisitorServices* pServices, ErrorInterface* pErrorInterface)
        {
            // Argument count check
            if (pLeft->GetListCount() != pRight->GetListCount())
            {
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
                Perform(pLeftNodes, pRightNodes, pServices, pErrorInterface);
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
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            std::unique_ptr<ValueNode> lhs(GetRvalue(pLeft, pServices, pErrorInterface));
            if (lhs == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<ValueNode> rhs(GetRvalue(pRight, pServices, pErrorInterface));
            if (rhs == nullptr)
            {
                return nullptr;
            }

            // If one side is an array, the other must be as well.
            if (lhs->IsArray())
            {
                if (rhs->IsArray())
                {
                    // The dimensions must match.
                    bool status = lhs->GetArrayValue()->Add(rhs->GetArrayValue());
                    if (!status)
                    {
                        ErrorInterface::ErrorInfo err(pLeft);
                        char buf[512];
                        sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_ARRAY_OPERATION_FAILED);
                        err.m_Msg = buf;
                        
                        pErrorInterface->SetErrorInfo(err);
                        return nullptr;
                    }

                    ValueNode* pValueNode = new ValueNode(lhs->GetArrayValue()->Clone());
                    assert(pValueNode != nullptr);
                    return pValueNode;
                }

                ErrorInterface::ErrorInfo err(pRight);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_ARRAY_OPERATION_FAILED);
                err.m_Msg = buf;
                
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
                
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }
            
            bool status = lhs->GetValueRef().Add(rhs->GetValue());
            if (!status)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                err.m_Msg = pErrorInterface->ERROR_BINARY_OPERATION_FAILED;
                
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            ValueNode* pRet = new ValueNode(lhs->GetValue());
            return pRet;
        }

    };

    class SubFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            std::unique_ptr<ValueNode> lhs(GetRvalue(pLeft, pServices, pErrorInterface));
            if (lhs == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<ValueNode> rhs(GetRvalue(pRight, pServices, pErrorInterface));
            if (rhs == nullptr)
            {
                return nullptr;
            }

            bool status = lhs->GetValueRef().Sub(rhs->GetValue());
            if (!status)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_BINARY_OPERATION_FAILED);
                err.m_Msg = buf;
                
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            return new ValueNode(lhs->GetValue());
        }
    };

    class MulFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            std::unique_ptr<ValueNode> lhs(GetRvalue(pLeft, pServices, pErrorInterface));
            if (lhs == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<ValueNode> rhs(GetRvalue(pRight, pServices, pErrorInterface));
            if (rhs == nullptr)
            {
                return nullptr;
            }

            bool status = lhs->GetValueRef().Mul(rhs->GetValue());
            if (!status)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_BINARY_OPERATION_FAILED);
                err.m_Msg = buf;
                
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            return new ValueNode(lhs->GetValue());
        }
    };

    class DivFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            std::unique_ptr<ValueNode> lhs(GetRvalue(pLeft, pServices, pErrorInterface));
            if (lhs == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<ValueNode> rhs(GetRvalue(pRight, pServices, pErrorInterface));
            if (rhs == nullptr)
            {
                return nullptr;
            }

            bool status = lhs->GetValueRef().Div(rhs->GetValue());
            if (!status)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_BINARY_OPERATION_FAILED);
                err.m_Msg = buf;
                
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            return new ValueNode(lhs->GetValue());
        }
    };

    class LesFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            std::unique_ptr<ValueNode> lhs(GetRvalue(pLeft, pServices, pErrorInterface));
            if (lhs == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<ValueNode> rhs(GetRvalue(pRight, pServices, pErrorInterface));
            if (rhs == nullptr)
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
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            std::unique_ptr<ValueNode> lhs(GetRvalue(pLeft, pServices, pErrorInterface));
            if (lhs == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<ValueNode> rhs(GetRvalue(pRight, pServices, pErrorInterface));
            if (rhs == nullptr)
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
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            std::unique_ptr<ValueNode> lhs(GetRvalue(pLeft, pServices, pErrorInterface));
            if (lhs == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<ValueNode> rhs(GetRvalue(pRight, pServices, pErrorInterface));
            if (rhs == nullptr)
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
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            std::unique_ptr<ValueNode> lhs(GetRvalue(pLeft, pServices, pErrorInterface));
            if (lhs == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<ValueNode> rhs(GetRvalue(pRight, pServices, pErrorInterface));
            if (rhs == nullptr)
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
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            std::unique_ptr<ValueNode> lhs(GetRvalue(pLeft, pServices, pErrorInterface));
            if (lhs == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<ValueNode> rhs(GetRvalue(pRight, pServices, pErrorInterface));
            if (rhs == nullptr)
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
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            std::unique_ptr<ValueNode> lhs(GetRvalue(pLeft, pServices, pErrorInterface));
            if (lhs == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<ValueNode> rhs(GetRvalue(pRight, pServices, pErrorInterface));
            if (rhs == nullptr)
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
                
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            Value v;
            v.SetIntValue(*status);
            return new ValueNode(v);
        }
    };

    class LorFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            std::unique_ptr<ValueNode> lhs(GetRvalue(pLeft, pServices, pErrorInterface));
            if (lhs == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<ValueNode> rhs(GetRvalue(pRight, pServices, pErrorInterface));
            if (rhs == nullptr)
            {
                return nullptr;
            }

            std::optional<bool> status = lhs->GetValue().LogicalOr(rhs->GetValue());
            if (status == std::nullopt)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_BINARY_OPERATION_FAILED);
                err.m_Msg = buf;
                
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            Value v;
            v.SetIntValue(*status);
            return new ValueNode(v);
        }
    };

    class LandFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            std::unique_ptr<ValueNode> lhs(GetRvalue(pLeft, pServices, pErrorInterface));
            if (lhs == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<ValueNode> rhs(GetRvalue(pRight, pServices, pErrorInterface));
            if (rhs == nullptr)
            {
                return nullptr;
            }

            std::optional<bool> status = lhs->GetValue().LogicalAnd(rhs->GetValue());
            if (status == std::nullopt)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_BINARY_OPERATION_FAILED);
                err.m_Msg = buf;
                
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
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            assert(pRight == nullptr);
            std::unique_ptr<ValueNode> lhs(GetRvalue(pLeft, pServices, pErrorInterface));
            if (lhs == nullptr)
            {
                return nullptr;
            }

            bool status = lhs->GetValueRef().Neg();
            if (status == false)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_UNARY_OPERATION_FAILED);
                err.m_Msg = buf;
                
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            return new ValueNode(lhs->GetValue());
        }
    };

    class BnegFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            assert(pRight == nullptr);
            std::unique_ptr<ValueNode> lhs(GetRvalue(pLeft, pServices, pErrorInterface));
            if (lhs == nullptr)
            {
                return nullptr;
            }

            bool status = lhs->GetValueRef().Bneg();
            if (status == false)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_UNARY_OPERATION_FAILED);
                err.m_Msg = buf;
                
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
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,  
                              ErrorInterface* pErrorInterface)
        {
            DimNode* pDimNode = dynamic_cast<DimNode*>(pRight);
            assert(pDimNode != nullptr);

            // Make the dimensions
            std::vector<int> dims;
            for (Node* pValNode = pDimNode->GetDim(); pValNode != nullptr; pValNode = pValNode->GetNext())
            {
                std::unique_ptr<ValueNode> rval(GetRvalue(pValNode, pServices, pErrorInterface));
                if (rval == nullptr)
                {
                    return nullptr;
                }

                if (rval->IsArray())
                {
                    ErrorInterface::ErrorInfo err(pDimNode);
                    err.m_Msg = pErrorInterface->ERROR_ARRAY_UNEXPECTED;
                    pErrorInterface->SetErrorInfo(err);
                    return nullptr;
                }

                if (rval->GetValue().GetType() != typeid(int))
                {
                    ErrorInterface::ErrorInfo err(pDimNode);
                    err.m_Msg = pErrorInterface->ERROR_INCORRECT_TYPE;
                    pErrorInterface->SetErrorInfo(err);
                    return nullptr;
                }

                dims.push_back(rval->GetValue().GetIntValue());
            }

            std::unique_ptr<ValueNode> pValueNode(new ValueNode);
            ArrayValue* pArrayValue = ArrayValue::Create(dims, typeid(int));
            pValueNode->SetArrayValue(pArrayValue);

            Lvalue* pLval = GetLvalue(pLeft, pValueNode.get(), pServices, pErrorInterface);
            assert(pLval != nullptr);
            pLval->Dim(pValueNode.get());
            delete pLval;

            // No Result
            return nullptr;
        }

    };

    // AryFunc is used for a[3,4,5]
    class AryFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight,
                              ExecutionNodeVisitorServices* pServices, 
                              ErrorInterface* pErrorInterface)
        {
            DimNode* pDimNode = dynamic_cast<DimNode*>(pRight);
            assert(pDimNode != nullptr);

            // Make the dimensions
            std::vector<int> dims;
            for (Node* pValNode = pDimNode->GetDim(); pValNode != nullptr; pValNode = pValNode->GetNext())
            {
                std::unique_ptr<ValueNode> v(GetRvalue(pValNode, pServices, pErrorInterface));
                if (v != nullptr)
                {
                    if (v->IsArray())
                    {
                        ErrorInterface::ErrorInfo err(pRight);
                        err.m_Msg = pErrorInterface->ERROR_INCORRECT_DIM;
                        pErrorInterface->SetErrorInfo(err);
                        
                        return nullptr;
                    }

                    if (v->GetValue().GetType() != typeid(int))
                    {
                        ErrorInterface::ErrorInfo err(pRight);
                        err.m_Msg = pErrorInterface->ERROR_INCORRECT_DIM;
                        pErrorInterface->SetErrorInfo(err);
                        
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

    class OrFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            std::unique_ptr<ValueNode> lhs(GetRvalue(pLeft, pServices, pErrorInterface));
            if (lhs == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<ValueNode> rhs(GetRvalue(pRight, pServices, pErrorInterface));
            if (rhs == nullptr)
            {
                return nullptr;
            }

            bool status = lhs->GetValueRef().Or(rhs->GetValue());
            if (status == false)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_UNARY_OPERATION_FAILED);
                err.m_Msg = buf;
                
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            return new ValueNode(lhs->GetValue());
        }
    };

    class AndFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            std::unique_ptr<ValueNode> lhs(GetRvalue(pLeft, pServices, pErrorInterface));
            if (lhs == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<ValueNode> rhs(GetRvalue(pRight, pServices, pErrorInterface));
            if (rhs == nullptr)
            {
                return nullptr;
            }

            bool status = lhs->GetValueRef().And(rhs->GetValue());
            if (status == false)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_UNARY_OPERATION_FAILED);
                err.m_Msg = buf;
                
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            return new ValueNode(lhs->GetValue());
        }
    };

    class XorFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            std::unique_ptr<ValueNode> lhs(GetRvalue(pLeft, pServices, pErrorInterface));
            if (lhs == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<ValueNode> rhs(GetRvalue(pRight, pServices, pErrorInterface));
            if (rhs == nullptr)
            {
                return nullptr;
            }

            bool status = lhs->GetValueRef().Xor(rhs->GetValue());
            if (status == false)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_UNARY_OPERATION_FAILED);
                err.m_Msg = buf;
                
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            return new ValueNode(lhs->GetValue());
        }
    };
 
    class LshFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            std::unique_ptr<ValueNode> lhs(GetRvalue(pLeft, pServices, pErrorInterface));
            if (lhs == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<ValueNode> rhs(GetRvalue(pRight, pServices, pErrorInterface));
            if (rhs == nullptr)
            {
                return nullptr;
            }

            bool status = lhs->GetValueRef().Lsh(rhs->GetValue());
            if (status == false)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_UNARY_OPERATION_FAILED);
                err.m_Msg = buf;
                
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            return new ValueNode(lhs->GetValue());
        }
    };
    
    class RshFunc : public BinaryFunc
    {
        virtual Node* Perform(Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
        {
            std::unique_ptr<ValueNode> lhs(GetRvalue(pLeft, pServices, pErrorInterface));
            if (lhs == nullptr)
            {
                return nullptr;
            }

            std::unique_ptr<ValueNode> rhs(GetRvalue(pRight, pServices, pErrorInterface));
            if (rhs == nullptr)
            {
                return nullptr;
            }

            bool status = lhs->GetValueRef().Rsh(rhs->GetValue());
            if (status == false)
            {
                ErrorInterface::ErrorInfo err(pLeft);
                char buf[512];
                sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_UNARY_OPERATION_FAILED);
                err.m_Msg = buf;
                
                pErrorInterface->SetErrorInfo(err);
                return nullptr;
            }

            return new ValueNode(lhs->GetValue());
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
        new AryFunc,
        new OrFunc,
        new AndFunc,
        new XorFunc,
        new LshFunc,
        new RshFunc,
        new BnegFunc,
        new LorFunc,
        new LandFunc
    };

    AlgorithmRepository::AlgorithmRepository()
    {
    }

    AlgorithmRepository::~AlgorithmRepository()
    {
    }
};
