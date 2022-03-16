#include <memory>
#include "InterpreterLvalues.h"
#include "Nodes/InterpreterValueNode.h"
#include "Visitors/InterpreterErrorInterface.h"

#include "DebugMemory/DebugMemory.h"

namespace Interpreter
{
    ArrayValue* Lvalue::ConvertValueNodes(int count, ValueNode* pList)
    {
        assert(pList != nullptr);
        std::vector<int> dims;
        dims.push_back(count);
        std::type_index t = pList->GetValueRef().GetType();
        std::unique_ptr<ArrayValue> pArrayValue(ArrayValue::Create(dims, t));

        int element = 0;
        for (ValueNode* pValueNode = pList; pValueNode != nullptr; pValueNode=dynamic_cast<ValueNode*>(pValueNode->GetNext()))
        {
            // Check for entire array.
            if (pValueNode->IsArray())
            {
                ErrorInterface::ErrorInfo err(pValueNode);
                err.m_Msg = m_pErrIf->ERROR_ARRAY_SPECIFIER_EXPECTED;
                m_pErrIf->SetErrorInfo(err);
                return nullptr;
            }

            dims.clear();
            dims.push_back(element);
            element++;
            bool status = pArrayValue->SetValue(dims, pValueNode->GetValue());
            if (!status)
            {
                ErrorInterface::ErrorInfo err(pValueNode);
                err.m_Msg = m_pErrIf->ERROR_INCORRECT_TYPE;
                m_pErrIf->SetErrorInfo(err);
                return nullptr;
            }
        }

        return pArrayValue.release();
    }

    // There is no symbol for the given rvalue in the symbol table.
    // Create it and set the value.
    // a = b where a does not exist, b is an array or value.
    // a = 1 where a does not exist, 1 is a value.
    void NoSymbolLvalue::Equ(ValueNode* pRvalue)
    {
        assert(m_pSymbolTable != nullptr);
        assert(m_pSymbolTable->IsSymbolPresent(m_Name) == false);
        SymbolTable::SymbolInfo* pInfo = new SymbolTable::SymbolInfo;
        pInfo->m_Name = m_Name;

        // No arrays specifiers when no symbol is specified.
        if (m_ArraySpecifier.size() != 0)
        {
            sprintf_s(m_Buf, sizeof(m_Buf), m_pErrIf->ERROR_MISSING_SYMBOL, m_Name.c_str());
            m_ErrInfo.m_Msg = m_Buf;
            
            m_pErrIf->SetErrorInfo(m_ErrInfo);
            delete pInfo;
            return;
        }

        bool status;
        if (pRvalue->IsArray())
        {
            pInfo->m_IsArray = true;
            pInfo->m_pArrayValue = pRvalue->GetArrayValue()->Clone();
            status = m_pSymbolTable->CreateSymbol(m_Name, pInfo);
        }
        else
        {
            pInfo->m_IsArray = false;
            pInfo->m_Value = pRvalue->GetValue();
            status = m_pSymbolTable->CreateSymbol(m_Name, pInfo);
        }
        assert(status);
    }

    // There is no symbol for the given:  a = {0,1,2,3...}
    // Create the symbol as an array of sizeof(pValueList).
    void NoSymbolLvalue::EquList(int listCount, ValueNode* pValueList)
    {
        // Convert the list into an array.
        std::unique_ptr<ArrayValue> pArrayValue(ConvertValueNodes(listCount, pValueList));

        // Create a symbol of that many elements.
        assert(m_pSymbolTable != nullptr);
        assert(m_pSymbolTable->IsSymbolPresent(m_Name) == false);
        std::unique_ptr<SymbolTable::SymbolInfo> pInfo(new SymbolTable::SymbolInfo);
        pInfo->m_Name = m_Name;
        pInfo->m_IsArray = true;
        pInfo->m_pArrayValue = pArrayValue.release();

        // No arrays specifiers when no symbol is specified.
        if (m_ArraySpecifier.size() != 0)
        {
            sprintf_s(m_Buf, sizeof(m_Buf), m_pErrIf->ERROR_MISSING_SYMBOL, m_Name.c_str());
            m_ErrInfo.m_Msg = m_Buf;
            
            m_pErrIf->SetErrorInfo(m_ErrInfo);
            return;
        }

        bool status = m_pSymbolTable->CreateSymbol(m_Name, pInfo.release());
        assert(status);
    }

    // There is no symbol for the given rvalue in the symbol table.
    // a = dim[q,r,s] where a does not exist.
    void NoSymbolLvalue::Dim(ValueNode* pRvalue)
    {
        assert(m_pSymbolTable != nullptr);
        assert(m_pSymbolTable->IsSymbolPresent(m_Name) == false);
        assert(pRvalue->IsArray());
        SymbolTable::SymbolInfo* pInfo = new SymbolTable::SymbolInfo;
        assert(pInfo != nullptr);
        pInfo->m_Name = m_Name;
        pInfo->m_IsArray = true;
        pInfo->m_pArrayValue = pRvalue->GetArrayValue()->Clone();
        bool status = m_pSymbolTable->CreateSymbol(m_Name, pInfo);
        assert(status);
    }

