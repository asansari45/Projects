#include "InterpreterFileReadNode.h"
#include "InterpreterVarNode.h"
#include "InterpreterValueNode.h"

namespace Interpreter
{

FileReadNode::FileReadNode():
    FileNode()
{
}

FileReadNode::FileReadNode(const FileReadNode& rProto):
    FileNode(rProto)
{
}

FileReadNode::~FileReadNode()
{
}

void FileReadNode::Free()
{
    delete this;
}

Node* FileReadNode::Clone()
{
    FileReadNode* pNode = new FileReadNode(*this);
    assert(pNode != nullptr);
    return pNode;
}

void FileReadNode::Accept(Interpreter::NodeVisitor& rVisitor)
{
    rVisitor.VisitFileReadNode(this);
}

};
