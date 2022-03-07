#include <assert.h>
#include "InterpreterArrayValue.h"
#include "Log/InterpreterLog.h"

namespace Interpreter
{

ArrayValue::ArrayValue():
    m_Dims(),
    m_Type(typeid(int))
{
    m_Data.m_pInts = nullptr;
}

ArrayValue::ArrayValue(const ArrayValue& p):
    m_Dims(p.m_Dims),
    m_Type(p.m_Type)
{
    if (m_Type == typeid(int))
    {
        AssignArrays(m_Data.m_pInts, p.GetData<int>(), GetElementCount());
    }

    if (m_Type == typeid(float))
    {
        AssignArrays(m_Data.m_pFloats, p.GetData<float>(), GetElementCount());
    }

    if (m_Type == typeid(std::string))
    {
        AssignArrays(m_Data.m_pStrings, p.GetData<std::string>(), GetElementCount());
    }
}

ArrayValue::~ArrayValue()
{
    if (m_Type == typeid(int))
    {
        delete [] m_Data.m_pInts;
    }

    if (m_Type == typeid(float))
    {
        delete [] m_Data.m_pFloats;
    }

    if (m_Type == typeid(std::string))
    {
        delete [] m_Data.m_pStrings;
    }
}

bool ArrayValue::Init(std::vector<int> dims, std::type_index t)
{
    if (m_Type == typeid(int))
    {
        delete [] m_Data.m_pInts;
    }

    if (m_Type == typeid(float))
    {
        delete [] m_Data.m_pFloats;
    }

    if (m_Type == typeid(std::string))
    {
        delete [] m_Data.m_pStrings;
    }

    m_Type = t;
    m_Dims = dims;
    if (m_Type == typeid(int))
    {
        m_Data.m_pInts = new int[GetElementCount()];
    }

    if (m_Type == typeid(float))
    {
        m_Data.m_pFloats = new float[GetElementCount()];
    }

    if (m_Type == typeid(std::string))
    {
        m_Data.m_pStrings = new std::string[GetElementCount()];
    }
}

std::optional<Value> ArrayValue::GetValue(std::vector<int> element)
{
    std::optional<int> index = ConvertElementIndex(element);
    if (index == std::nullopt)
    {
        return {};
    }

    Value v;
    if (m_Type == typeid(int))
    {
        v.SetIntValue(m_Data.m_pInts[*index]);
    }
    else if (m_Type == typeid(float))
    {
        v.SetFloatValue(m_Data.m_pFloats[*index]);
    }
    else if (m_Type == typeid(std::string))
    {
        v.SetStringValue(m_Data.m_pStrings[*index]);
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
        if (m_Type == typeid(int))
        {
            m_Data.m_pInts[*index] = v.GetIntValue();
            return true;
        }
        
        if (m_Type == typeid(float))
        {
            m_Data.m_pFloats[*index] = static_cast<float>(v.GetIntValue());
            return true;
        }

        assert(m_Type == typeid(std::string));
        delete [] m_Data.m_pStrings;
        m_Data.m_pStrings = nullptr;

        m_Type = typeid(int);
        m_Data.m_pInts = new int[GetElementCount()];
        assert(m_Data.m_pInts != nullptr);
        m_Data.m_pInts[*index] = v.GetIntValue();
        return true;
   }

    if (v.GetType() == typeid(float))
    {
        if (m_Type == typeid(int))
        {
            // Convert ints over to floats.
            float* pFloats = new float[GetElementCount()];
            AssignArrays(pFloats, m_Data.m_pInts, GetElementCount());
            delete [] m_Data.m_pInts;
            m_Data.m_pFloats = pFloats;
            m_Data.m_pFloats[*index] = v.GetFloatValue();
            return true;
        }

        if (m_Type == typeid(float))
        {
            m_Data.m_pFloats[*index] = v.GetFloatValue();
            return true;
        }

        assert(m_Data.m_pStrings != nullptr);
        delete [] m_Data.m_pStrings;

        m_Data.m_pFloats = new float[GetElementCount()];
        assert(m_Data.m_pFloats != nullptr);
        m_Data.m_pFloats[*index] = v.GetFloatValue();
        return true;
    }

    assert(v.GetType() == typeid(std::string));
    if (m_Type == typeid(std::string))
    {
        m_Data.m_pStrings[*index] = v.GetStringValue();
        return true;
    }

    if (m_Type == typeid(int))
    {
        delete [] m_Data.m_pInts;
        m_Type = typeid(std::string);
        m_Data.m_pStrings = new std::string[GetElementCount()];
        m_Data.m_pStrings[*index] = v.GetStringValue();
        return true;
    }

    assert(m_Type == typeid(float));
    delete [] m_Data.m_pFloats;
    m_Type = typeid(std::string);
    m_Data.m_pStrings = new std::string[GetElementCount()];
    m_Data.m_pStrings[*index] = v.GetStringValue();
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

std::type_index ArrayValue::GetType()
{
    return m_Type;
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
            AddArrays(m_Data.m_pInts, v.GetData<int>(), m_Data.m_pInts, GetElementCount());
            return true;
        }

        if (theirType == typeid(float))
        {
            // Convert results to be all floats.
            float* pFloats = new float[GetElementCount()];
            assert(m_Data.m_pFloats != nullptr);
            AddArrays(m_Data.m_pInts, v.GetData<float>(), pFloats, GetElementCount());
            delete [] m_Data.m_pInts;
            m_Type = typeid(float);
            m_Data.m_pFloats = pFloats;
            return true;
        }

        return false;
    }

