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
    ArrayValue(std::vector<int>& rDims);
    ~ArrayValue();

    // Simple accessor
    std::vector<int> GetDims() { return m_Dims; }
    void SetDims(std::vector<int> d);
    std::optional<Value> GetValue(std::vector<int> element);
    std::vector<Value> GetValues();
    bool SetValue(std::vector<int> element, Value v);

    // Operations
    bool Add(ArrayValue& v);

private:
    // Convert element index to flag index
    std::optional<int> ConvertElementIndex(std::vector<int> element);

    // Perform likeness checks between 2 arrays.
    bool LikenessChecks(ArrayValue& v);

    // Flat array for the values based on the dimension.
    std::vector<int> m_Dims;
    std::vector<Value> m_Values;
};

};
