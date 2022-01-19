#include <assert.h>
#include "InterpreterSymbolTable.hpp"
#include "InterpreterLog.h"

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

static void FillDebugBuf(SymbolTable::SymbolInfo info, char* buf)
{
    if (info.m_IsRef)
    {
        sprintf_s(buf, 512, "INFO isRef=%d, tName=%s, tTable=%s",
            info.m_IsRef,
            info.m_TargetName.c_str(),
            info.m_pTargetTable->GetName().c_str());
    }
    else
    {
        sprintf_s(buf, 512, "INFO isArray=%d", info.m_IsArray);
    }
}

bool SymbolTable::CreateSymbol(std::string name, SymbolInfo info)
{
    char buf[512];
    sprintf_s(buf, sizeof(buf), "SymbolTable %s:  CreateSymbol name=%s", m_Name.c_str(), name.c_str());
    Log::GetInst()->AddMessage(Log::DEBUG, buf);
    FillDebugBuf(info, buf);
    Log::GetInst()->AddMessage(Log::DEBUG, buf);

    if (m_SymbolMap.find(name) == m_SymbolMap.end())
    {
        m_SymbolMap.insert(std::make_pair(name, info));
        return true;
    }

    return false;
}


std::optional<SymbolTable::SymbolInfo> SymbolTable::ReadSymbol(std::string name)
{
    char buf[512];
    sprintf_s(buf, sizeof(buf), "SymbolTable %s:  ReadSymbol name=%s", m_Name.c_str(), name.c_str());
    Log::GetInst()->AddMessage(Log::DEBUG, buf);

    SYMBOL_MAP::iterator i = m_SymbolMap.find(name);
    if (i != m_SymbolMap.end())
    {
        return i->second;
    }

    return {};
}

bool SymbolTable::IsSymbolPresent(std::string name)
{
    std::optional<SymbolInfo> s = ReadSymbol(name);
    return s != std::nullopt;
}

bool SymbolTable::UpdateSymbol(std::string name, SymbolInfo info)
{
    char buf[512];
    sprintf_s(buf, sizeof(buf), "SymbolTable %s:  UpdateSymbol name=%s", m_Name.c_str(), name.c_str());
    Log::GetInst()->AddMessage(Log::DEBUG, buf);
    FillDebugBuf(info, buf);
    Log::GetInst()->AddMessage(Log::DEBUG, buf);

    SYMBOL_MAP::iterator i = m_SymbolMap.find(name);
    if (i != m_SymbolMap.end())
    {
        m_SymbolMap.erase(i);
        m_SymbolMap.insert(std::make_pair(name, info));
        return true;
    }

    return false;
}

void SymbolTable::DeleteSymbol(std::string name)
{
    char buf[512];
    sprintf_s(buf, sizeof(buf), "SymbolTable %s:  DeleteSymbol name=%s", m_Name.c_str(), name.c_str());
    Log::GetInst()->AddMessage(Log::DEBUG, buf);

    SYMBOL_MAP::iterator i = m_SymbolMap.find(name);
    if (i != m_SymbolMap.end())
    {
        m_SymbolMap.erase(i);
    }
}

void SymbolTable::Dump()
{
    char buf[512];
    sprintf_s(buf, sizeof(buf), "%s Symbol Table Contents", m_Name.c_str());
    Log::GetInst()->AddMessage(buf);

    for (SYMBOL_MAP::iterator i = m_SymbolMap.begin();
         i != m_SymbolMap.end();
         i++)
    {
        SymbolInfo& p = i->second;
        if (p.m_IsRef)
        {
            sprintf_s(buf, sizeof(buf), "REF %s==>%s", p.m_Name.c_str(), p.m_TargetName.c_str());
            Log::GetInst()->AddMessage(buf);
        }
        else if (p.m_IsArray)
        {
            std::vector<int> dims = p.m_ArrayValue.GetDims();
            if (dims.size() == 3)
            {
                sprintf_s(buf, sizeof(buf), "%s=dim[%d,%d,%d]", p.m_Name.c_str(), dims[0], dims[1], dims[2]);
                Log::GetInst()->AddMessage(buf);
            }
            else if (dims.size() == 2)
            {
                sprintf_s(buf, sizeof(buf), "%s=dim[%d,%d]", p.m_Name.c_str(), dims[0], dims[1]);
                Log::GetInst()->AddMessage(buf);
            }
            else if (dims.size() == 1)
            {
                sprintf_s(buf, sizeof(buf), "%s=dim[%d]", p.m_Name.c_str(), dims[0]);
                Log::GetInst()->AddMessage(buf);
            }
        }
        else
        {
            sprintf_s(buf, sizeof(buf), "%s", p.m_Name.c_str());
            Log::GetInst()->AddMessage(buf);
        }
    }
}

void SymbolTable::Clear()
{
    m_SymbolMap.clear();
}

SymbolTable* SymbolTable::CreateGlobalSymbols()
{
    SymbolTable* pSymbolTable = new SymbolTable("GLOBAL");
    assert(pSymbolTable != nullptr);

    SymbolInfo info;
    info.m_Value.SetIntValue(0);
    info.m_Name = "main";
    bool status = pSymbolTable->CreateSymbol("main", info);
    assert(status);
    return pSymbolTable;
}

void SymbolTable::DeleteGlobalSymbols(SymbolTable* pGlobalSymbols)
{
    delete pGlobalSymbols;
}

};
