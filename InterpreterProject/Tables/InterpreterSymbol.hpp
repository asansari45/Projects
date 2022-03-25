#pragma once

#include <string>
#include <optional>
#include <vector>

namespace Interpreter
{
    class SymbolTable;
    class Value;
    class ArrayValue;

    class Symbol
    {
    public:
        enum Kind
        {
            VALUE = 1,
            ARRAY = 2,
            REFERENCE = 3
        };

        // Value constructor
        Symbol(std::string name, SymbolTable* pTable, Value* pValue);

        // ArrayValue constructor
        Symbol(std::string name, SymbolTable* pTable, ArrayValue* pArrayValue);

        // Reference constructor
        Symbol(std::string name, std::string refName, SymbolTable* RefTable);

        // Make a copy of this symbol
        Symbol* Clone();

        std::string GetName() { return m_Name; }
        SymbolTable* GetTable() { return m_pTable; }
        std::string GetRefName() { return m_RefName; }
        std::string GetRefTable() { return m_pRefTable; }

        // Set/Get a simple value.
        void SetValue(Value* pValue);
        Value* GetValue();

        // Set an array element value.
        void SetArrayValue(ArrayValue* pArrayValue);
        ArrayValue* GetArrayValue();

        // Get an array element value
        template<typename T>
        std::optional<T> GetArrayValue(std::vector<int> element);
        template<typename T>
        std::optional<T> GetArrayValue(int flatElement);

        // Sets an entire array.
        template<typename T>
        bool SetArray(T* pValues, int elementCount);

        // Gets an entire array.
        template<typename T>
        T* GetArray

    private:
        Kind m_Kind;
        std::string m_Name;
        SymbolTable* m_pTable;
        Value* m_pValue;
        ArrayValue* m_pArrayValue;
        std::string m_RefName;
        SymbolTable* m_pRefTable;
    };
};
