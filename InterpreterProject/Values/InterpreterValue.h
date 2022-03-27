#pragma once
#include <cassert>
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
        #if 0
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
        #endif
        
        template<typename T>
        void SetValue(T v)
        {
            m_Type = typeid(T);
            if (m_Type == typeid(unsigned int))
            {
                m_UnsignedIntValue = v;
            }
            else if (m_Type == typeid(int))
            {
                m_IntValue = v;
            }
            else if (m_Type == typeid(float))
            {
                m_FloatValue = v;
            }
        }

        template<>
        void SetValue(std::string s)
        {
            m_Type = typeid(std::string);
            m_StringValue = s;
        }

        template<>
        void SetValue(File* pFile)
        {
            m_Type = typeid(File*);
            m_pFile = pFile;
        }

        template<typename T>
        T GetValue()
        {
            if (m_Type == typeid(unsigned int))
            {
                return (T) m_UnsignedIntValue;
            }
            else if (m_Type == typeid(int))
            {
                return (T) m_IntValue;
            }
            else if (m_Type == typeid(float))
            {
                return (T) m_FloatValue;
            }
            assert(false);
            return (T)0;
        }

        template<>
        std::string GetValue()
        {
            return m_StringValue;
        }

        template<>
        File* GetValue()
        {
            return m_pFile;
        }

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