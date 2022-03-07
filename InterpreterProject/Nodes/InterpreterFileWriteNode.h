#pragma once
#include "InterpreterFileNode.h"

namespace Interpreter
{
    class Node;

    class FileWriteNode : public FileNode
    {
    public:
        FileWriteNode();

        FileWriteNode(const FileWriteNode& rProto);

        virtual ~FileWriteNode();

        virtual Node* Clone();

        virtual void Free();

        virtual void Accept(Interpreter::NodeVisitor& rVisitor);

        Node* GetWriteNode() { return m_pWriteNode; }
        void SetWriteNode(Node* pWriteNode) { m_pWriteNode = pWriteNode; }
        
    private:
        Node* m_pWriteNode;    // Node holding value to write
    };

};
