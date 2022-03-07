#pragma once
#include "InterpreterFileNode.h"
#include "InterpreterVarNode.h"
#include "InterpreterValueNode.h"

namespace Interpreter
{

FileNode::FileNode():
    m_pFileNode(nullptr)
{
}

FileNode::FileNode(const FileNode& rProto):
    m_pFileNode(rProto.m_pFileNode->Clone())
{
}

FileNode::~FileNode()
{
    delete m_pFileNode;
}

void FileNode::Free()
{
    delete m_pFileNode;
    delete this;
}

Node* FileNode::Clone()
{
    FileNode* pNode = new FileNode(*this);
    assert(pNode != nullptr);
    return pNode;
}

};
