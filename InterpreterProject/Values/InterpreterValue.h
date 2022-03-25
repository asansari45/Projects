#pragma once
#include <string>
#include <typeindex>
#include <optional>

namespace Interpreter
{
    class File;

    class Value
    {
    public:
        Value();
        Value(File* pFile);
        Value(unsigned int ui);
        Value(int i);
        Value(float f);
        Value(std::string s);

        Value(const Value& v);

        ~Value();

        std::type_index GetType() { return m_Type; }
        void SetUnsignedIntValue(unsigned int value) { m_Type = typeid(unsigned int); m_UnsignedIntValue = value; }
        unsigned int GetUnsignedIntValue() { return m_UnsignedIntValue; }
        void SetIntValue(int value) { m_Type = typeid(int); m_IntValue = value; }
        int GetIntValue() { return m_IntValue; }
        void SetFloatValue(float value) { m_Type = typeid(float); m_FloatValue = value; }
        float GetFloatValue() { return m_FloatValue; }
        void SetStringValue(std::string value) { m_Type = typeid(std::string); m_StringValue = value; }
        std::string GetStringValue() { return m_StringValue; }
        void SetFileValue(File* pFile) { m_Type = typeid(File*);  m_pFile = pFile; }
        File* GetFileValue() { return m_pFile; }

        // Conversion routines
        operator unsigned int();
        operator int();
        operator float();
        operator std::string();

        bool IfEval();
        bool Neg();
        bool Bneg();

        std::string GetTypeRepr();
        std::string GetValueRepr();
        void FillStream(std::stringstream& rStream);

        // Helper methods for conversion
        static int ConvertBinary(const std::string s);
        static std::string HarvestControlChars(const char* pStr);
        static std::string RemoveUnderscores(const char* pStr);

    private:
        std::type_index m_Type;
        unsigned int m_UnsignedIntValue;
        int m_IntValue;
        float m_FloatValue;
        std::string m_StringValue;
        File* m_pFile;
    };
};