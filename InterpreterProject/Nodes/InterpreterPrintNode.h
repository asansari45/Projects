#pragma once
#include "InterpreterNode.h"

namespace Interpreter
{

    class PrintNode : public Node
    {
    public:
        enum Format 
        {
            NONE =  1,
            DEC =   2,
            HEX =   3,
            OCT =   4,
            WIDTH = 5,
            FILL  = 6,
            ENDL  = 7
        };

        PrintNode();
        PrintNode(Format fmt);
        PrintNode(Format fmt, int arg);
        PrintNode(const PrintNode& rProto);
        virtual ~PrintNode();
        virtual Node* Clone();
        virtual void Free();
        virtual void Accept(Interpreter::NodeVisitor& rVisitor);
        Node* GetChild() { return m_pChild; }
        void SetChild(Node* pChild) { m_pChild = pChild; }
        void SetFormat(Format format) { m_Format = format; }
        int GetFormat() { return m_Format; }
        void SetFormatArg(int formatArg) { m_FormatArg = formatArg; }
        int GetFormatArg() { return m_FormatArg; }
        void ModifyStream(std::stringstream& rStream);
    private:
        Format m_Format;
        int    m_FormatArg;
        Node* m_pChild;
    };

};
