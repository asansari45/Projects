#pragma once
#include "InterpreterFileNode.h"

namespace Interpreter
{
    class Node;

    class FileCloseNode : public FileNode
    {
    public:
        FileCloseNode();

        FileCloseNode(const FileCloseNode& rProto);

        virtual ~FileCloseNode();

        virtual Node* Clone();

        virtual void Free();

        virtual void Accept(Interpreter::NodeVisitor& rVisitor);
    };

};
