#include "InterpreterFileOpenNode.h"
#include "InterpreterVarNode.h"
#include "InterpreterValueNode.h"

namespace Interpreter
{

FileOpenNode::FileOpenNode():
    m_pFilenameNode(nullptr),
    m_pModeNode(nullptr)
{
}

FileOpenNode::FileOpenNode(const FileOpenNode& rProto):
    m_pFilenameNode(rProto.m_pFilenameNode),
    m_pModeNode(rProto.m_pModeNode)
{
}

FileOpenNode::~FileOpenNode()
{
    delete m_pFilenameNode;
    delete m_pModeNode;
}

void FileOpenNode::Free()
{
    delete this;
}

Node* FileOpenNode::Clone()
{
    FileOpenNode* pNode = new FileOpenNode(*this);
    assert(pNode != nullptr);
    pNode->SetFilenameNode(m_pFilenameNode->Clone());
    pNode->SetModeNode(m_pModeNode->Clone());
    return pNode;
}

void FileOpenNode::Accept(Interpreter::NodeVisitor& rVisitor)
{
    rVisitor.VisitFileOpenNode(this);
}

};
