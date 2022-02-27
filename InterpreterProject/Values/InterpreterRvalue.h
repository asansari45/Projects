#pragma once

#include "InterpreterValue.h"
#include "InterpreterArrayValue.h"
#include "Nodes/InterpreterValueNode.h"
#include "Tables/InterpreterSymbolTable.h"
#include "File/InterpreterFile.h"

namespace Interpreter
{
    class Rvalue
    {
    public:
        Rvalue(ArrayValue& rArrayValue) :
            m_Type(SymbolTable::SymbolInfo::ARRAY),
            m_Value(),
            m_ArrayValue(rArrayValue),
            m_File()
        {
        }

        Rvalue(Value& rValue) :
            m_Type(SymbolTable::SymbolInfo::ATOMIC),
            m_Value(rValue),
            m_ArrayValue(),
            m_File()
        {
        }

        Rvalue(File* pFile) :
            m_Type(SymbolTable::SymbolInfo::FILE),
            m_Value(),
            m_ArrayValue(),
            m_File()
        {
        }

        Rvalue(ValueNode* pValueNode) :
            m_Type(pValueNode->GetType()),
            m_Value(pValueNode->GetValue()),
            m_ArrayValue(pValueNode->GetArrayValue()),
            m_File(pValueNode->GetFile())
        {
        }

        ~Rvalue()
        {
        }

        SymbolTable::SymbolInfo::SymbolType GetType() const { return m_Type; }
        ArrayValue& GetArrayValue() { return m_ArrayValue; }
        Value& GetValue() { return m_Value; }
        std::string GetFile() { return m_File; }

    private:
        SymbolTable::SymbolInfo::SymbolType m_Type;
        Value m_Value;
        ArrayValue m_ArrayValue;
        std::string m_File;
    };
}


