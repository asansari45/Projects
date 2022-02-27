#pragma once
#include "InterpreterNode.h"
#include "Tables/InterpreterSymbolTable.h"

namespace Interpreter
{
    class ValueNode : public Node
    {
    public:
        ValueNode();
        ValueNode(Value v);
        ValueNode(ArrayValue v);
        ValueNode(std::string filename);
        ValueNode(const ValueNode& rNode);
        virtual ~ValueNode();
        Node* Clone();
        virtual void Accept(NodeVisitor& rVisitor);
        SymbolTable::SymbolInfo::SymbolType GetType() { return m_Type; }
        Value GetValue() { return m_Value; }
        void SetValue(Value v) { m_Type = SymbolTable::SymbolInfo::ATOMIC; m_Value = v; }
        ArrayValue& GetArrayValue() { return m_ArrayValue; }
        void SetArrayValue(ArrayValue v) { m_Type = SymbolTable::SymbolInfo::ARRAY; m_ArrayValue = v; }
        std::string GetFile() { return m_File; }
        void SetFile(std::string file) { m_Type = SymbolTable::SymbolInfo::FILE; m_File = file; }

    private:
        SymbolTable::SymbolInfo::SymbolType m_Type;
        ArrayValue m_ArrayValue;
        Value m_Value;
        std::string m_File;
    };
};

