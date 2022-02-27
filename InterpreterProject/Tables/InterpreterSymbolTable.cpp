#include <assert.h>
#include "InterpreterSymbolTable.h"
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

static void FillDebugBuf(SymbolTable::SymbolInfo info, char* buf, int len)
{
    if (info.m_IsRef)
    {
        sprintf_s(buf, len, "INFO isRef=%d, tName=%s, tTable=%s",
                  info.m_IsRef,
                  info.m_RefName.c_str(),
                  info.m_pRefTable->GetName().c_str());
    }
    else
    {
        std::string s;
        if (info.m_Type == SymbolTable::SymbolInfo::ARRAY)
        {
            s = "Type=ARRAY " + info.m_ArrayValue.GetRepresentation();
        }
        else if (info.m_Type == SymbolTable::SymbolInfo::ATOMIC)
        {
            s = "Type=ATOMIC " + info.m_Value.GetRepresentation();
        }
        else
        {
            assert(info.m_Type == SymbolTable::SymbolInfo::FILE);
            s = "TYPE=FILE";
        }

        sprintf_s(buf, len, "%s", s.c_str());
    }
}

bool SymbolTable::CreateSymbol(std::string name, SymbolInfo info)
{
#if defined(SYMTABLE_DEBUG)
    char sinfo[256];
    FillDebugBuf(info, sinfo, sizeof(sinfo));

    char buf[256];
    sprintf_s(buf, sizeof(buf), "SymbolTable %s:  CreateSymbol name=%s %s", m_Name.c_str(), name.c_str(), sinfo);

    Log::GetInst()->AddMessage(Log::DEBUG, buf);
#endif

    if (m_SymbolMap.find(name) == m_SymbolMap.end())
    {
        info.m_pTable = this;
        m_SymbolMap.insert(std::make_pair(name, info));
        return true;
    }

    return false;
}


std::optional<SymbolTable::SymbolInfo> SymbolTable::ReadSymbol(std::string name, bool getroot)
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
            if (i->second.m_IsRef)
            {
                return i->second.m_pRefTable->ReadSymbol(i->second.m_RefName, getroot);
            }
        }

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
#if defined(SYMTABLE_DEBUG)
    char buf[512];
    sprintf_s(buf, sizeof(buf), "SymbolTable %s:  UpdateSymbol name=%s", m_Name.c_str(), name.c_str());
    Log::GetInst()->AddMessage(Log::DEBUG, buf);
    FillDebugBuf(info, buf, sizeof(buf));
    Log::GetInst()->AddMessage(Log::DEBUG, buf);
#endif

    SYMBOL_MAP::iterator i = m_SymbolMap.find(name);
    if (i != m_SymbolMap.end())
    {
        m_SymbolMap.erase(i);
        info.m_pTable = this;
        m_SymbolMap.insert(std::make_pair(name, info));
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
            sprintf_s(buf, sizeof(buf), "REF %s==>%s::%s", p.m_Name.c_str(), 
                                                           p.m_pRefTable->GetName().c_str(),
                                                           p.m_RefName.c_str());
            Log::GetInst()->AddMessage(buf);
        }
        else if (p.m_Type == SymbolInfo::ARRAY)
        {
            std::vector<int> dims = p.m_ArrayValue.GetDims();
            if (dims.size() == 3)
            {
                sprintf_s(buf, sizeof(buf), "%s=dim[%d,%d,%d]  %s", p.m_Name.c_str(), dims[0], dims[1], dims[2], p.m_ArrayValue.GetRepresentation().c_str());
                Log::GetInst()->AddMessage(buf);
            }
            else if (dims.size() == 2)
            {
                sprintf_s(buf, sizeof(buf), "%s=dim[%d,%d]  %s", p.m_Name.c_str(), dims[0], dims[1], p.m_ArrayValue.GetRepresentation().c_str());
                Log::GetInst()->AddMessage(buf);
            }
            else if (dims.size() == 1)
            {
                sprintf_s(buf, sizeof(buf), "%s=dim[%d]  %s", p.m_Name.c_str(), dims[0], p.m_ArrayValue.GetRepresentation().c_str());
                Log::GetInst()->AddMessage(buf);
            }
        }
        else
        {
            sprintf_s(buf, sizeof(buf), "%s  %s", p.m_Name.c_str(), p.m_Value.GetRepresentation().c_str());
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

    m_SymbolMap.clear();
}

SymbolTable* SymbolTable::Clone()
{
    SymbolTable* pClone = new SymbolTable(m_Name);
    assert(pClone != nullptr);

    for (SYMBOL_MAP::iterator i = m_SymbolMap.begin(); i != m_SymbolMap.end(); i++)
    {
        bool status = pClone->CreateSymbol(i->first, i->second);
        assert(status);
    }
    return pClone;
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
