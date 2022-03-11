#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{
    class ValueNode : public Node
    {
    public:
        ValueNode();
        ValueNode(Value v);
        ValueNode(ArrayValue* v);
        ValueNode(const ValueNode& rNode);
        virtual ~ValueNode();
        Node* Clone();
        virtual void Accept(NodeVisitor& rVisitor);
        Value GetValue() { return m_Value; }
        Value& GetValueRef() { return m_Value; }
        void SetValue(Value v) { m_Array = false; m_Value = v; }
        ArrayValue* GetArrayValue() const { return m_pArrayValue; }
        void SetArrayValue(ArrayValue* pValue) { m_Array = true; m_pArrayValue = pValue; }
        bool IsArray() const { return m_Array; }

    private:
        bool m_Array;
        ArrayValue* m_pArrayValue;
        Value m_Value;
    };
};

