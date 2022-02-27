#include "InterpreterValueNode.h"

namespace Interpreter
{
    ValueNode::ValueNode() :
        Node(),
        m_Type(SymbolTable::SymbolInfo::ATOMIC),
        m_ArrayValue(),
        m_Value(),
        m_File()
    {
    }

    ValueNode::ValueNode(Value v) :
        Node(),
        m_Type(SymbolTable::SymbolInfo::ATOMIC),
        m_ArrayValue(),
        m_Value(v),
        m_File()
    {
    }

    ValueNode::ValueNode(ArrayValue v) :
        Node(),
        m_Type(SymbolTable::SymbolInfo::ARRAY),
        m_ArrayValue(v),
        m_Value(),
        m_File()
    {
    }

    ValueNode::ValueNode(std::string filename) :
        Node(),
        m_Type(SymbolTable::SymbolInfo::FILE),
        m_ArrayValue(),
        m_Value(),
        m_File(filename)
    {
    }
    
    ValueNode::ValueNode(const ValueNode& rNode) :
        Node(rNode),
        m_Type(rNode.m_Type),
        m_ArrayValue(rNode.m_ArrayValue),
        m_Value(rNode.m_Value),
        m_File(rNode.m_File)
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

