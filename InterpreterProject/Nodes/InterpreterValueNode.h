#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{
    class ValueNode : public Node
    {
    public:
        ValueNode() :
            Node(),
            m_Array(false),
            m_ArrayValue(),
            m_Value()
        {
        }

        ValueNode(Value v) :
            Node(),
            m_Array(false),
            m_ArrayValue(),
            m_Value(v)
        {
        }

        ValueNode(ArrayValue v) :
            Node(),
            m_Array(true),
            m_ArrayValue(v),
            m_Value()
        {
        }

        ValueNode(const ValueNode& rNode) :
            Node(rNode),
            m_Value(rNode.m_Value)
        {
        }

        virtual ~ValueNode()
        {
        }

        Node* Clone()
        {
            return new ValueNode(*this);
        }

        virtual void Accept(NodeVisitor& rVisitor)
        {
            rVisitor.VisitValueNode(this);
        }

        Value GetValue() { return m_Value; }
        void SetValue(Value v) { m_Array = false; m_Value = v; }

        ArrayValue GetArrayValue() { return m_ArrayValue; }
        void SetArrayValue(ArrayValue v) { m_Array = true; m_ArrayValue = v; }

        bool IsArray() { return m_Array; }

    private:
        bool m_Array;
        ArrayValue m_ArrayValue;
        Value m_Value;
    };
};

