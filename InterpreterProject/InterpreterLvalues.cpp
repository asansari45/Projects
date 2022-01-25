#include "InterpreterLvalues.h"
#include "InterpreterRvalue.h"
#include "InterpreterErrorInterface.h"

namespace Interpreter
{
	// There is no symbol for the given rvalue in the symbol table.
	// Create it and set the value.
	// a = b where a does not exist, b is an array or value.
	// a = 1 where a does not exist, 1 is a value.
	void NoSymbolLvalue::Equ(Rvalue& rRvalue)
	{
		assert(m_pSymbolTable != nullptr);
		assert(m_pSymbolTable->IsSymbolPresent(m_Name) == false);
		SymbolTable::SymbolInfo info;
		info.m_Name = m_Name;

		// No arrays specifiers when no symbol is specified.
		if (m_ArraySpecifier.size() != 0)
		{
			sprintf_s(m_Buf, sizeof(m_Buf), m_pErrIf->ERROR_MISSING_SYMBOL, m_Name.c_str());
			m_ErrInfo.m_Msg = m_Buf;
			
			m_pErrIf->SetErrorInfo(m_ErrInfo);
			return;
		}

		bool status;
		if (rRvalue.IsArray())
		{
			info.m_IsArray = true;
			info.m_ArrayValue = rRvalue.GetArrayValue();
			status = m_pSymbolTable->CreateSymbol(m_Name, info);
		}
		else
		{
			info.m_IsArray = false;
			info.m_Value = rRvalue.GetValue();
			status = m_pSymbolTable->CreateSymbol(m_Name, info);
		}
		assert(status);
	}

	// There is no symbol for the given rvalue in the symbol table.
	// a = dim[q,r,s] where a does not exist.
	void NoSymbolLvalue::Dim(Rvalue& rRvalue)
	{
		assert(m_pSymbolTable != nullptr);
		assert(m_pSymbolTable->IsSymbolPresent(m_Name) == false);
		assert(rRvalue.IsArray());
		SymbolTable::SymbolInfo info;
		info.m_Name = m_Name;
		info.m_IsArray = true;
		info.m_ArrayValue = rRvalue.GetArrayValue();
		bool status = m_pSymbolTable->CreateSymbol(m_Name, info);
		assert(status);
	}

	// There is a symbol in the symbol table.  It is an array.
	// We are setting the value of one element here.
	// a[0,1,1] = 3 where a must exist or an error.
	// a[0,1,1] = b where a must exist and b must not be an array.
	void ArrayElementLvalue::Equ(Rvalue& rRvalue)
	{
		assert(m_pSymbolTable != nullptr);
		if (!m_pSymbolTable->IsSymbolPresent(m_Name))
		{
			sprintf_s(m_Buf, sizeof(m_Buf), m_pErrIf->ERROR_MISSING_SYMBOL, m_Name.c_str());
			m_ErrInfo.m_Msg = m_Buf;
			
			m_pErrIf->SetErrorInfo(m_ErrInfo);
			return;
		}

		std::optional<SymbolTable::SymbolInfo> info = m_pSymbolTable->ReadSymbol(m_Name);
		assert(info != std::nullopt);

		if (!info->m_IsArray)
		{
			sprintf_s(m_Buf, sizeof(m_Buf), m_pErrIf->ERROR_UNEXPECTED_ARRAY_SPECIFIER, m_Name.c_str());
			m_ErrInfo.m_Msg = m_Buf;
			
			m_pErrIf->SetErrorInfo(m_ErrInfo);
			return;
		}
		
		if (rRvalue.IsArray())
		{
			sprintf_s(m_Buf, sizeof(m_Buf), m_pErrIf->ERROR_UNEXPECTED_ARRAY_SPECIFIER, m_Name.c_str());
			m_ErrInfo.m_Msg = m_Buf;
			
			m_pErrIf->SetErrorInfo(m_ErrInfo);
			return;
		}

		bool status = info->m_ArrayValue.SetValue(m_ArraySpecifier, rRvalue.GetValue());
		if (!status)
		{
			// The array specifier could be wrong.
			sprintf_s(m_Buf, sizeof(m_Buf), m_pErrIf->ERROR_INCORRECT_ARRAY_SPECIFIER, m_Name.c_str());
			m_ErrInfo.m_Msg = m_Buf;
			
			m_pErrIf->SetErrorInfo(m_ErrInfo);
			return;
		}

		status = m_pSymbolTable->UpdateSymbol(m_Name, *info);
		assert(status);
	}

	// There is a symbol in the symbol table.  It is an array.
	// We are setting the value of the entire array here.
	// a = b where b is an array.
	void WholeArrayLvalue::Equ(Rvalue& rRvalue)
	{
		assert(rRvalue.IsArray());
		m_pSymbolTable->DeleteSymbol(m_Name);
		SymbolTable::SymbolInfo info;
		info.m_Name = m_Name;
		info.m_IsArray = true;
		info.m_ArrayValue = rRvalue.GetArrayValue();
		bool status = m_pSymbolTable->CreateSymbol(m_Name, info);
		assert(status);
	}

