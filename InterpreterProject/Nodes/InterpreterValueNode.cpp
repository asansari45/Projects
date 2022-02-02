#include "InterpreterValueNode.h"

namespace Interpreter
{
    ValueNode::ValueNode() :
        Node(),
        m_Array(false),
        m_ArrayValue(),
        m_Value()
    {
    }

    ValueNode::ValueNode(Value v) :
        Node(),
        m_Array(false),
        m_ArrayValue(),
        m_Value(v)
    {
    }

    ValueNode::ValueNode(ArrayValue v) :
        Node(),
        m_Array(true),
        m_ArrayValue(v),
        m_Value()
    {
    }

    ValueNode::ValueNode(const ValueNode& rNode) :
        Node(rNode),
        m_Value(rNode.m_Value)
    {
    }

    ValueNode::~ValueNode()
    {
    }

    Node* ValueNode::Clone()
    {
        return new ValueNode(*this);
    }

    void ValueNode::Accept(NodeVisitor& rVisitor)
    {
        rVisitor.VisitValueNode(this);
    }
};

