#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{
    class ValueNode : public Node
    {
    public:
        ValueNode();
        ValueNode(Value v);
        ValueNode(ArrayValue v);
        ValueNode(const ValueNode& rNode);
        virtual ~ValueNode();
        Node* Clone();
        virtual void Accept(NodeVisitor& rVisitor);
        Value GetValue() { return m_Value; }
        void SetValue(Value v) { m_Array = false; m_Value = v; }
        ArrayValue& GetArrayValue() { return m_ArrayValue; }
        void SetArrayValue(ArrayValue v) { m_Array = true; m_ArrayValue = v; }
        bool IsArray() { return m_Array; }

    private:
        bool m_Array;
        ArrayValue m_ArrayValue;
        Value m_Value;
    };
};

