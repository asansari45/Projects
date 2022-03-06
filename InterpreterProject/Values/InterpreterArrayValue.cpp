#include <assert.h>
#include "InterpreterArrayValue.h"
#include "Log/InterpreterLog.h"

namespace Interpreter
{

ArrayValue::ArrayValue():
    m_Dims(),
    m_pInts(nullptr),
    m_pFloats(nullptr),
    m_pStrings(nullptr)
{
}

ArrayValue::ArrayValue(const ArrayValue& p):
    m_Dims(p.m_Dims),
    m_pInts(nullptr),
    m_pFloats(nullptr),
    m_pStrings(nullptr)
{
    if (p.m_pInts != nullptr)
    {
        m_pInts = new int[GetElementCount()];
        assert(m_pInts != nullptr);
        std::memcpy(m_pInts, const_cast<ArrayValue&>(p).GetInts(), GetElementCount() * sizeof(int));
    }

    if (p.m_pFloats != nullptr)
    {
        m_pFloats = new float[GetElementCount()];
        assert(m_pFloats != nullptr);
        std::memcpy(m_pFloats, const_cast<ArrayValue&>(p).GetFloats(), GetElementCount() * sizeof(float));
    }

    if (p.m_pStrings != nullptr)
    {
        m_pStrings = new std::string[GetElementCount()];
        assert(m_pStrings != nullptr);
        for (int i = 0; i < GetElementCount(); i++)
        {
            m_pStrings[i] = const_cast<ArrayValue&>(p).GetStrings()[i];
        }
    }
}

ArrayValue::~ArrayValue()
{
    delete [] m_pInts;
    delete [] m_pFloats;
    delete [] m_pStrings;
}

std::optional<Value> ArrayValue::GetValue(std::vector<int> element)
{
    std::optional<int> index = ConvertElementIndex(element);
    if (index == std::nullopt)
    {
        return {};
    }

    Value v;
    if (m_pInts != nullptr)
    {
        v.SetIntValue(m_pInts[*index]);
    }
    else if (m_pFloats != nullptr)
    {
        v.SetFloatValue(m_pFloats[*index]);
    }
    else if (m_pStrings != nullptr)
    {
        v.SetStringValue(m_pStrings[*index]);
    }
    else
    {
        return {};
    }

    return v;
}

bool ArrayValue::SetValue(std::vector<int> element, Value v)
{
    std::optional<int> index = ConvertElementIndex(element);
    if (index == std::nullopt)
    {
        return false;
    }

    if (v.GetType() == typeid(int))
    {
        if (m_pInts != nullptr)
        {
            m_pInts[*index] = v.GetIntValue();
            return true;
        }
        
        if (m_pFloats != nullptr)
        {
            m_pFloats[*index] = static_cast<float>(v.GetIntValue());
            return true;
        }

        assert(m_pStrings != nullptr);
        delete [] m_pStrings;
        m_pStrings = nullptr;

        m_pInts = new int[GetElementCount()];
        assert(m_pInts != nullptr);
        m_pInts[*index] = v.GetIntValue();
        return true;
   }

    if (v.GetType() == typeid(float))
    {
        if (m_pInts != nullptr)
        {
            // Convert ints over to floats.
            m_pFloats = new float[GetElementCount()];
            AssignArrays(m_pFloats, m_pInts, GetElementCount());
            delete [] m_pInts;
            m_pInts = nullptr;
            m_pFloats[*index] = v.GetFloatValue();
            return true;
        }

        if (m_pFloats != nullptr)
        {
            m_pFloats[*index] = v.GetFloatValue();
            return true;
        }

        assert(m_pStrings != nullptr);
        delete [] m_pStrings;
        m_pStrings = nullptr;

        m_pFloats = new float[GetElementCount()];
        assert(m_pFloats != nullptr);
        m_pFloats[*index] = v.GetFloatValue();
        return true;
    }

    assert(v.GetType() == typeid(std::string));
    if (m_pStrings != nullptr)
    {
        m_pStrings[*index] = v.GetStringValue();
        return true;
    }

    if (m_pInts != nullptr)
    {
        delete [] m_pInts;
        m_pInts = nullptr;
        m_pStrings = new std::string[GetElementCount()];
        m_pStrings[*index] = v.GetStringValue();
        return true;
    }

    assert(m_pFloats != nullptr);
    delete [] m_pFloats;
    m_pFloats = nullptr;
    m_pStrings = new std::string[GetElementCount()];
    m_pStrings[*index] = v.GetStringValue();
    return true;
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
    // Create a flat array of ints
    m_Dims = d;
    delete [] m_pInts;
    m_pInts = nullptr;

    delete [] m_pFloats;
    m_pFloats = nullptr;

    delete [] m_pStrings;
    m_pStrings = nullptr;

    m_pInts = new int[GetElementCount()];
    assert(m_pInts != nullptr);
    std::memset(m_pInts, 0, sizeof(int)*GetElementCount());
}

std::type_index ArrayValue::GetType()
{
    if (m_pInts != nullptr)
    {
        return typeid(int);
    }

    if ( m_pFloats != nullptr)
    {
        return typeid(float);
    }

    return typeid(std::string);
}

// Add 2 arrays together.
bool ArrayValue::Add(ArrayValue& v)
{
    bool status = LikenessChecks(v);
    if (!status)
    {
        return false;
    }

    std::type_index ourType = GetType();
    std::type_index theirType = v.GetType();
    if (ourType == typeid(int))
    {
        if (theirType == typeid(int))
        {
            AddArrays(m_pInts, v.GetInts(), m_pInts, GetElementCount());
            return true;
        }

        if (theirType == typeid(float))
        {
            // Convert results to be all floats.
            m_pFloats = new float[GetElementCount()];
            assert(m_pFloats != nullptr);
            AddArrays(m_pInts, v.GetFloats(), m_pFloats, GetElementCount());
            delete [] m_pInts;
            m_pInts = nullptr;
            return true;
        }

        return false;
    }

    if (ourType == typeid(float))
    {
        if (theirType == typeid(int))
        {
            AddArrays(m_pFloats, v.GetInts(), m_pFloats, GetElementCount());
            return true;
        }

        if (theirType == typeid(float))
        {
            AddArrays(m_pFloats, v.GetFloats(), m_pFloats, GetElementCount());
            return true;
        }

        return false;
    }

    assert(ourType == typeid(std::string));
    if (theirType == typeid(std::string))
    {
        AddArrays(m_pStrings, v.GetStrings(), m_pStrings, GetElementCount());
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
    std::type_index ourType = GetType();
    std::type_index theirType = v.GetType();
    if (ourType == typeid(int))
    {
        if (theirType == typeid(std::string))
        {
            return false;
        }

        return true;
    }

    if (ourType == typeid(float))
    {
        if (theirType == typeid(std::string))
        {
            return false;
        }

        return true;
    }

    assert(ourType == typeid(std::string));
    if (theirType == typeid(int) || theirType == typeid(float))
    {
        return false;
    }

    return true;
}

int ArrayValue::GetElementCount()
{
    int count = 1;
    for (auto i = 0; i < m_Dims.size(); i++)
    {
        count *= m_Dims[i];
    }

    return count;
}

std::string ArrayValue::GetTypeRepr()
{
    if (m_pInts != nullptr)
    {
        return "INT";
    }

    if (m_pFloats != nullptr)
    {
        return "FLT";
    }

    if (m_pStrings != nullptr)
    {
        return "STR";
    }

    return "UNK";
}

std::string ArrayValue::GetDimsRepr()
{
    assert(m_Dims.size() <= 3);
    std::string s = "[";
    for (size_t d = 0; d < m_Dims.size(); d++)
    {
        char buf[128];
        if (d != m_Dims.size() - 1)
        {
            sprintf_s(buf, sizeof(buf), "%d,", m_Dims[d]);
        }
        else
        {
            sprintf_s(buf, sizeof(buf), "%d", m_Dims[d]);
        }

        s += buf;
    }

    s += "]";
    return s;
}

std::string ArrayValue::GetValuesRepr()
{
    const int ELEMENT_COUNT = 10;
    char buf[256];
    std::string s;
    for (int i = 0; i < ELEMENT_COUNT && i < GetElementCount(); i++)
    {
        if ( m_pInts != nullptr)
        {
            sprintf_s(buf, sizeof(buf), "%d ", m_pInts[i]);
        }
        else if( m_pFloats != nullptr)
        {
            sprintf_s(buf, sizeof(buf), "%.2f ", m_pFloats[i]);
        }
        else if( m_pStrings != nullptr)
        {
            sprintf_s(buf, sizeof(buf), "%s ", m_pStrings[i].c_str());
        }
        s += buf;
    }
    s += "...";
    return s;
}

ArrayValue& ArrayValue::operator=(const ArrayValue& o)
{
    delete [] m_pInts;
    m_pInts = nullptr;
    delete [] m_pFloats;
    m_pFloats = nullptr;
    delete [] m_pStrings;
    m_pStrings = nullptr;

    m_Dims = const_cast<ArrayValue&>(o).GetDims();
    if (const_cast<ArrayValue&>(o).GetInts())
    {
        m_pInts = new int[GetElementCount()];
        AssignArrays(m_pInts, const_cast<ArrayValue&>(o).GetInts(), GetElementCount());
    }

    if (const_cast<ArrayValue&>(o).GetFloats())
    {
        m_pFloats = new float[GetElementCount()];
        AssignArrays(m_pFloats, const_cast<ArrayValue&>(o).GetFloats(), GetElementCount());
    }

    if (const_cast<ArrayValue&>(o).GetStrings())
    {
        m_pStrings = new std::string[GetElementCount()];
        AssignArrays(m_pStrings, const_cast<ArrayValue&>(o).GetStrings(), GetElementCount());
    }

    return *this;
}

}
