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

    enum Type
    {
        NONE,
        DEBUG
    };

    // Constructor
    Log();

    // Destructor
    ~Log();

    void SetAction(Action action) { m_Action = action; }
    void SetType(Type type) { m_Type = type; }
    void SetFile(std::string file) { m_File = file; }

    // Adds a message.
    void AddMessage(const std::string msg);
    void AddMessage(Type type, const std::string msg);
    void AddNoNewlineMessage(const std::string msg);
    void AddNoNewlineMessage(Type type, const std::string msg);

    // Gets the singleton
    static Log* GetInst();

private:
    const char* DEFAULT_FILENAME = "InterpreterLog.log";
    static Log* m_pInst;
    Action m_Action;
    Type m_Type;
    std::string m_File;
    FILE* m_pFile;
};

};

