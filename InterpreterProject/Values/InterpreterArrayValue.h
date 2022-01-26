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

    // For debug purposes
    std::string GetRepresentation()
    {
        assert(m_Dims.size() <= 3);
        std::vector<int> element;
        std::string s = "[";
        for (size_t d = 0; d < m_Dims.size(); d++)
        {
            if (d != m_Dims.size() - 1)
            {
                s += "0,";
            }
            else
            {
                s += "0";
            }
            element.push_back(0);
        }

        const int ELEMENT_COUNT = 10;
        char buf[256];
        sprintf_s(buf, sizeof(buf), "%s...%d]:  ", s.c_str(), ELEMENT_COUNT-1);
        s = buf;

        int maxElementCount = m_Dims.back();
        for (int i = 0; i < maxElementCount && i < ELEMENT_COUNT; i++)
        {
            element[m_Dims.size() - 1] = i;
            std::optional<Value> v = GetValue(element);
            assert(v != std::nullopt);
            s += v->GetRepresentation() + " ";
        }
        s += "...";
        return s;
    }

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
