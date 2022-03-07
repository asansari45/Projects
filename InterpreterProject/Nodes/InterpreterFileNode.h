#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{
    class Node;

    class FileNode : public Node
    {
    public:
        FileNode();

        FileNode(const FileNode& rProto);

        virtual ~FileNode();

        virtual Node* Clone();

        virtual void Free();

        Node* GetFileNode() { return m_pFileNode; }
        void SetFileNode(Node* pFileNode) { m_pFileNode = pFileNode; }

    private:
        Node* m_pFileNode;     // Node holding File* value
    };

};
