#include "InterpreterFunctionDefNode.h"

namespace Interpreter
{

FunctionDefNode::FunctionDefNode(std::string name) :
    m_Name(name),
    m_pInputVars(nullptr),
    m_pCode(nullptr),
    m_pSymbolTable(nullptr)
{
    m_pSymbolTable = new SymbolTable(m_Name);
    assert(m_pSymbolTable != nullptr);
}

FunctionDefNode::~FunctionDefNode()
{
    if (m_pInputVars)
    {
        m_pInputVars->FreeList();
        m_pInputVars = nullptr;
    }

    if (m_pCode)
    {
        m_pCode->FreeList();
        m_pCode = nullptr;
    }

    delete m_pSymbolTable;
    m_pSymbolTable = nullptr;
}

Node* FunctionDefNode::Clone()
{
    FunctionDefNode* pNode = new FunctionDefNode(m_Name);
    pNode->SetInputVars(m_pInputVars->CloneList());
    pNode->SetCode(m_pCode->CloneList());
    pNode->SetSymbolTable(m_pSymbolTable->Clone());
    return pNode;
}

void FunctionDefNode::Free()
{
    Node::Free();
}

void FunctionDefNode::Accept(Interpreter::NodeVisitor& rVisitor)
{
    rVisitor.VisitFunctionDefNode(this);
}

int FunctionDefNode::GetInputVarCount()
{
    int count = 0;
    for (Interpreter::Node* pNode = m_pInputVars; pNode; pNode = pNode->GetNext())
    {
        count++;
    }
    return count;
}

void FunctionDefNode::SetSymbolTable(SymbolTable* pSymbolTable)
{
    delete m_pSymbolTable;
    m_pSymbolTable = pSymbolTable;
}

};
