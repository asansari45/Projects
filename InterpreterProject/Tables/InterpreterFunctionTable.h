#pragma once

#include <map>
#include <string>

namespace Interpreter
{

class FunctionDefNode;

class FunctionTable
{
public:
    static FunctionTable* GetInst();
    bool CreateFunction(std::string name, FunctionDefNode* f);
    bool IsPresent(std::string name);
    void DeleteFunction(std::string name);
    FunctionDefNode* ReadFunction(std::string name);
    void Clear();
    void Dump();
private:
    FunctionTable()
    {
    }

    ~FunctionTable()
    {
    }

    std::map<std::string, FunctionDefNode*> m_Map;
    static FunctionTable* m_pInst;
};

};

