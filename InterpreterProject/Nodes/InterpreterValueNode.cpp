#include "InterpreterValueNode.h"

namespace Interpreter
{
    ValueNode::ValueNode() :
        Node(),
        m_Array(false),
        m_pArrayValue(nullptr),
        m_Value()
    {
    }

    ValueNode::ValueNode(Value v) :
        Node(),
        m_Array(false),
        m_pArrayValue(nullptr),
        m_Value(v)
    {
    }

    ValueNode::ValueNode(ArrayValue* pArrayValue) :
        Node(),
        m_Array(true),
        m_pArrayValue(pArrayValue),
        m_Value()
    {
    }

    ValueNode::ValueNode(const ValueNode& rNode) :
        Node(rNode),
        m_Array(rNode.m_Array),
        m_Value(rNode.m_Value)
    {
        if (rNode.IsArray())
        {
            m_pArrayValue = rNode.GetArrayValue()->Clone();
        }
    }

    ValueNode::~ValueNode()
    {
        delete m_pArrayValue;
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