    // There is a symbol in the symbol table.  It is an array.
    // We are setting the value of one element here.
    // a[0,1,1] = 3 where a must exist or an error.
    // a[0,1,1] = b where a must exist and b must not be an array.
    void ArrayElementLvalue::Equ(ValueNode* pRvalue)
    {
        assert(m_pSymbolTable != nullptr);
        if (!m_pSymbolTable->IsSymbolPresent(m_Name))
        {
            sprintf_s(m_Buf, sizeof(m_Buf), m_pErrIf->ERROR_MISSING_SYMBOL, m_Name.c_str());
            m_ErrInfo.m_Msg = m_Buf;
            
            m_pErrIf->SetErrorInfo(m_ErrInfo);
            return;
        }

        SymbolTable::SymbolInfo* pInfo = m_pSymbolTable->ReadSymbol(m_Name);
        assert(pInfo != nullptr);

        if (!pInfo->m_IsArray)
        {
            sprintf_s(m_Buf, sizeof(m_Buf), m_pErrIf->ERROR_UNEXPECTED_ARRAY_SPECIFIER, m_Name.c_str());
            m_ErrInfo.m_Msg = m_Buf;
            
            m_pErrIf->SetErrorInfo(m_ErrInfo);
            return;
        }
        
        if (pRvalue->IsArray())
        {
            sprintf_s(m_Buf, sizeof(m_Buf), m_pErrIf->ERROR_UNEXPECTED_ARRAY_SPECIFIER, m_Name.c_str());
            m_ErrInfo.m_Msg = m_Buf;
            
            m_pErrIf->SetErrorInfo(m_ErrInfo);
            return;
        }

        bool status = pInfo->m_pArrayValue->SetValue(m_ArraySpecifier, pRvalue->GetValue());
        if (!status)
        {
            // The array specifier could be wrong.
            sprintf_s(m_Buf, sizeof(m_Buf), m_pErrIf->ERROR_INCORRECT_ARRAY_SPECIFIER, m_Name.c_str());
            m_ErrInfo.m_Msg = m_Buf;
            
            m_pErrIf->SetErrorInfo(m_ErrInfo);
            return;
        }
    }

    // There is a symbol in the symbol table.  It is an array.
    // We are setting the value of the entire array here.
    // a = b where b is an array.
    void WholeArrayLvalue::Equ(ValueNode* pRvalue)
    {
        assert(pRvalue->IsArray());
        m_pSymbolTable->DeleteSymbol(m_Name);
        SymbolTable::SymbolInfo* pInfo = new SymbolTable::SymbolInfo;
        assert(pInfo != nullptr);

        pInfo->m_Name = m_Name;
        pInfo->m_IsArray = true;
        pInfo->m_pArrayValue = pRvalue->GetArrayValue()->Clone();
        bool status = m_pSymbolTable->CreateSymbol(m_Name, pInfo);
        assert(status);
    }

    // There is a symbol in the symbol table.  It is an array.
    // We are setting the value of the entire array here.
    // a = {0,1,2,3,4,...}.
    void WholeArrayLvalue::EquList(int count, ValueNode* pList)
    {
        std::unique_ptr<ArrayValue> pArrayValue(ConvertValueNodes(count, pList));
        if (pArrayValue.get() == nullptr)
        {
            return;
        }

        m_pSymbolTable->DeleteSymbol(m_Name);
        SymbolTable::SymbolInfo* pInfo = new SymbolTable::SymbolInfo;
        assert(pInfo != nullptr);
        pInfo->m_Name = m_Name;
        pInfo->m_IsArray = true;
        pInfo->m_pArrayValue = pArrayValue.release();
        bool status = m_pSymbolTable->CreateSymbol(m_Name, pInfo);
        assert(status);
    }

    // There is a symbol in the symbol table.  It is an array.
    // a = dim[q,r,s]
    void WholeArrayLvalue::Dim(ValueNode* pRvalue)
    {
        assert(pRvalue->IsArray());
        m_pSymbolTable->DeleteSymbol(m_Name);
        SymbolTable::SymbolInfo* pInfo = new SymbolTable::SymbolInfo;
        assert(pInfo != nullptr);

        pInfo->m_Name = m_Name;
        pInfo->m_IsArray = true;
        pInfo->m_pArrayValue = pRvalue->GetArrayValue()->Clone();
        bool status = m_pSymbolTable->CreateSymbol(m_Name, pInfo);
        assert(status);
    }

    // There is a symbol in the symbol table.  It is not array.
    // a = 1 
    // a = b where b is not an array.
    void VariableLvalue::Equ(ValueNode* pRvalue)
    {
        assert(!pRvalue->IsArray());
        m_pSymbolTable->DeleteSymbol(m_Name);
        SymbolTable::SymbolInfo* pInfo = new SymbolTable::SymbolInfo;
        assert(pInfo != nullptr);

        pInfo->m_Name = m_Name;
        pInfo->m_Value = pRvalue->GetValue();
        bool status = m_pSymbolTable->CreateSymbol(m_Name, pInfo);
        assert(status);
    }

    // There is a symbol in the symbol table.  It is not an array.
    // a = dim[q,r,s]
    void VariableLvalue::Dim(ValueNode* pRvalue)
    {
        assert(pRvalue->IsArray());
        m_pSymbolTable->DeleteSymbol(m_Name);
        SymbolTable::SymbolInfo* pInfo = new SymbolTable::SymbolInfo;
        assert(pInfo != nullptr);

        pInfo->m_Name = m_Name;
        pInfo->m_IsArray = true;
        pInfo->m_pArrayValue = pRvalue->GetArrayValue()->Clone();
        bool status = m_pSymbolTable->CreateSymbol(m_Name, pInfo);
        assert(status);
    }
}