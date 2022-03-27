#include <memory>
#include "InterpreterBitwiseFunc.h"
#include "Nodes/InterpreterValueNode.h"
#include "Visitors/InterpreterErrorInterface.h"

namespace Interpreter
{
    unsigned int uior(unsigned int a, unsigned int b)
    {
        return a | b;
    }
    unsigned int uiand(unsigned int a, unsigned int b)
    {
        return a & b;
    }
    unsigned int uixor(unsigned int a, unsigned int b)
    {
        return a ^ b;
    }
    unsigned int uilsh(unsigned int a, unsigned int b)
    {
        return a << b;
    }
    unsigned int uirsh(unsigned int a, unsigned int b)
    {
        return a >> b;
    }
    static unsigned int (*uibfuncptr[])(unsigned int, unsigned int) = {
        uior,  // or
        uiand, // and
        uixor, // xor
        uilsh, // lsh
        uirsh  // rsh
    };

    int ior(int a, int b)
    {
        return a | b;
    }
    int iand(int a, int b)
    {
        return a & b;
    }
    int ixor(int a, int b)
    {
        return a ^ b;
    }
    int ilsh(int a, int b)
    {
        return a << b;
    }
    int irsh(int a, int b)
    {
        return a >> b;
    }
    static int (*ibfuncptr[])(int, int) = {
        ior,  // or
        iand, // and
        ixor, // xor
        ilsh, // lsh
        irsh  // rsh
    };

    Node* BitwiseFunc::Perform(BinaryNode::Operator oper, Node* pLeft, Node* pRight, 
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

        // Array operations are not allowed with binary or, and, etc...
        if (lhs->IsArray() || rhs->IsArray())
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
            unsigned int result = uibfuncptr[oper-BinaryNode::OR]((unsigned int)lhs->GetValueRef(), 
                                                                  (unsigned int)rhs->GetValueRef());
            resultValue.SetValue(result);
        }
        else if (opertype == typeid(int))
        {
            int result = ibfuncptr[oper-BinaryNode::OR]((int)lhs->GetValueRef(),
                                                        (int)rhs->GetValueRef());
            resultValue.SetValue(result);
        }
        else
        {
            assert(false);
        }

        return new ValueNode(resultValue);
    }

    bool BitwiseFunc::CheckBinaryTypes(BinaryNode::Operator oper, ValueNode* pA, ValueNode* pB)
    {
        std::type_index atype = pA->GetValueRef().GetType();
        std::type_index btype = pB->GetValueRef().GetType();

        // All other operations are not allowed when either side is a string.
        if (atype == typeid(std::string) || btype == typeid(std::string))
        {
            return false;
        }

        if (atype == typeid(float) || btype == typeid(float))
        {
            return false;
        }

        // The only types are integer and float which are allowed.
        return true;
    }

    std::type_index BitwiseFunc::GetResultantType(ValueNode* pA, ValueNode* pB)
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

