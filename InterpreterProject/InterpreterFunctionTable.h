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
    bool Add(std::string name);
    bool IsPresent(std::string name);
    void Rem(std::string name);
    bool AttachDefinition(std::string name, FunctionDefNode* f);
    FunctionDefNode* Lookup(std::string name);
    void ClearDefinitions();
    void ClearNames();
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

