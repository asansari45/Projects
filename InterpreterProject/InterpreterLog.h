#pragma once
#include <string>
#include <map>

namespace Interpreter
{

class Context;

class Log
{
public:
    // Different logging actions.
    enum Action
    {
        STDOUT_ONLY,
        FILE_ONLY,
        STDOUT_AND_FILE
    };

    // Constructor
    Log();

    // Destructor
    ~Log();

    void SetAction(Action action) { m_Action = action; }
    void Setfile(std::string file) { m_File = file; }

    // Adds a message.
    void AddMessage(const std::string msg);
    void AddNoNewlineMessage(const std::string msg);

    // Gets the singleton
    static Log* GetInst();

private:
    const char* DEFAULT_FILENAME = "InterpreterLog.log";
    static Log* m_pInst;
    Action m_Action;
    std::string m_File;
    FILE* m_pFile;
};

};

