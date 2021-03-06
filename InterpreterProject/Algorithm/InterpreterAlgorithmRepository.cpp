#include <assert.h>
#include <optional>
#include <memory>
#include "InterpreterAlgorithmRepository.h"
#include "InterpreterArithmeticFunc.h"
#include "InterpreterLogicalFunc.h"
#include "InterpreterBitwiseFunc.h"
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
        for (int i = 0; i <= BinaryNode::BNEG; i++)
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

            // Not array, check for string.  If string, then return an index.
            if (pInfo->m_Value.GetType() == typeid(std::string))
            {
                std::string s = pInfo->m_Value.GetValue<std::string>();
                if (arraySpecifier.size() == 1)
                {
                    int index = arraySpecifier[0];
                    if (index < s.size())
                    {
                        return new ValueNode(static_cast<char>(s[index]));
                    }

                    // index too large
                    ErrorInterface::ErrorInfo err(pNode);
                    char buf[512];
                    sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_INCORRECT_ARRAY_SPECIFIER, s.c_str());
                    err.m_Msg = buf;
                    pErrorInterface->SetErrorInfo(err);
                    return nullptr;
                }

                if (arraySpecifier.size() != 0)
                {
                    ErrorInterface::ErrorInfo err(pNode);
                    char buf[512];
                    sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_INCORRECT_ARRAY_SPECIFIER, s.c_str());
                    err.m_Msg = buf;
                    pErrorInterface->SetErrorInfo(err);
                    return nullptr;
                }
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
        virtual Node* Perform(BinaryNode::Operator oper,
                              Node* pLeft, Node* pRight, 
                              ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
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
            bool listTypeSet = false;
            std::type_index listType(typeid(int));
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

                // Calculate the overall list type.  It will be string if all entries are string.
                // It will be ints if all entries are ints.  It will be floats if all entries are either
                // float or ints.
                std::type_index valueType(typeid(int));
                if (pValueNode->IsArray())
                {
                    errInfo.m_Msg = pErrorInterface->ERROR_ARRAY_UNEXPECTED;
                    pErrorInterface->SetErrorInfo(errInfo);
                    pHead->FreeList();
                    return;
                }

                valueType = pValueNode->GetValueRef().GetType();

                if (listTypeSet == false)
                {
                    listType = valueType;
                    listTypeSet = true;
                }
                else
                {
                    if (listType == typeid(std::string))
                    {
                        // We have strings so far.  It must be a string.
                        if (valueType != typeid(std::string))
                        {
                            errInfo.m_Msg = pErrorInterface->ERROR_INCORRECT_TYPE;
                            pErrorInterface->SetErrorInfo(errInfo);
                            pHead->FreeList();
                            return;
                        }
                    }
                    else if (listType == typeid(int))
                    {
                        if (valueType == typeid(float))
                        {
                            listType = typeid(float);
                        }
                        else if (valueType == typeid(std::string))
                        {
                            errInfo.m_Msg = pErrorInterface->ERROR_INCORRECT_TYPE;
                            pErrorInterface->SetErrorInfo(errInfo);
                            pHead->FreeList();
                            return;
                        }
                    }
                    else if (listType == typeid(float))
                    {
                        if (valueType == typeid(std::string))
                        {
                            errInfo.m_Msg = pErrorInterface->ERROR_INCORRECT_TYPE;
                            pErrorInterface->SetErrorInfo(errInfo);
                            pHead->FreeList();
                            return;
                        }
                    }
                }
            }

            // Let's store all the values in a single dimension array value.
            std::vector<int> dims;
            dims.push_back(count);
            std::unique_ptr<ArrayValue> pArrayValue(ArrayValue::Create(dims, listType));
            count = 0;
            for (ValueNode* pNode = pHead; pNode != nullptr; pNode = dynamic_cast<ValueNode*>(pNode->GetNext()))
            {
                dims.clear();
                dims.push_back(count);
                count++;
                bool status = pArrayValue->SetValue(dims, pNode->GetValueRef());
                assert(status);
            }
            pHead->FreeList();

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
                        return;
                    }

                    pLvalue.reset(new WholeArrayLvalue(pVarNode->GetName(), pInfo->m_pTable, pErrorInterface, 
                                                       errInfo));

                }
                else
                {
                    errInfo.m_Msg = pErrorInterface->ERROR_ENTIRE_ARRAY_EXPECTED;
                    pErrorInterface->SetErrorInfo(errInfo);
                    return;
                }
            }

            assert(pLvalue.get() != nullptr);
            pLvalue->EquList(pArrayValue.get());
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
                Perform(BinaryNode::EQU, pLeftNodes, pRightNodes, pServices, pErrorInterface);
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

    class NegFunc : public BinaryFunc
    {
        virtual Node* Perform(BinaryNode::Operator oper,
                              Node* pLeft, Node* pRight, 
                              ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
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
        virtual Node* Perform(BinaryNode::Operator oper,
                              Node* pLeft, Node* pRight, 
                              ExecutionNodeVisitorServices* pServices,ErrorInterface* pErrorInterface)
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
        virtual Node* Perform(BinaryNode::Operator oper,
                              Node* pLeft, Node* pRight, ExecutionNodeVisitorServices* pServices,  
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

                dims.push_back(rval->GetValue().GetValue<int>());
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
        virtual Node* Perform(BinaryNode::Operator oper,
                              Node* pLeft, Node* pRight,
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

                    dims.push_back(v->GetValue().GetValue<int>());
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
        new EquFunc,        // equ
        new ArithmeticFunc, // add
        new ArithmeticFunc, // sub
        new ArithmeticFunc, // mul
        new ArithmeticFunc, // div
        new LogicalFunc,    // les
        new LogicalFunc,    // leq
        new LogicalFunc,    // grt
        new LogicalFunc,    // geq
        new LogicalFunc,    // deq
        new LogicalFunc,    // neq
        new LogicalFunc,    // lor
        new LogicalFunc,    // land
        new NegFunc,        // neg
        new DimFunc,        // dim
        new AryFunc,        // ary
        new BitwiseFunc,    // or
        new BitwiseFunc,    // and
        new BitwiseFunc,    // xor
        new BitwiseFunc,    // lsh
        new BitwiseFunc,    // rsh
        new BnegFunc
    };

    AlgorithmRepository::AlgorithmRepository()
    {
    }

    AlgorithmRepository::~AlgorithmRepository()
    {
    }
};
