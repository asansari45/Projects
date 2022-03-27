#include <memory>
#include "InterpreterLogicalFunc.h"
#include "Nodes/InterpreterValueNode.h"
#include "Visitors/InterpreterErrorInterface.h"

namespace Interpreter
{
    bool cles(char a, char b)
    {
        return a < b;
    }
    bool cleq(char a, char b)
    {
        return a <= b;
    }
    bool cgrt(char a, char b)
    {
        return a > b;
    }
    bool cgeq(char a, char b)
    {
        return a >= b;
    }
    bool cdeq(char a, char b)
    {
        return a == b;
    }
    bool cneq(char a, char b)
    {
        return a != b;
    }
    bool clor(char a, char b)
    {
        return a || b;
    }
    bool cland(char a, char b)
    {
        return a && b;
    }
    static bool (*clfuncptr[])(char, char) = {
        cles,
        cleq,
        cgrt,
        cgeq,
        cdeq,
        cneq,
        clor,
        cland
    };

    bool uiles(unsigned int a, unsigned int b)
    {
        return a < b;
    }
    bool uileq(unsigned int a, unsigned int b)
    {
        return a <= b;
    }
    bool uigrt(unsigned int a, unsigned int b)
    {
        return a > b;
    }
    bool uigeq(unsigned int a, unsigned int b)
    {
        return a >= b;
    }
    bool uideq(unsigned int a, unsigned int b)
    {
        return a == b;
    }
    bool uineq(unsigned int a, unsigned int b)
    {
        return a != b;
    }
    bool uilor(unsigned int a, unsigned int b)
    {
        return a || b;
    }
    bool uiland(unsigned int a, unsigned int b)
    {
        return a && b;
    }
    static bool (*uilfuncptr[])(unsigned int, unsigned int) = {
        uiles,
        uileq,
        uigrt,
        uigeq,
        uideq,
        uineq,
        uilor,
        uiland
    };

    bool fles(float a, float b)
    {
        return a < b;
    }
    bool fleq(float a, float b)
    {
        return a <= b;
    }
    bool fgrt(float a, float b)
    {
        return a > b;
    }
    bool fgeq(float a, float b)
    {
        return a >= b;
    }
    bool fdeq(float a, float b)
    {
        return a == b;
    }
    bool fneq(float a, float b)
    {
        return a != b;
    }
    bool flor(float a, float b)
    {
        return a || b;
    }
    bool fland(float a, float b)
    {
        return a && b;
    }
    static bool (*flfuncptr[])(float, float) = {
        fles,
        fleq,
        fgrt,
        fgeq,
        fdeq,
        fneq,
        flor,
        fland
    };

    bool iles(int a, int b)
    {
        return a < b;
    }
    bool ileq(int a, int b)
    {
        return a <= b;
    }
    bool igrt(int a, int b)
    {
        return a > b;
    }
    bool igeq(int a, int b)
    {
        return a >= b;
    }
    bool ideq(int a, int b)
    {
        return a == b;
    }
    bool ineq(int a, int b)
    {
        return a != b;
    }
    bool ilor(int a, int b)
    {
        return a || b;
    }
    bool iland(int a, int b)
    {
        return a && b;
    }
    static bool (*ilfuncptr[])(int, int) = {
        iles,
        ileq,
        igrt,
        igeq,
        ideq,
        ineq,
        ilor,
        iland
    };

    bool sles(std::string a, std::string b)
    {
        return a < b;
    }
    bool sleq(std::string a, std::string b)
    {
        return a <= b;
    }
    bool sgrt(std::string a, std::string b)
    {
        return a > b;
    }
    bool sgeq(std::string a, std::string b)
    {
        return a >= b;
    }
    bool sdeq(std::string a, std::string b)
    {
        return a == b;
    }
    bool sneq(std::string a, std::string b)
    {
        return a != b;
    }
    bool sor(std::string a, std::string b)
    {
        return a.size() == 0 || b.size() == 0;
    }
    bool sand(std::string a, std::string b)
    {
        return a.size() && b.size();
    }
    static bool (*slfuncptr[])(std::string, std::string) = {
        sles,
        sleq,
        sgrt,
        sgeq,
        sdeq,
        sneq,
        sor,
        sand
    };

    Node* LogicalFunc::Perform(BinaryNode::Operator oper, Node* pLeft, Node* pRight, 
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
        if (opertype == typeid(char))
        {
            bool result = clfuncptr[oper-BinaryNode::LES]((char)lhs->GetValueRef(),
                                                          (char)rhs->GetValueRef());
            resultValue.SetValue<int>(result);
        }
        else if (opertype == typeid(unsigned int))
        {
            bool result = uilfuncptr[oper-BinaryNode::LES]((unsigned int)lhs->GetValueRef(),
                                                           (unsigned int)rhs->GetValueRef());
            resultValue.SetValue<int>(result);
        }
        else if (opertype == typeid(int))
        {
            bool result = ilfuncptr[oper-BinaryNode::LES]((int)lhs->GetValueRef(),
                                                          (int)rhs->GetValueRef());
            resultValue.SetValue<int>(result);
        }
        else if (opertype == typeid(float))
        {
            bool result = flfuncptr[oper-BinaryNode::LES]((float) lhs->GetValueRef(),
                                                          (float) rhs->GetValueRef());
            resultValue.SetValue<int>(result);
        }
        else if (opertype == typeid(std::string))
        {
            bool result = slfuncptr[oper-BinaryNode::LES](lhs->GetValueRef().GetValue<std::string>(),
                                                          rhs->GetValueRef().GetValue<std::string>());
            resultValue.SetValue<int>(result);
        }

        return new ValueNode(resultValue);
    }

    bool LogicalFunc::CheckBinaryTypes(BinaryNode::Operator oper, ValueNode* pA, ValueNode* pB)
    {
        std::type_index atype = pA->GetValueRef().GetType();
        std::type_index btype = pB->GetValueRef().GetType();

        // if one side is string the other side has to be string as well.
        if (atype == typeid(std::string))
        {
            if (btype == typeid(std::string))
            {
                return true;
            }

            return false;
        }

        // ints and floats may be compared with one another.
        return true;
    }

    std::type_index LogicalFunc::GetResultantType(ValueNode* pA, ValueNode* pB)
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

