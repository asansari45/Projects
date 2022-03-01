#pragma once
#include "InterpreterFileNode.h"
#include "VarNode.h"
#include "ValueNode.h"

namespace Interpreter
{

FileNode::FileNode():
    m_Command(UNKNOWN),
    m_pFileNode(nullptr),
    m_pFilenameNode(nullptr),
    m_pModeNode(nullptr),
    m_pFile(nullptr)
{
}

FileNode::FileNode(const FileNode& rProto):
    m_Command(rProto.m_Command),
    m_pFileNode(rProto.m_pFileNode),
    m_pFilenameNode(rProto.m_pFilenameNode),
    m_pModeNode(rProto.m_pModeNode),
    m_pFile(rProto.m_pFile)
{
}

FileNode::~FileNode()
{
    delete m_pFileNode;
    delete m_pFilenameNode;
    delete m_pModeNode;
}

void FileNode::Free()
{
    delete this;
}

Node* FileNode::Clone()
{
    FileNode* pNode = new FileNode(*this);
    assert(pNode != nullptr);
    pNode->SetFileNode(m_pFileNode->Clone());
    pNode->SetFilenameNode(m_pFilenameNode->Clone());
    pNode->SetModeNode(m_pModeNode->Clone());
}

void FileNode::Accept(Interpreter::NodeVisitor& rVisitor)
{
    rVisitor.VisitFileNode(this);
}

};
