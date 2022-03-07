#include "InterpreterFileCloseNode.h"
#include "InterpreterVarNode.h"
#include "InterpreterValueNode.h"

namespace Interpreter
{

FileCloseNode::FileCloseNode():
    FileNode()
{
}

FileCloseNode::FileCloseNode(const FileCloseNode& rProto):
    FileNode(rProto)
{
}

FileCloseNode::~FileCloseNode()
{
}

void FileCloseNode::Free()
{
    delete this;
}

Node* FileCloseNode::Clone()
{
    FileCloseNode* pNode = new FileCloseNode(*this);
    assert(pNode != nullptr);
    return pNode;
}

void FileCloseNode::Accept(Interpreter::NodeVisitor& rVisitor)
{
    rVisitor.VisitFileCloseNode(this);
}

};
