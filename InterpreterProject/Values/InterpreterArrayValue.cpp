#include <assert.h>
#include "InterpreterArrayValue.h"
#include "InterpreterLog.h"

namespace Interpreter
{

ArrayValue::ArrayValue() :
    m_Dims(),
    m_Values()
{
}

ArrayValue::ArrayValue(std::vector<int>& rDims) :
    m_Dims(rDims),
    m_Values()
{
    // Create a flat array.
    int elemCount = rDims[0];
    for (size_t i = 1; i < rDims.size(); i++)
    {
        elemCount *= rDims[i];
    }

    // Fill with default values.
    for (int i = 0; i < elemCount; i++)
    {
        Value v;
        m_Values.push_back(v);
    }
}

ArrayValue::~ArrayValue()
{
}

std::optional<Value> ArrayValue::GetValue(std::vector<int> element)
{
    std::optional<int> index = ConvertElementIndex(element);
    if (index != std::nullopt)
    {
        return m_Values[index.value()];
    }

    return {};
}

std::vector<Value> ArrayValue::GetValues()
{
    return m_Values;
}

bool ArrayValue::SetValue(std::vector<int> element, Value v)
{
    std::optional<int> index = ConvertElementIndex(element);
    if (index != std::nullopt)
    {
        m_Values[index.value()] = v;
        return true;
    }

    return false;
}

// Only 3 dimensions are allowed.  2-dimensions are just rows concatenated together.
// 2 dimensions are [row,column].
// 3 dimensions are 2-dimensional arrays concatenated.  [row, column, selector].
std::optional<int> ArrayValue::ConvertElementIndex(std::vector<int> element)
{
    // Checks to make sure it matches our dimensions
    if (element.size() != m_Dims.size())
    {
        return {};
    }

    // 1-dimensional array
    if (element.size() == 1)
    {
        // Only return the index if it fits.
        if (element[0] < m_Dims[0])
        {
            return element[0];
        }
        else
        {
            return {};
        }
    }

    // 2-dimensional array
    if (element.size() == 2)
    {
        int rowCount = m_Dims[0];
        int columnCount = m_Dims[1];

        int requestedRow = element[0];
        int requestedCol = element[1];
        if (requestedRow >= rowCount || requestedCol >= columnCount)
        {
            return {};
        }

        return requestedRow * columnCount + requestedCol;
    }

    // 3-dimensional array
    if (element.size() == 3)
    {
        int rowCount = m_Dims[0];
        int columnCount = m_Dims[1];
        int matrixCount = m_Dims[2];

        int requestedRow = element[0];
        int requestedCol = element[1];
        int matrixSel = element[2];
        if (requestedRow >= rowCount || requestedCol >= columnCount || matrixSel >= matrixCount)
        {
            return {};
        }

        return matrixSel * rowCount* columnCount + requestedRow * columnCount + requestedCol;
    }

    return {};
}

void ArrayValue::SetDims(std::vector<int> d)
{
    // Create a flat array.
    m_Dims = d;
    int elemCount = m_Dims[0];
    for (size_t i = 1; i < m_Dims.size(); i++)
    {
        elemCount *= m_Dims[i];
    }

    // Fill with default values.
    m_Values.clear();
    for (int i = 0; i < elemCount; i++)
    {
        Value v;
        m_Values.push_back(v);
    }
}

// Add 2 arrays together.
bool ArrayValue::Add(ArrayValue& v)
{
    bool status = LikenessChecks(v);
    if (!status)
    {
        return false;
    }

    // All the checks passed. Let's add each element one by one.
    std::vector<Value> otherValues = v.GetValues();
    for (auto i = 0; i < m_Values.size(); i++)
    {
        status = m_Values[i].Add(otherValues[i]);
        assert(status);
    }

    return true;
}

bool ArrayValue::LikenessChecks(ArrayValue& v)
{
    // The dimensions must match.
    if (m_Dims != v.GetDims())
    {
        return false;
    }

    // The type of each element must match.
    std::vector<Value> otherValues = v.GetValues();
    for (auto i = 0; i < m_Values.size(); i++)
    {
        if (m_Values[i].GetType() != otherValues[i].GetType())
        {
            return false;
        }
    }

    return true;
}

}
