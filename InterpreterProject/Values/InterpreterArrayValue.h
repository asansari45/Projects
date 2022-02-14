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

    ArrayValue();
    ArrayValue(const ArrayValue& p);
    ~ArrayValue();

    // Simple accessor
    std::vector<int> GetDims() { return m_Dims; }
    void SetDims(std::vector<int> d);

    std::optional<Value> GetValue(std::vector<int> element);
    // std::vector<Value> GetValues();
    bool SetValue(std::vector<int> element, Value v);

    std::type_index GetType();

    ArrayValue& operator=(const ArrayValue& o);

    // Operations
    bool Add(ArrayValue& v);

    // For debug purposes
    std::string GetRepresentation();

    int* GetInts(){ return m_pInts; }
    float* GetFloats(){ return m_pFloats; }
    std::string* GetStrings(){ return m_pStrings; }

private:
    // Convert element index to flag index
    std::optional<int> ConvertElementIndex(std::vector<int> element);

    // Perform likeness checks between 2 arrays.
    bool LikenessChecks(ArrayValue& v);

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
    int* m_pInts;
    float* m_pFloats;
    std::string* m_pStrings;
};

};
