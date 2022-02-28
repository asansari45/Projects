#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{
    class File;
    class ValueNode;
    class VarNode;

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

        FileNode(const FileNode& rProto) :
            m_Command(rProto.m_Command),
            m_Varname(rProto.m_Varname),
            m_Filename(rProto.m_Filename),
            m_Mode(rProto.m_Mode),
            m_pFile(rProto.m_pFile),
            m_pNode(nullptr)
        {
        }

        virtual ~FileNode();

        virtual Node* Clone();

        virtual void Free();

        virtual void Accept(Interpreter::NodeVisitor& rVisitor);

        Command GetCommand(){ return m_Command; }
        void SetCommand(Command c){ m_Command = c; }
        std::string GetVarname(){ return m_Varname; }
        void SetVarname(std::string varname){ m_Varname = varname; }
        std::string GetFilename(){ return m_Filename; }
        void SetFilename(std::string filename){ m_Filename = filename; }
        void SetMode(std::string mode){ m_Mode = mode; }
        std::string GetMode(){ return m_Mode; }
        void SetFile(File* pFile){ m_pFile = pFile; }
        File* GetFile() { return m_pFile; }

        // Variabe/Value to read/write
        void SetNode(Node* pNode) { m_pNode = pNode; }
        Node* GetNode() { return m_pNode; }
        
    private:
        Command m_Command;
        std::string m_Varname;
        std::string m_Filename;
        std::string m_Mode;
        File* m_pFile;

        // Variable/Value to read/write
        Node* m_pNode;
    };

};
