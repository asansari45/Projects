#pragma once

#include "InterpreterValue.h"
#include "InterpreterArrayValue.h"
#include "Nodes/InterpreterValueNode.h"

namespace Interpreter
{
    class ArrayValue;

    class Rvalue
    {
    public:
        Rvalue(ArrayValue& rArrayValue) :
            m_IsArray(true),
            m_Value(),
            m_ArrayValue(rArrayValue)
        {
        }

        Rvalue(Value& rValue) :
            m_IsArray(false),
            m_Value(rValue),
            m_ArrayValue()
        {
        }

        Rvalue(ValueNode* pValueNode) :
            m_IsArray(false),
            m_Value(),
            m_ArrayValue()
        {
            if (pValueNode->IsArray())
            {
                m_IsArray = true;
                m_ArrayValue = pValueNode->GetArrayValue();
            }
            else
            {
                m_Value = pValueNode->GetValue();
            }
        }

        ~Rvalue()
        {
        }

        bool IsArray() const { return m_IsArray; }
        ArrayValue& GetArrayValue() { return m_ArrayValue; }
        Value& GetValue() { return m_Value; }

    private:
        bool m_IsArray;
        Value m_Value;
        ArrayValue m_ArrayValue;
    };
}


