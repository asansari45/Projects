#include "InterpreterFunctionTable.h"
#include "Nodes/InterpreterFunctionDefNode.h"
#include "Nodes/InterpreterRefNode.h"
#include "Nodes/InterpreterVarNode.h"
#include "Log/InterpreterLog.h"

#include "DebugMemory/DebugMemory.h"

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

    void FunctionTable::Shutdown()
    {
        if (m_pInst != nullptr)
        {
            m_pInst->Clear();
            delete m_pInst;
        }
    }

    bool FunctionTable::CreateFunction(std::string name, FunctionDefNode* pDef)
    {
        std::map<std::string, FunctionDefNode*>::iterator i = m_Map.find(name);
        if (i != m_Map.end())
        {
            // Already in there.
            return false;
        }

        m_Map.insert(std::make_pair(name, pDef));
        return true;
    }

    bool FunctionTable::IsPresent(std::string name)
    {
        return ReadFunction(name) != nullptr;
    }

    void FunctionTable::DeleteFunction(std::string name)
    {
        std::map<std::string, FunctionDefNode*>::iterator i = m_Map.find(name);
        if (i != m_Map.end())
        {
            delete i->second;
            m_Map.erase(i);
        }
    }

    FunctionDefNode* FunctionTable::ReadFunction(std::string name)
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

    void FunctionTable::Dump()
    {
        for (std::map<std::string, FunctionDefNode*>::iterator i = m_Map.begin();
            i != m_Map.end();
            i++)
        {
            char buf[512];
            FunctionDefNode* p = i->second;
            sprintf_s(buf, sizeof(buf), "function %s(", p->GetName().c_str());
            char params[512];
            for (Node* pNode = p->GetInputVars(); pNode; pNode=pNode->GetNext())
            {
                bool ref = false;
                std::string name;
                RefNode* pRefNode = dynamic_cast<RefNode*>(pNode);
                if (pRefNode != nullptr)
                {
                    ref = true;
                    name = pRefNode->GetName();
                }
                else
                {
                    VarNode* pVarNode = dynamic_cast<VarNode*>(pNode);
                    assert(pVarNode != nullptr);
                    name = pVarNode->GetName();
                }
                if (pNode->GetNext() != nullptr)
                {
                    if (ref)
                    {
                        sprintf_s(params, sizeof(params), "&%s,", name.c_str());
                    }
                    else
                    {
                        sprintf_s(params, sizeof(params), "%s,", name.c_str());
                    }
                }
                else
                {
                    if (ref)
                    {
                        sprintf_s(params, sizeof(params), "&%s", name.c_str());
                    }
                    else
                    {
                        sprintf_s(params, sizeof(params), "%s", name.c_str());
                    }
                }
                strcat_s(buf, params);
            }
            strcat_s(buf, ")");
            Log::GetInst()->AddMessage(buf);
        }
    }
};
