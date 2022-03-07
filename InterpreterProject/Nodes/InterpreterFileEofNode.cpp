#include "InterpreterFileEofNode.h"
#include "InterpreterVarNode.h"
#include "InterpreterValueNode.h"

namespace Interpreter
{

FileEofNode::FileEofNode():
    FileNode()
{
}

FileEofNode::FileEofNode(const FileEofNode& rProto):
    FileNode(rProto)
{
}

FileEofNode::~FileEofNode()
{
}

void FileEofNode::Free()
{
    delete this;
}

Node* FileEofNode::Clone()
{
    FileEofNode* pNode = new FileEofNode(*this);
    assert(pNode != nullptr);
    return pNode;
}

void FileEofNode::Accept(Interpreter::NodeVisitor& rVisitor)
{
    rVisitor.VisitFileEofNode(this);
}

};
