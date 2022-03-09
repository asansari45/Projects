#include <assert.h>
#include "Tables/InterpreterSymbolTable.h"
#include "Log/InterpreterLog.h"

#include "DebugMemory/DebugMemory.h"

#undef SYMTABLE_DEBUG

namespace Interpreter
{

SymbolTable::SymbolTable(const std::string name) :
    m_SymbolMap(),
    m_Name(name)
{
}

SymbolTable::SymbolTable(SymbolTable& rProto) :
    m_SymbolMap(rProto.m_SymbolMap),
    m_Name(rProto.m_Name)
{
}

SymbolTable::~SymbolTable()
{
}

static void FillDebugBuf(SymbolTable::SymbolInfo* pInfo, char* buf, int len)
{
    if (pInfo->m_IsRef)
    {
        sprintf_s(buf, len, "INFO isRef=%d, tName=%s, tTable=%s",
                  pInfo->m_IsRef,
                  pInfo->m_RefName.c_str(),
                  pInfo->m_pRefTable->GetName().c_str());
    }
    else
    {
        std::string s;
        if (pInfo->m_IsArray)
        {
            s = pInfo->m_pArrayValue->GetTypeRepr() + " " + pInfo->m_pArrayValue->GetDimsRepr() + " " + pInfo->m_pArrayValue->GetValuesRepr();
        }
        else
        {
            s = pInfo->m_Value.GetTypeRepr() + " " = pInfo->m_Value.GetValueRepr();
        }

        sprintf_s(buf, len, "INFO isArray=%d %s", pInfo->m_IsArray, s.c_str());
    }
}

bool SymbolTable::CreateSymbol(std::string name, SymbolInfo* pInfo)
{
#if defined(SYMTABLE_DEBUG)
    char sinfo[256];
    FillDebugBuf(pInfo, sinfo, sizeof(sinfo));

    char buf[256];
    sprintf_s(buf, sizeof(buf), "SymbolTable %s:  CreateSymbol name=%s %s", m_Name.c_str(), name.c_str(), sinfo);

    Log::GetInst()->AddMessage(Log::DEBUG, buf);
#endif

    if (m_SymbolMap.find(name) == m_SymbolMap.end())
    {
        pInfo->m_pTable = this;
        m_SymbolMap.insert(std::make_pair(name, pInfo));
        return true;
    }

    return false;
}


SymbolTable::SymbolInfo* SymbolTable::ReadSymbol(std::string name, bool getroot)
{
#if defined(SYMTABLE_DEBUG)
    char buf[512];
    sprintf_s(buf, sizeof(buf), "SymbolTable %s:  ReadSymbol name=%s", m_Name.c_str(), name.c_str());
    Log::GetInst()->AddMessage(Log::DEBUG, buf);
#endif

    SYMBOL_MAP::iterator i = m_SymbolMap.find(name);
    if (i != m_SymbolMap.end())
    {
        if (getroot)
        {
            if (i->second->m_IsRef)
            {
                return i->second->m_pRefTable->ReadSymbol(i->second->m_RefName, getroot);
            }
        }

        return i->second;
    }

    return nullptr;
}

bool SymbolTable::IsSymbolPresent(std::string name)
{
    return ReadSymbol(name) != nullptr;
}

bool SymbolTable::UpdateSymbol(std::string name, SymbolInfo* pInfo)
{
#if defined(SYMTABLE_DEBUG)
    char buf[512];
    sprintf_s(buf, sizeof(buf), "SymbolTable %s:  UpdateSymbol name=%s", m_Name.c_str(), name.c_str());
    Log::GetInst()->AddMessage(Log::DEBUG, buf);
    FillDebugBuf(pInfo, buf, sizeof(buf));
    Log::GetInst()->AddMessage(Log::DEBUG, buf);
#endif

    SYMBOL_MAP::iterator i = m_SymbolMap.find(name);
    if (i != m_SymbolMap.end())
    {
        if (i->second->m_IsArray)
        {
            delete i->second->m_pArrayValue;
        }
        m_SymbolMap.erase(i);
        pInfo->m_pTable = this;
        m_SymbolMap.insert(std::make_pair(name, pInfo));
        return true;
    }

    return false;
}

void SymbolTable::DeleteSymbol(std::string name)
{
#if defined(SYMTABLE_DEBUG)
    char buf[512];
    sprintf_s(buf, sizeof(buf), "SymbolTable %s:  DeleteSymbol name=%s", m_Name.c_str(), name.c_str());
    Log::GetInst()->AddMessage(Log::DEBUG, buf);
#endif

    SYMBOL_MAP::iterator i = m_SymbolMap.find(name);
    if (i != m_SymbolMap.end())
    {
        if (i->second->m_IsArray)
        {
            delete i->second->m_pArrayValue;
        }
        delete i->second;
        m_SymbolMap.erase(i);
    }
}

void SymbolTable::Dump()
{
    char buf[512];
    sprintf_s(buf, sizeof(buf), "Symbol Table Name:  %s", m_Name.c_str());
    Log::GetInst()->AddMessage(buf);

    // Process all the non-references
    Log::GetInst()->AddMessage("NAME                  TYPE   DIMS          VALUE(S)");
    Log::GetInst()->AddMessage("===================================================");
    for (SYMBOL_MAP::iterator i = m_SymbolMap.begin();
         i != m_SymbolMap.end();
         i++)
    {
        SymbolInfo* p = i->second;
        char buf[512];
        if (p->m_IsArray)
        {
            sprintf_s(buf, sizeof(buf), "%-20s  %-5s  %-12s  %s", p->m_Name.c_str(), 
                                                                  p->m_pArrayValue->GetTypeRepr().c_str(),
                                                                  p->m_pArrayValue->GetDimsRepr().c_str(),
                                                                  p->m_pArrayValue->GetValuesRepr().c_str());
        }
        else
        {
            sprintf_s(buf, sizeof(buf), "%-20s  %-5s  %-12s  %s", p->m_Name.c_str(),
                                                                  p->m_Value.GetTypeRepr().c_str(),
                                                                  "---",
                                                                  p->m_Value.GetValueRepr().c_str());
        }

        Log::GetInst()->AddMessage(buf);
    }

    // Process all the non-references
    Log::GetInst()->AddMessage("");
    Log::GetInst()->AddMessage("REFERENCER    REFERENCEE      REFERENCEE");
    Log::GetInst()->AddMessage("  NAME           TABLE           NAME");
    Log::GetInst()->AddMessage("========================================");
    for (SYMBOL_MAP::iterator i = m_SymbolMap.begin();
         i != m_SymbolMap.end();
         i++)
    {
        SymbolInfo* p = i->second;
        if (p->m_IsRef)
        {
            char buf[512];
            sprintf_s(buf, sizeof(buf), "%-20s  %-20s  %s", p->m_Name.c_str(), 
                                                            p->m_pRefTable->GetName().c_str(),
                                                            p->m_RefName.c_str());
            Log::GetInst()->AddMessage(buf);
        }
    }
}

void SymbolTable::Clear()
{
#if defined(SYMTABLE_DEBUG)
    char buf[512];
    sprintf_s(buf, sizeof(buf), "SymbolTable %s:  CLEAR", m_Name.c_str());
    Log::GetInst()->AddMessage(Log::DEBUG, buf);
#endif

    for (SYMBOL_MAP::iterator i = m_SymbolMap.begin(); i != m_SymbolMap.end(); i++)
    {
        if (i->second->m_IsArray)
        {
            delete i->second->m_pArrayValue;
        }
        delete i->second;
    }

    m_SymbolMap.clear();
}

SymbolTable* SymbolTable::Clone()
{
    SymbolTable* pClone = new SymbolTable(m_Name);
    assert(pClone != nullptr);

    for (SYMBOL_MAP::iterator i = m_SymbolMap.begin(); i != m_SymbolMap.end(); i++)
    {
        bool status = pClone->CreateSymbol(i->first, i->second->Clone());
        assert(status);
    }
    return pClone;
}

SymbolTable* SymbolTable::CreateGlobalSymbols()
{
    SymbolTable* pSymbolTable = new SymbolTable("GLOBAL");
    assert(pSymbolTable != nullptr);

    SymbolInfo* pInfo = new SymbolInfo;
    assert(pInfo != nullptr);

    pInfo->m_Value.SetIntValue(0);
    pInfo->m_Name = "main";
    bool status = pSymbolTable->CreateSymbol("main", pInfo);
    assert(status);
    return pSymbolTable;
}

void SymbolTable::DeleteGlobalSymbols(SymbolTable* pGlobalSymbols)
{
    pGlobalSymbols->Clear();
    delete pGlobalSymbols;
}

};