    if (ourType == typeid(float))
    {
        if (theirType == typeid(int))
        {
            AddArrays(m_Data.m_pFloats, v.GetData<int>(), m_Data.m_pFloats, GetElementCount());
            return true;
        }

        if (theirType == typeid(float))
        {
            AddArrays(m_Data.m_pFloats, v.GetData<float>(), m_Data.m_pFloats, GetElementCount());
            return true;
        }

        return false;
    }

    assert(ourType == typeid(std::string));
    if (theirType == typeid(std::string))
    {
        AddArrays(m_Data.m_pStrings, v.GetData<std::string>(), m_Data.m_pStrings, GetElementCount());
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
    if (m_Type == typeid(int))
    {
        return "INT";
    }

    if (m_Type == typeid(float))
    {
        return "FLT";
    }

    if (m_Type == typeid(std::string))
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
        if ( m_Type == typeid(int))
        {
            sprintf_s(buf, sizeof(buf), "%d ", m_Data.m_pInts[i]);
        }
        else if( m_Type == typeid(float))
        {
            sprintf_s(buf, sizeof(buf), "%.2f ", m_Data.m_pFloats[i]);
        }
        else if( m_Type == typeid(std::string))
        {
            sprintf_s(buf, sizeof(buf), "%s ", m_Data.m_pStrings[i].c_str());
        }
        s += buf;
    }
    s += "...";
    return s;
}

ArrayValue& ArrayValue::operator=(const ArrayValue& o)
{
    ArrayValue& r = const_cast<ArrayValue&>(o);
    m_Dims = r.GetDims();
    if ( m_Type == typeid(int))
    {
        delete [] m_Data.m_pInts;
    }
    else if( m_Type == typeid(float))
    {
        delete [] m_Data.m_pFloats;
    }
    else if( m_Type == typeid(std::string))
    {
        delete [] m_Data.m_pStrings;
    }

    if (r.GetType() == typeid(int))
    {
        m_Data.m_pInts = new int[GetElementCount()];
        AssignArrays(m_Data.m_pInts, r.GetData<int>(), GetElementCount());
    }
    else if (r.GetType() == typeid(float))
    {
        m_Data.m_pFloats = new float[GetElementCount()];
        AssignArrays(m_Data.m_pFloats, r.GetData<float>(), GetElementCount());
    }
    else if (r.GetType() == typeid(std::string))
    {
        m_Data.m_pStrings = new std::string[GetElementCount()];
        AssignArrays(m_Data.m_pStrings, r.GetData<std::string>(), GetElementCount());
    }

    return *this;
}

}