	// There is a symbol in the symbol table.  It is an array.
	// a = dim[q,r,s]
	void WholeArrayLvalue::Dim(Rvalue& rRvalue)
	{
		assert(rRvalue.IsArray());
		m_pSymbolTable->DeleteSymbol(m_Name);
		SymbolTable::SymbolInfo info;
		info.m_Name = m_Name;
		info.m_IsArray = true;
		info.m_ArrayValue = rRvalue.GetArrayValue();
		bool status = m_pSymbolTable->CreateSymbol(m_Name, info);
		assert(status);
	}

	// There is a symbol in the symbol table.  It is not array.
	// a = 1 
	// a = b where b is not an array.
	void VariableLvalue::Equ(Rvalue& rRvalue)
	{
		assert(!rRvalue.IsArray());
		m_pSymbolTable->DeleteSymbol(m_Name);
		SymbolTable::SymbolInfo info;
		info.m_Name = m_Name;
		info.m_Value = rRvalue.GetValue();
		bool status = m_pSymbolTable->CreateSymbol(m_Name, info);
		assert(status);
	}

	// There is a symbol in the symbol table.  It is not an array.
	// a = dim[q,r,s]
	void VariableLvalue::Dim(Rvalue& rRvalue)
	{
		assert(rRvalue.IsArray());
		m_pSymbolTable->DeleteSymbol(m_Name);
		SymbolTable::SymbolInfo info;
		info.m_Name = m_Name;
		info.m_IsArray = true;
		info.m_ArrayValue = rRvalue.GetArrayValue();
		bool status = m_pSymbolTable->CreateSymbol(m_Name, info);
		assert(status);
	}

	// This is a reference parameter in a function.  It can morph into other values
	// based on what the target is.
	void RefLvalue::Equ(Rvalue& rRvalue)
	{
		assert(m_pSymbolTable != nullptr);
		assert(m_pSymbolTable->IsSymbolPresent(m_Name));

		std::optional<SymbolTable::SymbolInfo> symbolInfo = m_pSymbolTable->ReadSymbol(m_Name);
		assert(symbolInfo != std::nullopt);
		assert(symbolInfo->m_IsRef);

		if (!rRvalue.IsArray())
		{
			// rvalue is not an array
	   		VariableLvalue lvalue(symbolInfo->m_RefName, symbolInfo->m_pRefTable, m_pErrIf, m_ErrInfo);
			lvalue.Equ(rRvalue);
			return;
		}

		// It is an array.
		WholeArrayLvalue lvalue(symbolInfo->m_RefName, symbolInfo->m_pRefTable, m_pErrIf, m_ErrInfo);
		lvalue.Equ(rRvalue);
	}

	// This is a reference parameter in a function.  It can morph into other values
	// based on what the target is.
	// a = dim[q,r,s]
	void RefLvalue::Dim(Rvalue& rRvalue)
	{
		assert(m_pSymbolTable != nullptr);
		assert(m_pSymbolTable->IsSymbolPresent(m_Name));

		std::optional<SymbolTable::SymbolInfo> symbolInfo = m_pSymbolTable->ReadSymbol(m_Name);
		assert(symbolInfo != std::nullopt);
		assert(symbolInfo->m_IsRef);

		if (!rRvalue.IsArray())
		{
			// rvalue is not an array
			VariableLvalue lvalue(symbolInfo->m_RefName, symbolInfo->m_pRefTable, m_pErrIf, m_ErrInfo);
			lvalue.Dim(rRvalue);
			return;
		}

		// It is an array.
		WholeArrayLvalue lvalue(symbolInfo->m_RefName, symbolInfo->m_pRefTable, m_pErrIf, m_ErrInfo);
		lvalue.Dim(rRvalue);
	}

	// This is a reference parameter in a function.  It can morph into other values
	// based on what the target is.
	void RefArraySpecifierLvalue::Equ(Rvalue& rRvalue)
	{
		assert(m_pSymbolTable != nullptr);
		assert(m_pSymbolTable->IsSymbolPresent(m_Name));

		std::optional<SymbolTable::SymbolInfo> symbolInfo = m_pSymbolTable->ReadSymbol(m_Name);
		assert(symbolInfo != std::nullopt);
		assert(symbolInfo->m_IsRef);

		ArrayElementLvalue lvalue(symbolInfo->m_RefName, 
								  symbolInfo->m_pRefTable, 
								  m_pErrIf, 
								  m_ErrInfo,
								  m_ArraySpecifier);
		lvalue.Equ(rRvalue);
	}
}