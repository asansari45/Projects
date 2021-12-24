#include "InterpreterFunctionTable.h"
#include "InterpreterNode.hpp"

namespace Interpreter
{
    FunctionTable* FunctionTable::m_pInst = nullptr;
    FunctionTable* FunctionTable::GetInst()
    {
        if (m_pInst == nullptr)
        {
            m_pInst = new FunctionTable;
        }
        return m_pInst;
    }

    bool FunctionTable::Add(std::string name)
    {
        std::map<std::string, FunctionDefNode*>::iterator i = m_Map.find(name);
        if (i != m_Map.end())
        {
            // Already in there.
            return false;
        }

        // Just insert the name into the map for now.
        m_Map.insert(std::make_pair(name, nullptr));
        return true;
    }

    bool FunctionTable::IsPresent(std::string name)
    {
        return m_Map.find(name) != m_Map.end();
    }

    void FunctionTable::Rem(std::string name)
    {
        std::map<std::string, FunctionDefNode*>::iterator i = m_Map.find(name);
        if (i != m_Map.end())
        {
            m_Map.erase(i);
        }
    }

    bool FunctionTable::AttachDefinition(std::string name, FunctionDefNode* f)
    {
        std::map<std::string, FunctionDefNode*>::iterator i = m_Map.find(name);
        if (i != m_Map.end())
        {
            if (i->second != nullptr)
            {
                return false;
            }

            i->second = f;
            return true;
        }

        return false;
    }

    FunctionDefNode* FunctionTable::Lookup(std::string name)
    {
        std::map<std::string, FunctionDefNode*>::iterator i = m_Map.find(name);
        if (i != m_Map.end())
        {
            return i->second;
        }

        return nullptr;
    }

    void FunctionTable::Clear()
    {
        for (std::map<std::string, FunctionDefNode*>::iterator i = m_Map.begin();
            i != m_Map.end();
            i++)
        {
            delete i->second;

        }
        m_Map.clear();
    }
};
