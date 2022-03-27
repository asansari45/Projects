#include <sstream>
#include "InterpreterValue.h"
#include "File/InterpreterFile.h"

namespace Interpreter
{
    Value::Value() :
        m_Type(typeid(int)),
        m_CharValue(),
        m_UnsignedIntValue(),
        m_IntValue(),
        m_FloatValue(),
        m_StringValue(),
        m_pFile(nullptr)
    {
    }

    Value::Value(char c) :
        m_Type(typeid(char)),
        m_CharValue(c),
        m_UnsignedIntValue(),
        m_IntValue(),
        m_FloatValue(),
        m_StringValue(),
        m_pFile(nullptr)
    {
    }

    Value::Value(unsigned int ui) :
        m_Type(typeid(unsigned int)),
        m_CharValue(),
        m_UnsignedIntValue(ui),
        m_IntValue(),
        m_FloatValue(),
        m_StringValue(),
        m_pFile(nullptr)
    {
    }

    Value::Value(int i) :
        m_Type(typeid(int)),
        m_CharValue(),
        m_UnsignedIntValue(),
        m_IntValue(i),
        m_FloatValue(),
        m_StringValue(),
        m_pFile(nullptr)
    {
    }

    Value::Value(float f) :
        m_Type(typeid(float)),
        m_CharValue(),
        m_UnsignedIntValue(),
        m_IntValue(),
        m_FloatValue(f),
        m_StringValue(),
        m_pFile(nullptr)
    {
    }

    Value::Value(std::string s) :
        m_Type(typeid(std::string)),
        m_CharValue(),
        m_UnsignedIntValue(),
        m_IntValue(),
        m_FloatValue(),
        m_StringValue(s),
        m_pFile(nullptr)
    {
    }

    Value::Value(File* pFile) :
        m_Type(typeid(File*)),
        m_CharValue(),
        m_UnsignedIntValue(),
        m_IntValue(),
        m_FloatValue(),
        m_StringValue(),
        m_pFile(pFile)
    {
    }

    Value::Value(const Value& v) :
        m_Type(v.m_Type),
        m_CharValue(v.m_CharValue),
        m_UnsignedIntValue(v.m_UnsignedIntValue),
        m_IntValue(v.m_IntValue),
        m_FloatValue(v.m_FloatValue),
        m_StringValue(v.m_StringValue),
        m_pFile(v.m_pFile)
    {
    }

    Value::~Value()
    {
    }

    std::string Value::GetTypeRepr()
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

        if (m_Type == typeid(File*))
        {
            return "FILE";
        }

