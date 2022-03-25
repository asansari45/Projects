#include <memory>
#include "InterpreterArithmeticFunc.h"
#include "Nodes/InterpreterValueNode.h"
#include "Visitors/InterpreterErrorInterface.h"

namespace Interpreter
{
    unsigned int uiadd(unsigned int a, unsigned int b)
    {
        return a + b;
    }
    unsigned int uisub(unsigned int a, unsigned int b)
    {
        return a - b;
    }
    unsigned int uimul(unsigned int a, unsigned int b)
    {
        return a * b;
    }
    unsigned int uidiv(unsigned int a, unsigned int b)
    {
        return a / b;
    }
    static unsigned int (*uifuncptr[])(unsigned int, unsigned int) = 
    {
        uiadd,
        uisub,
        uimul,
        uidiv
    };

    float fadd(float a, float b)
    {
        return a + b;
    }
    float fsub(float a, float b)
    {
        return a - b;
    }
    float fmul(float a, float b)
    {
        return a * b;
    }
    float fdiv(float a, float b)
    {
        return a / b;
    }
    static float (*ffuncptr[])(float, float) = 
    {
        fadd,
        fsub,
        fmul,
        fdiv
    };

    int iadd(int a, int b)
    {
        return a + b;
    }
    int isub(int a, int b)
    {
        return a - b;
    }
    int imul(int a, int b)
    {
        return a * b;
    }
    int idiv(int a, int b)
    {
        return a / b;
    }
    static int (*ifuncptr[])(int, int) = {
        iadd,
        isub,
        imul,
        idiv
    };

    std::string sadd(std::string a, std::string b)
    {
        return a + b;
    }

    static std::string (*sfuncptr[])(std::string, std::string) = {
        sadd
    };


    Node* ArithmeticFunc::Perform(BinaryNode::Operator oper, Node* pLeft, Node* pRight, 
                                  ExecutionNodeVisitorServices* pServices,
                                  ErrorInterface* pErrorInterface)
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

        // Array operations are not allowed with binary add, sub, etc...
        if (lhs->IsArray() || rhs-IsArray())
        {
            ErrorInterface::ErrorInfo err(pRight);
            char buf[512];
            sprintf_s(buf, sizeof(buf), pErrorInterface->ERROR_ARRAY_OPERATION_FAILED);
            err.m_Msg = buf;
            pErrorInterface->SetErrorInfo(err);
            return nullptr;
        }

        bool status = CheckBinaryTypes(oper, lhs.get(), rhs.get());
        if (!status)
        {
            ErrorInterface::ErrorInfo err(pLeft);
            err.m_Msg = pErrorInterface->ERROR_BINARY_OPERATION_FAILED;
            pErrorInterface->SetErrorInfo(err);
            return nullptr;
        }

        // Get the type that operation should be performed in
        std::type_index opertype = GetResultantType(lhs.get(), rhs.get());
        Value resultValue;
        if (opertype == typeid(unsigned int))
        {
            unsigned int result = uifuncptr[oper-BinaryNode::ADD]((unsigned int)lhs->GetValueRef(), 
                                                                  (unsigned int)rhs->GetValueRef());
            resultValue.SetUnsignedIntValue(result);
        }
        else if (opertype == typeid(int))
        {
            int result = ifuncptr[oper-BinaryNode::ADD]((int)lhs->GetValueRef(),
                                                        (int)rhs->GetValueRef());
            resultValue.SetIntValue(result);
        }
        else if (opertype == typeid(float))
        {
            float result = ffuncptr[oper-BinaryNode::ADD]((float)lhs->GetValueRef(),
                                                          (float)rhs->GetValueRef());
            resultValue.SetFloatValue(result);
        }
        else if (opertype == typeid(std::string))
        {
            std::string result = sfuncptr[oper-BinaryNode::ADD](lhs->GetValueRef().GetStringValue(),
                                                                rhs->GetValueRef().GetStringValue());
            resultValue.SetStringValue(result);
        }

        return new ValueNode(resultValue);
    }

    bool ArithmeticFunc::CheckBinaryTypes(BinaryNode::Operator oper, ValueNode* pA, ValueNode* pB)
    {
        std::type_index atype = pA->GetValueRef().GetType();
        std::type_index btype = pB->GetValueRef().GetType();

        // add operations are only allowed on strings.
        if (oper == BinaryNode::ADD)
        {
            if (atype == typeid(std::string) && btype == typeid(std::string))
            {
                return true;
            }
        }

        // All other operations are not allowed when either side is a string.
        if (atype == typeid(std::string) || btype == typeid(std::string))
        {
            return false;
        }

        // The only types are integer and float which are allowed.
        return true;
    }

    std::type_index ArithmeticFunc::GetResultantType(ValueNode* pA, ValueNode* pB)
    {
        std::type_index atype = pA->GetValueRef().GetType();
        std::type_index btype = pB->GetValueRef().GetType();

        if (atype == btype)
        {
            return atype;
        }

        if (atype == typeid(float) || btype == typeid(float))
        {
            return typeid(float);
        }

        // Let's just make it unsigned
        return typeid(unsigned int);
    }
};

