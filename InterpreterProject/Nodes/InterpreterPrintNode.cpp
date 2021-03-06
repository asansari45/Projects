#include <assert.h>
#include <ios>
#include <sstream>
#include <iomanip>
#include "InterpreterPrintNode.h"

namespace Interpreter
{

    PrintNode::PrintNode() :
        m_Format(PrintNode::NONE),
        m_FormatArg(0),
        m_pChild(nullptr)
    {
    }

    PrintNode::PrintNode(Format format) :
        m_Format(format),
        m_FormatArg(0),
        m_pChild(nullptr)
    {
    }

    PrintNode::PrintNode(Format format, int formatArg) :
        m_Format(format),
        m_FormatArg(formatArg),
        m_pChild(nullptr)
    {
    }

    PrintNode::PrintNode(const PrintNode& rProto):
        Node(rProto),
        m_Format(rProto.m_Format),
        m_FormatArg(rProto.m_FormatArg),
        m_pChild(nullptr)
    {
        if (rProto.m_pChild != nullptr)
        {
            m_pChild = rProto.m_pChild->CloneList();
        }
    }

    PrintNode::~PrintNode()
    {
        if (m_pChild)
        {
            m_pChild->FreeList();
            m_pChild = nullptr;
        }
    }

    Node* PrintNode::Clone()
    {
        return new PrintNode(*this);
    }

    void PrintNode::Free()
    {
        Node::Free();
    }

    void PrintNode::Accept(Interpreter::NodeVisitor& rVisitor)
    {
        rVisitor.VisitPrintNode(this);
    }

    void PrintNode::ModifyStream(std::stringstream& rStream)
    {
        assert(m_Format != NONE);
        if (m_Format == DEC)
        {
            rStream << std::dec;
        }
        else if (m_Format == HEX)
        {
            rStream << std::hex;
        }
        else if (m_Format == OCT)
        {
            rStream << std::oct;
        }
        else if (m_Format == WIDTH)
        {
            rStream << std::setw(m_FormatArg);
        }
        else if (m_Format == FILL)
        {
            rStream << std::setfill(static_cast<char>(m_FormatArg));
        }
        else if (m_Format == ENDL)
        {
            rStream << std::endl;
        }
    }

};
