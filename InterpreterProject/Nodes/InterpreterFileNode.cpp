#pragma once
#include "InterpreterFileNode.h"

namespace Interpreter
{

FileNode::FileNode():
    m_Command(UNKNOWN),
    m_Name(),
    m_Mode(),
    m_pFile(nullptr),
    m_pNode(nullptr)
{
}

FileNode::~FileNode()
{
    delete m_pNode;
    m_pNode = nullptr;
}

Node* FileNode::Clone()
{
    FileNode* pNode = new FileNode(*this);
    assert(pNode != nullptr);
    if (m_pNode != nullptr)
    {
        pNode->SetNode(m_pNode->Clone());
    }

    return pNode;
}

void FileNode::Accept(Interpreter::NodeVisitor& rVisitor)
{
    rVisitor.VisitFileNode(this);
}

};
