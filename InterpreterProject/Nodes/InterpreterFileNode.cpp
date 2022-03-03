#pragma once
#include "InterpreterFileNode.h"
#include "InterpreterVarNode.h"
#include "InterpreterValueNode.h"

namespace Interpreter
{

FileNode::FileNode():
    m_Command(UNKNOWN),
    m_pFileNode(nullptr),
    m_pWriteNode(nullptr),
    m_pFilenameNode(nullptr),
    m_pModeNode(nullptr)
{
}

FileNode::FileNode(const FileNode& rProto):
    m_Command(rProto.m_Command),
    m_pFileNode(rProto.m_pFileNode),
    m_pWriteNode(rProto.m_pWriteNode),
    m_pFilenameNode(rProto.m_pFilenameNode),
    m_pModeNode(rProto.m_pModeNode)
{
}

FileNode::~FileNode()
{
    delete m_pFileNode;
    delete m_pWriteNode;
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
    pNode->SetWriteNode(m_pWriteNode->Clone());
    pNode->SetFilenameNode(m_pFilenameNode->Clone());
    pNode->SetModeNode(m_pModeNode->Clone());
    return pNode;
}

void FileNode::Accept(Interpreter::NodeVisitor& rVisitor)
{
    rVisitor.VisitFileNode(this);
}

};
