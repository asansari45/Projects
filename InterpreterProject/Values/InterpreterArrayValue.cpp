#include <assert.h>
#include <sstream>
#include "InterpreterArrayValue.h"
#include "Log/InterpreterLog.h"

namespace Interpreter
{

ArrayValue::ArrayValue(std::vector<int> dims, std::type_index t):
    m_Dims(dims),
    m_Type(t)
{
    if (m_Type == typeid(char))
    {
        m_Data.m_pChars = new char[GetElementCount()];
        std::memset(m_Data.m_pChars, 0, sizeof(char) * GetElementCount());
        assert(m_Data.m_pChars != nullptr);
    }
    else if (m_Type == typeid(unsigned int))
    {
        m_Data.m_pUnsignedInts = new unsigned int[GetElementCount()];
        std::memset(m_Data.m_pUnsignedInts, 0, sizeof(unsigned int) * GetElementCount());
        assert(m_Data.m_pUnsignedInts != nullptr);
    }
    else if (m_Type == typeid(int))
    {
        m_Data.m_pInts = new int[GetElementCount()];
        std::memset(m_Data.m_pInts, 0, sizeof(int) * GetElementCount());
        assert(m_Data.m_pInts != nullptr);
    }
    else if (m_Type == typeid(float))
    {
        m_Data.m_pFloats = new float[GetElementCount()];
        std::memset(m_Data.m_pFloats, 0, sizeof(float) * GetElementCount());
        assert(m_Data.m_pFloats != nullptr);
    }
    else if (m_Type == typeid(std::string))
    {
        m_Data.m_pStrings = new std::string[GetElementCount()];
        assert(m_Data.m_pStrings != nullptr);
    }
    else
    {
        assert(false);
    }
}

ArrayValue::~ArrayValue()
{
    if (m_Type == typeid(char))
    {
        delete [] m_Data.m_pChars;
    }

    if (m_Type == typeid(unsigned int))
    {
        delete [] m_Data.m_pUnsignedInts;
    }

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

ArrayValue* ArrayValue::Create(std::vector<int> dims, std::type_index t)
{
    return new ArrayValue(dims,t);
}

std::optional<Value> ArrayValue::GetValue(std::vector<int> element)
{
    std::optional<int> index = ConvertElementIndex(element);
    if (index == std::nullopt)
    {
        return {};
    }

    return GetValue(*index);
}

std::optional<Value> ArrayValue::GetValue(int flatElement)
{
    if (flatElement >= GetElementCount())
    {
        return {}; 
    }

    Value v;
    if (m_Type == typeid(char))
    {
        v.SetValue(m_Data.m_pChars[flatElement]);
    }
    else if (m_Type == typeid(unsigned int))
    {
        v.SetValue(m_Data.m_pUnsignedInts[flatElement]);
    }
    else if (m_Type == typeid(int))
    {
        v.SetValue(m_Data.m_pInts[flatElement]);
    }
    else if (m_Type == typeid(float))
    {
        v.SetValue(m_Data.m_pFloats[flatElement]);
    }
    else if (m_Type == typeid(std::string))
    {
        v.SetValue(m_Data.m_pStrings[flatElement]);
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

    return SetValue(*index, v);
}

bool ArrayValue::SetValue(int flatElement, Value v)
{
    if (flatElement >= GetElementCount())
    {
        return {};
    }
    
    if (v.GetType() == m_Type)
    {
        if (m_Type == typeid(char))
        {
            m_Data.m_pChars[flatElement] = v.GetValue<char>();
            return true;
        }

        if (m_Type == typeid(unsigned int))
        {
            m_Data.m_pUnsignedInts[flatElement] = v.GetValue<unsigned int>();
            return true;
        }

        if (m_Type == typeid(int))
        {
            m_Data.m_pInts[flatElement] = v.GetValue<int>();
            return true;
        }

        if (m_Type == typeid(float))
        {
            m_Data.m_pFloats[flatElement] = v.GetValue<float>();
            return true;
        }

        if (m_Type == typeid(std::string))
        {
            m_Data.m_pStrings[flatElement] = v.GetValue<std::string>();
            return true;
        }
    }

    // Types are not equal and are not string
    if (v.GetType() == typeid(char))
    {
        char* pData = ConvertT<char>();
        pData[flatElement] = v.GetValue<char>();
        m_Data.m_pChars = pData;
        m_Type = typeid(char);
    }
    else if (v.GetType() == typeid(unsigned int))
    {
        unsigned int* pData = ConvertT<unsigned int>();
        pData[flatElement] = v.GetValue<unsigned int>();
        m_Data.m_pUnsignedInts = pData;
        m_Type = typeid(unsigned int);
    }
    else if (v.GetType() == typeid(int))
    {
        int* pData = ConvertT<int>();
        pData[flatElement] = v.GetValue<int>();
        m_Data.m_pInts = pData;
        m_Type = typeid(int);
    }
    else if (v.GetType() == typeid(float))
    {
        float* pData = ConvertT<float>();
        pData[flatElement] = v.GetValue<float>();
        m_Data.m_pFloats = pData;
        m_Type = typeid(float);
    }
    else if (v.GetType() == typeid(std::string))
    {
        std::string* pData = ConvertT<std::string>();
        pData[flatElement] = v.GetValue<std::string>();
        m_Data.m_pStrings = pData;
        m_Type = typeid(std::string);
    }

    return true;
}

template<>
std::string* ArrayValue::ConvertT()
{
    assert(m_Type != typeid(std::string));

    std::string * pData = new std::string[GetElementCount()];
    assert(pData != nullptr);

    // if T is string, there is no conversion, just free what's there.
    if (m_Type == typeid(char))
    {
        delete[] m_Data.m_pChars;
    }

    if (m_Type == typeid(int))
    {
        delete[] m_Data.m_pInts;
    }

    if (m_Type == typeid(unsigned int))
    {
        delete[] m_Data.m_pUnsignedInts;
    }

    if (m_Type == typeid(float))
    {
        delete[] m_Data.m_pFloats;
    }

    return pData;
}

template<typename T>
T* ArrayValue::ConvertT()
{
    assert(m_Type != typeid(T));

    T* pData = new T[GetElementCount()];
    assert(pData != nullptr);

    // if T is string, there is no conversion, just free what's there.
    if (m_Type == typeid(char))
    {
        for (int i = 0; i < GetElementCount(); i++)
        {
            pData[i] = (T)m_Data.m_pChars[i];
        }
        delete[] m_Data.m_pChars;
    }

    if (m_Type == typeid(int))
    {
        for (int i = 0; i < GetElementCount(); i++)
        {
            pData[i] = (T)m_Data.m_pInts[i];
        }
        delete[] m_Data.m_pInts;
    }

    if (m_Type == typeid(unsigned int))
    {
        for (int i = 0; i < GetElementCount(); i++)
        {
            pData[i] = (T)m_Data.m_pUnsignedInts[i];
        }
        delete[] m_Data.m_pUnsignedInts;
    }

    if (m_Type == typeid(float))
    {
        for (int i = 0; i < GetElementCount(); i++)
        {
            pData[i] = (T)m_Data.m_pUnsignedInts[i];
        }
        delete[] m_Data.m_pFloats;
    }

    if (m_Type == typeid(std::string))
    {
        delete[] m_Data.m_pStrings;
    }

    return pData;
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
    if (m_Type == typeid(char))
    {
        return "CHAR";
    }

    if (m_Type == typeid(unsigned int))
    {
        return "UINT";
    }

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
        if ( m_Type == typeid(char))
        {
            sprintf_s(buf, sizeof(buf), "%c ", m_Data.m_pChars[i]);
        }
        else if ( m_Type == typeid(unsigned int))
        {
            sprintf_s(buf, sizeof(buf), "%uU ", m_Data.m_pUnsignedInts[i]);
        }
        else if ( m_Type == typeid(int))
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

void ArrayValue::FillStream(std::stringstream& rStream)
{
    const int ELEMENT_COUNT = 10;
    for (int i = 0; i < ELEMENT_COUNT && i < GetElementCount(); i++)
    {
        if (m_Type == typeid(char))
        {
            rStream << m_Data.m_pChars[i];
        }
        else if (m_Type == typeid(unsigned int))
        {
            rStream << m_Data.m_pUnsignedInts[i];
        }
        else if (m_Type == typeid(int))
        {
            rStream << m_Data.m_pInts[i];
        }
        else if (m_Type == typeid(float))
        {
            rStream << m_Data.m_pFloats[i];
        }
        else if (m_Type == typeid(std::string))
        {
            rStream << m_Data.m_pStrings[i];
        }
    }
    rStream << "...";
}

ArrayValue* ArrayValue::Clone()
{
    // Clone has memory allocated for the given type.
    ArrayValue* pClone = ArrayValue::Create(m_Dims, m_Type);
    assert(pClone != nullptr);

    // copy data over to clone's buffer.
    if (m_Type == typeid(char))
    {
        std::memcpy(pClone->m_Data.m_pChars, m_Data.m_pChars, sizeof(char) * GetElementCount());
    }
    else if (m_Type == typeid(unsigned int))
    {
        std::memcpy(pClone->m_Data.m_pUnsignedInts, m_Data.m_pUnsignedInts, sizeof(unsigned int) * GetElementCount());
    }
    else if (m_Type == typeid(int))
    {
        std::memcpy(pClone->m_Data.m_pInts, m_Data.m_pInts, sizeof(int) * GetElementCount());
    }
    else if (m_Type == typeid(float))
    {
        std::memcpy(pClone->m_Data.m_pFloats, m_Data.m_pFloats, sizeof(float) * GetElementCount());
    }
    else if (m_Type == typeid(std::string))
    {
        for (int i = 0; i < GetElementCount(); i++)
        {
            pClone->m_Data.m_pStrings[i] = m_Data.m_pStrings[i];
        }
    }
    
    return pClone;
}
}
