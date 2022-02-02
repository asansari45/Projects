#include "InterpreterVarNode.h"

namespace Interpreter
{

    VarNode::VarNode() :
        Node(),
        m_Name(),
        m_ArraySpecifier(),
        m_SymbolPresent(false),
        m_SymbolInfo(),
        m_pNoSymbol(nullptr)
    {
    }

    VarNode::VarNode(const VarNode& rNode) :
        Node(rNode),
        m_Name(rNode.m_Name),
        m_ArraySpecifier(rNode.m_ArraySpecifier),
        m_SymbolPresent(rNode.m_SymbolPresent),
        m_SymbolInfo(rNode.m_SymbolInfo),
        m_pNoSymbol(rNode.m_pNoSymbol)
    {
    }

    VarNode::~VarNode()
    {
    }

    Node* VarNode::Clone()
    {
        return new VarNode(*this);
    }

    void VarNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitVarNode(this);
    }
};
