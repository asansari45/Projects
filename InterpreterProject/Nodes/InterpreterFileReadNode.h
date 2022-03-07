#pragma once
#include "InterpreterFileNode.h"

namespace Interpreter
{
    class Node;

    class FileReadNode : public FileNode
    {
    public:
        FileReadNode();

        FileReadNode(const FileReadNode& rProto);

        virtual ~FileReadNode();

        virtual Node* Clone();

        virtual void Free();

        virtual void Accept(Interpreter::NodeVisitor& rVisitor);
    };

};