        return "STR";
    }

    std::string Value::GetValueRepr()
    {
        char buf[256];
        if (m_Type == typeid(char))
        {
            sprintf_s(buf, sizeof(buf), "%c", m_CharValue);
            return std::string(buf);
        }

        if (m_Type == typeid(unsigned int))
        {
            sprintf_s(buf, sizeof(buf), "%uU", m_UnsignedIntValue);
            return std::string(buf);
        }

        if (m_Type == typeid(int))
        {
            sprintf_s(buf, sizeof(buf), "%d", m_IntValue);
            return std::string(buf);
        }

        if (m_Type == typeid(float))
        {
            sprintf_s(buf, sizeof(buf), "%g", m_FloatValue);
            return std::string(buf);
        }

        if (m_Type == typeid(File*))
        {
            sprintf_s(buf, sizeof(buf), "%p", m_pFile);
            return std::string(buf);
        }

        return m_StringValue;
    }

    bool Value::IfEval()
    {
        if (m_Type == typeid(char))
        {
            return m_CharValue != 0;
        }

        if (m_Type == typeid(unsigned int))
        {
            return m_UnsignedIntValue != 0;
        }

        if (m_Type == typeid(int))
        {
            return m_IntValue != 0;
        }

        if (m_Type == typeid(float))
        {
            return m_FloatValue != 0;
        }

        if (m_Type == typeid(File*))
        {
            return m_pFile != nullptr;
        }

        return m_StringValue.size() != 0;
    }

    Value::operator char()
    {
        assert(m_Type != typeid(std::string));
        if (m_Type == typeid(char))
        {
            return m_CharValue;
        }

        if (m_Type == typeid(unsigned int))
        {
            return m_UnsignedIntValue;
        }

        if (m_Type == typeid(int))
        {
            return static_cast<unsigned int>(m_IntValue);
        }

        if (m_Type == typeid(float))
        {
            return static_cast<unsigned int>(m_FloatValue);
        }

        assert(false);
        return 0;
    }

    Value::operator unsigned int()
    {
        assert(m_Type != typeid(std::string));
        if (m_Type == typeid(char))
        {
            return m_CharValue;
        }

        if (m_Type == typeid(unsigned int))
        {
            return m_UnsignedIntValue;
        }

        if (m_Type == typeid(int))
        {
            return static_cast<unsigned int>(m_IntValue);
        }

        if (m_Type == typeid(float))
        {
            return static_cast<unsigned int>(m_FloatValue);
        }

        assert(false);
        return 0;
    }
    
    Value::operator int()
    {
        assert(m_Type != typeid(std::string));
        if (m_Type == typeid(char))
        {
            return m_CharValue;
        }

        if (m_Type == typeid(unsigned int))
        {
            return static_cast<int>(m_UnsignedIntValue);
        }

        if (m_Type == typeid(int))
        {
            return m_IntValue;
        }

        if (m_Type == typeid(float))
        {
            return static_cast<int>(m_FloatValue);
        }

        assert(false);
        return 0;
    }
    
    Value::operator float()
    {
        assert(m_Type != typeid(std::string));
        if (m_Type == typeid(char))
        {
            return m_CharValue;
        }

        if (m_Type == typeid(unsigned int))
        {
            return static_cast<float>(m_UnsignedIntValue);
        }

        if (m_Type == typeid(int))
        {
            return static_cast<float>(m_IntValue);
        }

        if (m_Type == typeid(float))
        {
            return m_FloatValue;
        }

        assert(false);
        return 0.0;
    }

    Value::operator std::string()
    {
        assert(m_Type == typeid(std::string));
        return m_StringValue;
    }

    bool Value::Neg()
    {
        std::type_index mytype = GetType();

        if (mytype == typeid(std::string))
        {
            return false;
        }

        if (mytype == typeid(unsigned int))
        {
            return false;
        }

        if (mytype == typeid(char))
        {
            m_CharValue = -m_CharValue;
        }

        if (mytype == typeid(int))
        {
            m_IntValue = -m_IntValue;
        }

        if (mytype == typeid(float))
        {
            m_FloatValue = -m_FloatValue;
        }

        return true;
    }

    bool Value::Bneg()
    {
        std::type_index mytype = GetType();

        if (mytype == typeid(std::string) || mytype == typeid(float))
        {
            return false;
        }

        if (m_Type == typeid(char))
        {
            m_CharValue = ~m_CharValue;
            return true;
        }

        if (m_Type == typeid(unsigned int))
        {
            m_UnsignedIntValue = ~m_UnsignedIntValue;
            return true;
        }

        assert(m_Type == typeid(int));
        m_IntValue = ~m_IntValue;
        return true;
    }

    void Value::FillStream(std::stringstream& rStream)
    {
        if (m_Type == typeid(char))
        {
            rStream << GetValue<char>();
        }
        if (m_Type == typeid(unsigned int))
        {
            rStream << GetValue<unsigned int>();
        }
        else if (m_Type == typeid(int))
        {
            rStream << GetValue<int>();
        }
        else if (m_Type == typeid(float))
        {
            rStream << GetValue<float>();
        }
        else
        {
            assert(m_Type == typeid(std::string));
            rStream << GetValue<std::string>();
        }
    } 

    int Value::ConvertBinary(const std::string s)
    {
        int result = 0;
        for (int i = 0; i < s.size(); i++)
        {
            if (s[i] == '1' || s[i] == '0')
            {
                result = result | ((s[i] - '0')<<(s.size()-1-i));
            }
        }
        return result;
    }

    std::string Value::RemoveUnderscores(const char* pStr)
    {
        std::string s;
        int len = static_cast<int>(strlen(pStr));
        for ( int i = 0; i < len; i++)
        {
            if (pStr[i] != '_')
            {
                s += pStr[i];
            }
        }

        return s;
    }

    std::string Value::HarvestControlChars(const char* pStr)
    {
        std::string s;
        int i = 0;
        size_t len = strlen(pStr);
        while (len >= 2)
        {
            if (pStr[i] == '\\' && pStr[i+1] == 'n')
            {
                s += '\n';
                i += 2;
                len -= 2;
            }
            else
            {
                s += pStr[i];
                i++;
                len--;
            }
        }
        assert(len == 0 || len == 1);
        if (len != 0)
        {
            s += pStr[strlen(pStr)-1];
        }

        return s;
    }
}

