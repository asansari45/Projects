#pragma once

#include <string>
#include <vector>
#include "Visitors/InterpreterNodeVisitor.hpp"
#include "Values/InterpreterValue.h"
#include "Values/InterpreterArrayValue.h"
#include "Driver/InterpreterContext.h"
#include "Tables/InterpreterSymbolTable.h"

namespace Interpreter
{

class Node
{
public:
    Node();
    Node(const Node& rNode);

    virtual ~Node()
    {
    }

    // Clone only node and sons.
    virtual Node* Clone();
    Node* CloneList();
    void FreeList();
    virtual void Free();

    virtual void Accept(NodeVisitor& rVisitor);

    Node* GetNext() { return m_pNext; }
    void SetNext(Node* pNext) { m_pNext = pNext; }

    void SetFile(std::string f) { m_File = f; }
    std::string GetFile() { return m_File; }
    void SetLine(int line) { m_Line = line; }
    int GetLine() { return m_Line; }
    void SetColumn(int column) { m_Column = column; }
    int GetColumn() { return m_Column; }

private:
    Node* m_pNext;
    std::string m_File;
    int m_Line;
    int m_Column;
};

};
