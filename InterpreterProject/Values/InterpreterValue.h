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
        Value(char c);
        Value(unsigned int ui);
        Value(int i);
        Value(float f);
        Value(std::string s);

        Value(const Value& v);

        ~Value();

        std::type_index GetType() { return m_Type; }

        template<typename T>
        void SetValue(T v)
        {
            m_Type = typeid(T);
            if (m_Type == typeid(char))
            {
                m_CharValue = static_cast<char>(v);
            }
            else if (m_Type == typeid(unsigned int))
            {
                m_UnsignedIntValue = static_cast<unsigned int>(v);
            }
            else if (m_Type == typeid(int))
            {
                m_IntValue = static_cast<int>(v);
            }
            else if (m_Type == typeid(float))
            {
                m_FloatValue = static_cast<float>(v);
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
            if (m_Type == typeid(char))
            {
                return (T) m_CharValue;
            }
            else if (m_Type == typeid(unsigned int))
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
        operator char();
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
        char m_CharValue;
        unsigned int m_UnsignedIntValue;
        int m_IntValue;
        float m_FloatValue;
        std::string m_StringValue;
        File* m_pFile;
    };
};