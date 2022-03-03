#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{
    class Node;

    class FileNode : public Node
    {
    public:
        enum Command
        {
            UNKNOWN = 1,
            OPEN = 2,
            READ = 3,
            WRITE = 4,
            CLOSE = 5,
            END_OF_FILE = 6
        };

        FileNode();

        FileNode(const FileNode& rProto);

        virtual ~FileNode();

        virtual Node* Clone();

        virtual void Free();

        virtual void Accept(Interpreter::NodeVisitor& rVisitor);

        Command GetCommand(){ return m_Command; }
        void SetCommand(Command c){ m_Command = c; }
        
        Node* GetFileNode() { return m_pFileNode; }
        void SetFileNode(Node* pFileNode) { m_pFileNode = pFileNode; }

        Node* GetWriteNode() { return m_pWriteNode; }
        void SetWriteNode(Node* pWriteNode) { m_pWriteNode = pWriteNode; }
        
        Node* GetFilenameNode() { return m_pFilenameNode; }
        void SetFilenameNode(Node* pFilenameNode){ m_pFilenameNode = pFilenameNode; }

        void SetModeNode(Node* pModeNode){ m_pModeNode = pModeNode; }
        Node* GetModeNode(){ return m_pModeNode; }

    private:
        Command m_Command;
        Node* m_pFileNode;     // Node holding File* value
        Node* m_pWriteNode;    // Node holding value to write
        Node* m_pFilenameNode; // Filename node
        Node* m_pModeNode;     // Mode node
    };

};
