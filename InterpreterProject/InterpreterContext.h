#pragma once
#include <string>
#include <vector>

namespace Interpreter
{
    class Context
    {
    public:
        Context() :
            m_File(),
            m_Line(0),
            m_Column(0)
        {
        }

        ~Context()
        {
        }
        std::string GetInput() { return m_Input; }
        void SetInput(std::string input) { m_Input = input; }
 
        std::string GetFile() { return m_File; }
        void SetFile(std::string file) { m_File = file; }

        int GetLine() { return m_Line; }
        void SetLine(int line) { m_Line = line; }
        void IncLine() { m_Line++; }

        int GetColumn() { return m_Column; }
        void SetColumn(int column) { m_Column = column; }
        void IncColumn(int column) { m_Column += column; }

    private:
        std::string m_Input;
        std::string m_File;
        int m_Line;
        int m_Column;
    };

    extern std::vector<Context*> contextStack;
};

