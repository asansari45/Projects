#pragma once

#include <map>
#include <string>
#include <stdio.h>
#include "Values/InterpreterValue.h"
#include "Values/InterpreterArrayValue.h"
#include "Tables/InterpreterSymbolTable.h"

namespace Interpreter
{
    class File
    {
    public:
        static std::optional<File*> Open(std::string name, std::string fileName, std::string mode);
        static std::optional<File*> Get(std::string name);
        void Close();
        bool Write(Value v);
        bool Write(ArrayValue v);
        bool Read(SymbolTable::SymbolInfo::SymbolType& rType, Value& rValue, ArrayValue& rArrayValue);
        bool Eof();

    private:
        File(std::string name, FILE* pFile):
            m_Name(name),
            m_pFile(pFile)
        {
        }
 
        ~File()
        {
        }

        // The file to read/write.
        std::string m_Name;
        FILE* m_pFile;

        enum Type
        {
            INTEGER = 1,
            FLOAT   = 2,
            STRING  = 3
        };

        struct Header
        {
            SymbolTable::SymbolInfo::SymbolType m_Type;
            Type m_SubType;
            int m_Dims[3];
        };

        typedef std::map<std::string, File*> FILEMAP;
        static FILEMAP m_FileMap;
    };
}