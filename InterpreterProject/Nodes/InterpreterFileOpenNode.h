#pragma once
#include "InterpreterFileNode.h"

namespace Interpreter
{
    class FileOpenNode : public FileNode
    {
    public:

        FileOpenNode();

        FileOpenNode(const FileOpenNode& rProto);

        virtual ~FileOpenNode();

        virtual Node* Clone();

        virtual void Free();

        virtual void Accept(Interpreter::NodeVisitor& rVisitor);

        Node* GetFilenameNode() { return m_pFilenameNode; }
        void SetFilenameNode(Node* pFilenameNode){ m_pFilenameNode = pFilenameNode; }

        void SetModeNode(Node* pModeNode){ m_pModeNode = pModeNode; }
        Node* GetModeNode(){ return m_pModeNode; }

    private:
        Node* m_pFilenameNode; // Filename node
        Node* m_pModeNode;     // Mode node
    };

};
