#pragma once
#include "InterpreterFileNode.h"

namespace Interpreter
{
    class Node;

    class FileEofNode : public FileNode
    {
    public:
        FileEofNode();

        FileEofNode(const FileEofNode& rProto);

        virtual ~FileEofNode();

        virtual Node* Clone();

        virtual void Free();

        virtual void Accept(Interpreter::NodeVisitor& rVisitor);
    };

};
