#include <assert.h>
#include "InterpreterSymbolTable.hpp"
#include "InterpreterLog.h"

namespace Interpreter
{

SymbolTable::SymbolTable() :
    m_SymbolMap()
{
}

SymbolTable::~SymbolTable()
{
}

bool SymbolTable::AddSymbol(const std::string name, Value value)
{
    if (m_SymbolMap.find(name) == m_SymbolMap.end())
    {
        SymbolInfo* pSymbolInfo = new SymbolInfo;
        pSymbolInfo->m_Name = name;
        pSymbolInfo->m_IsArray = false;
        pSymbolInfo->m_Value = value;
        m_SymbolMap.insert(std::make_pair(name, pSymbolInfo));
        return true;
    }

    return false;
}

bool SymbolTable::AddSymbol(const std::string name, ArrayValue value)
{
    if (m_SymbolMap.find(name) == m_SymbolMap.end())
    {
        SymbolInfo* pSymbolInfo = new SymbolInfo;
        pSymbolInfo->m_Name = name;
        pSymbolInfo->m_IsArray = true;
        pSymbolInfo->m_ArrayValue = value;
        m_SymbolMap.insert(std::make_pair(name, pSymbolInfo));
        return true;
    }

    return false;
}

bool SymbolTable::ChangeSymbol(const std::string symbol, Value value, const std::vector<int>* pElement)
{
    std::map<std::string, SymbolInfo*>::iterator i = m_SymbolMap.find(symbol);
    if (i == m_SymbolMap.end())
    {
        return false;
    }

    SymbolInfo* pSymbolInfo = i->second;
    if (pSymbolInfo->m_IsArray)
    {
        // The symbol is currently an array.
        if (pElement != nullptr)
        {
            // An element is specified.
            return pSymbolInfo->m_ArrayValue.SetValue(*pElement, value);
        }

        // No element is specified.  Let's change the element be a discrete element.
        pSymbolInfo->m_IsArray = false;
        pSymbolInfo->m_Value = value;
        return true;
    }

    // Not an array
    if (pElement != nullptr)
    {
        // A specific element is asked for a non-array.  Error.
        return false;
    }

    pSymbolInfo->m_Value = value;
    return true;
}

bool SymbolTable::ChangeSymbol(const std::string symbol, ArrayValue value)
{
    std::map<std::string, SymbolInfo*>::iterator i = m_SymbolMap.find(symbol);
    if (i == m_SymbolMap.end())
    {
        return false;
    }

    SymbolInfo* pSymbolInfo = i->second;
    pSymbolInfo->m_IsArray = true;
    pSymbolInfo->m_ArrayValue = value;
    return true;
}

bool SymbolTable::DelSymbol(const std::string symbol)
{
    std::map<std::string, SymbolInfo*>::iterator i = m_SymbolMap.find(symbol);
    if (i != m_SymbolMap.end())
    {
        delete i->second;
        m_SymbolMap.erase(i);
        return true;
    }

    return false;
}

std::optional<Value> SymbolTable::GetSymbolValue(const std::string name, const std::vector<int>* pElement)
{
    std::map<std::string, SymbolInfo*>::iterator i = m_SymbolMap.find(name);
    if (i == m_SymbolMap.end())
    {
        return {};
    }

    SymbolInfo* pInfo = i->second;
    if (i->second->m_IsArray && pElement == nullptr)
    {
        return {};
    }

    if (i->second->m_IsArray == false && pElement != nullptr)
    {
        return {};
    }

    if ( !i->second->m_IsArray)
    {
        return pInfo->m_Value;
    }

    return pInfo->m_ArrayValue.GetValue(*pElement);
}

std::optional<ArrayValue> SymbolTable::GetArraySymbolValue(const std::string name)
{
    std::map<std::string, SymbolInfo*>::iterator i = m_SymbolMap.find(name);
    if (i == m_SymbolMap.end())
    {
        return {};
    }

    SymbolInfo* pInfo = i->second;
    if (!i->second->m_IsArray)
    {
        return {};
    }

    return pInfo->m_ArrayValue;
}

std::optional<std::type_index> SymbolTable::GetSymbolType(const std::string name)
{
    std::map<std::string, SymbolInfo*>::iterator i = m_SymbolMap.find(name);
    if (i != m_SymbolMap.end())
    {
        return i->second->m_Value.GetType();
    }

    return {};
}

bool SymbolTable::IsSymbolArray(const std::string name)
{
    std::map<std::string, SymbolInfo*>::iterator i = m_SymbolMap.find(name);
    if (i != m_SymbolMap.end())
    {
        return i->second->m_IsArray;
    }

    return false;
}

std::optional<std::vector<int> > SymbolTable::GetSymbolDims(const std::string name)
{
    std::map<std::string, SymbolInfo*>::iterator i = m_SymbolMap.find(name);
    if (i != m_SymbolMap.end())
    {
        return i->second->m_ArrayValue.GetDims();
    }

    return {};
}

bool SymbolTable::IsSymbolPresent(const std::string name)
{
    return m_SymbolMap.find(name) != m_SymbolMap.end();
}
        
void SymbolTable::Dump()
{
    for (std::map<std::string, SymbolInfo*>::iterator i = m_SymbolMap.begin();
        i != m_SymbolMap.end();
        i++)
    {
        SymbolInfo* p = i->second;
        if (p->m_IsArray)
        {
            std::vector<int> dims = p->m_ArrayValue.GetDims();
            char buf[512];
            if (dims.size() == 3)
            {
                sprintf_s(buf, sizeof(buf), "%s=dim[%d,%d,%d]", p->m_Name.c_str(), dims[0], dims[1], dims[2]);
                Log::GetInst()->AddMessage(buf);
            }
            else if (dims.size() == 2)
            {
                sprintf_s(buf, sizeof(buf), "%s=dim[%d,%d]", p->m_Name.c_str(), dims[0], dims[1]);
                Log::GetInst()->AddMessage(buf);
            }
            else if (dims.size() == 1)
            {
                sprintf_s(buf, sizeof(buf), "%s=dim[%d]", p->m_Name.c_str(), dims[0]);
                Log::GetInst()->AddMessage(buf);
            }
        }
        else
        {
            char buf[512];
            sprintf_s(buf, sizeof(buf), "%s", p->m_Name.c_str());
            Log::GetInst()->AddMessage(buf);
        }
    }
}

void SymbolTable::Clear()
{
    for (std::map<std::string, SymbolInfo*>::iterator i = m_SymbolMap.begin();
        i != m_SymbolMap.end();
        i++)
    {
        delete i->second;
    }
    m_SymbolMap.clear();
}

SymbolTable* SymbolTable::CreateGlobalSymbols()
{
    SymbolTable* pSymbolTable = new SymbolTable();
    assert(pSymbolTable != nullptr);

    Value v;
    bool status = pSymbolTable->AddSymbol("main", v);
    assert(status);
    return pSymbolTable;
}

void SymbolTable::DeleteGlobalSymbols(SymbolTable* pGlobalSymbols)
{
    delete pGlobalSymbols;
}

};
