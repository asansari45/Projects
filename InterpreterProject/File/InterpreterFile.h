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
        // static File* Open(std::string name, std::string fileName, std::string mode);
        // static File* Open(std::string name, std::string fileName, std::string mode);
        static File* Open(std::string fileName, std::string mode);
        static File* Get(std::string name);
        void Close();
        bool Write(Value v);
        bool Write(ArrayValue* v);
        bool Read(bool& rArray, Value& rValue, ArrayValue** ppArrayValue);
        bool Eof();

    private:
        File(FILE* pFile):
            m_pFile(pFile)
        {
        }
 
        ~File()
        {
        }

        // The file to read/write.
        FILE* m_pFile;

        enum Type
        {
            INTEGER = 1,
            FLOAT   = 2,
            STRING  = 3
        };

        struct Header
        {
            uint32_t m_Array;
            uint32_t m_SubType;
            uint32_t m_Dims[3];
        };

        typedef std::map<std::string, File*> FILEMAP;
        static FILEMAP m_FileMap;
    };
}