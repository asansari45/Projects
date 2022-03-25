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
    int GetElementCount();

    std::optional<Value> GetValue(std::vector<int> element);
    std::optional<Value> GetValue(int flatElement);
    bool SetValue(std::vector<int> element, Value v);
    bool SetValue(int flatElement, Value v);

    unsigned int* GetUnsignedIntData(){ return m_Data.m_pUnsignedInts; }
    int* GetIntData(){ return m_Data.m_pInts; }
    float* GetFloatData() { return m_Data.m_pFloats; }
    std::string* GetStringData() { return m_Data.m_pStrings; }

    std::type_index GetType();

    // For debug purposes
    std::string GetTypeRepr();
    std::string GetDimsRepr();
    std::string GetValuesRepr();
    void FillStream(std::stringstream& rStream);

private:
    ArrayValue(std::vector<int> dims, std::type_index t);
    ArrayValue(const ArrayValue& p)=delete;

    // Convert element index to flag index
    std::optional<int> ConvertElementIndex(std::vector<int> element);

    // Convert and then free.
    unsigned int* ConvertFreeUnsignedInt();
    int* ConvertFreeInt();
    float* ConvertFreeFloat();

    // Flat array for the values based on the dimension.
    std::vector<int> m_Dims;

    // Could be int, float, or string
    std::type_index m_Type;
    union Type
    {
        unsigned int* m_pUnsignedInts;
        int* m_pInts;
        float* m_pFloats;
        std::string* m_pStrings;
    };

    Type m_Data;
};

};
