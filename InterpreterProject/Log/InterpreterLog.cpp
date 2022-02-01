#include <stdio.h>
#include <cstdarg>
#include <assert.h>
#include "InterpreterLog.h"
#include "InterpreterContext.h"
#include "DebugMemory/DebugMemory.h"

namespace Interpreter
{
// Singleton instance
Log* Log::m_pInst = nullptr;

Log::Log() :
    m_Action(STDOUT_AND_FILE),
    m_Type(NONE),
    m_File(DEFAULT_FILENAME),
    m_pFile(nullptr)
{
    fopen_s(&m_pFile, m_File.c_str(), "w");
    assert(m_pFile != nullptr);
}

Log::~Log()
{
    if (m_pFile != nullptr)
    {
        fclose(m_pFile);
    }
}

void Log::AddMessage(const std::string msg)
{
    std::string s = msg + '\n';

    if (m_Action == STDOUT_ONLY || m_Action == STDOUT_AND_FILE)
    {
        printf(s.c_str());
    }

    if (m_Action == FILE_ONLY || m_Action == STDOUT_AND_FILE)
    {
        fprintf(m_pFile, s.c_str());
    }
}

void Log::AddMessage(Type type, const std::string msg)
{
    if (m_Type == type)
    {
        std::string newmsg = msg;
        if (type == DEBUG)
        {
            newmsg = "DEBUG:  " + msg;
        }
        AddMessage(newmsg);
    }
}

void Log::AddNoNewlineMessage(const std::string msg)
{
    if (m_Action == STDOUT_ONLY || m_Action == STDOUT_AND_FILE)
    {
        printf(msg.c_str());
    }

    if (m_Action == FILE_ONLY || m_Action == STDOUT_AND_FILE)
    {
        fprintf(m_pFile, msg.c_str());
    }
}

Log* Log::GetInst()
{
    if (m_pInst == nullptr)
    {
        m_pInst = new Log();
        assert(m_pInst != nullptr);
    }

    return m_pInst;
}

void Log::Shutdown()
{
    if (m_pInst != nullptr)
    {
        fclose(m_pInst->m_pFile);
        delete m_pInst;
        m_pInst = nullptr;
    }
}

void Log::AddNoNewlineMessage(Type type, const std::string msg)
{
    if (type == m_Type)
    {
        AddNoNewlineMessage(msg);
    }
}

};

