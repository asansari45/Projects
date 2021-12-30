#include "InterpreterFunctionTable.h"
#include "InterpreterNode.hpp"
#include "InterpreterLog.h"

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

    void FunctionTable::ClearDefinitions()
    {
        std::vector<std::string> toBeDeleted;
        for (std::map<std::string, FunctionDefNode*>::iterator i = m_Map.begin();
            i != m_Map.end();
            i++)
        {
            if (i->second != nullptr)
            {
                toBeDeleted.push_back(i->first);
            }
        }

        for (size_t i = 0; i < toBeDeleted.size(); i++)
        {
            std::map<std::string, FunctionDefNode*>::iterator j = m_Map.find(toBeDeleted[i]);
            assert(j != m_Map.end());
            delete j->second;
            m_Map.erase(j);
        }
    }

    void FunctionTable::ClearNames()
    {
        std::vector<std::string> toBeDeleted;
        for (std::map<std::string, FunctionDefNode*>::iterator i = m_Map.begin();
            i != m_Map.end();
            i++)
        {
            if (i->second == nullptr)
            {
                toBeDeleted.push_back(i->first);
            }
        }

        for (size_t i = 0; i < toBeDeleted.size(); i++)
        {
            std::map<std::string, FunctionDefNode*>::iterator j = m_Map.find(toBeDeleted[i]);
            assert(j != m_Map.end());
            m_Map.erase(j);
        }
    }

    void FunctionTable::Dump()
    {
        for (std::map<std::string, FunctionDefNode*>::iterator i = m_Map.begin();
            i != m_Map.end();
            i++)
        {
            char buf[512];
            FunctionDefNode* p = i->second;
            sprintf_s(buf, sizeof(buf), "function %s(", p->GetNameVar()->GetName().c_str());
            char params[512];
            for (VarNode* pNode = p->GetInputVars(); pNode; pNode=dynamic_cast<VarNode*>(pNode->GetNext()))
            {
                if (pNode->GetNext() != nullptr)
                {
                    sprintf_s(params, sizeof(params), "%s,", pNode->GetName().c_str());
                }
                else
                {
                    sprintf_s(params, sizeof(params), "%s", pNode->GetName().c_str());
                }
                strcat_s(buf, params);
            }
            strcat_s(buf, ")");
            Log::GetInst()->AddMessage(buf);
        }
    }
};
