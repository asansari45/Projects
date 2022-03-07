#include "InterpreterFileWriteNode.h"
#include "InterpreterVarNode.h"
#include "InterpreterValueNode.h"

namespace Interpreter
{

FileWriteNode::FileWriteNode():
    FileNode(),
    m_pWriteNode(nullptr)
{
}

FileWriteNode::FileWriteNode(const FileWriteNode& rProto):
    FileNode(rProto),
    m_pWriteNode(rProto.m_pWriteNode->Clone())
{
}

FileWriteNode::~FileWriteNode()
{
    delete m_pWriteNode;
}

void FileWriteNode::Free()
{
    delete this;
}

Node* FileWriteNode::Clone()
{
    FileWriteNode* pNode = new FileWriteNode(*this);
    assert(pNode != nullptr);
    return pNode;
}

void FileWriteNode::Accept(Interpreter::NodeVisitor& rVisitor)
{
    rVisitor.VisitFileWriteNode(this);
}

};
