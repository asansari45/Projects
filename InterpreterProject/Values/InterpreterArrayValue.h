#pragma once
#include <vector>
#include <optional>
#include "InterpreterValue.h"

namespace Interpreter
{

// Only up to 3 dimensions
class ArrayValue
{
public:

    ~ArrayValue();
    static ArrayValue* Create(std::vector<int> dims, std::type_index t);

    ArrayValue* Clone();

    // Simple accessor
    std::vector<int> GetDims() { return m_Dims; }

    std::optional<Value> GetValue(std::vector<int> element);
    bool SetValue(std::vector<int> element, Value v);

    std::type_index GetType();

    // Operations
    bool Add(ArrayValue* v);

    // For debug purposes
    std::string GetTypeRepr();
    std::string GetDimsRepr();
    std::string GetValuesRepr();
    void FillStream(std::stringstream& rStream);

    int* GetIntData() const
    {
        return m_Data.m_pInts;
    }

    float* GetFloatData() const
    {
        return m_Data.m_pFloats;
    }

    std::string* GetStringData() const
    {
        return m_Data.m_pStrings;
    }

private:
    ArrayValue(std::vector<int> dims, std::type_index t);
    ArrayValue(const ArrayValue& p)=delete;

    // Convert element index to flag index
    std::optional<int> ConvertElementIndex(std::vector<int> element);

    // Perform likeness checks between 2 arrays.
    bool LikenessChecks(ArrayValue* v);

    int GetElementCount();

    template<class A, class B, class C>
    void AddArrays(A* pArray1, B* pArray2, C* pResult, int elementCount )
    {
        for (int i=0; i < elementCount; i++)
        {
            pResult[i] = static_cast<C>(pArray1[i]) + static_cast<C>(pArray2[i]);
        }
    }

    template<class A>
    void AssignArrays(A* pArray1, A* pArray2, int elementCount)
    {
        for (int i=0; i < elementCount; i++)
        {
            pArray1[i] = pArray2[i];
        }
    }

    template<class A, class B>
    void AssignArrays(A* pArray1, B* pArray2, int elementCount)
    {
        for (int i=0; i < elementCount; i++)
        {
            pArray1[i] = static_cast<A>(pArray2[i]);
        }
    }

    // Flat array for the values based on the dimension.
    std::vector<int> m_Dims;

    // Could be int, float, or string
    std::type_index m_Type;
    union Type
    {
        int* m_pInts;
        float* m_pFloats;
        std::string* m_pStrings;
    };

    Type m_Data;
};

};
