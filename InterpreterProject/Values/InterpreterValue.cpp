#include <sstream>
#include "InterpreterValue.h"
#include "File/InterpreterFile.h"

namespace Interpreter
{
    Value::Value() :
        m_Type(typeid(int)),
        m_IntValue(),
        m_FloatValue(),
        m_StringValue(),
        m_pFile(nullptr)
    {
    }

    Value::Value(int i) :
        m_Type(typeid(int)),
        m_IntValue(i),
        m_FloatValue(),
        m_StringValue(),
        m_pFile(nullptr)
    {
    }

    Value::Value(float f) :
        m_Type(typeid(float)),
        m_IntValue(),
        m_FloatValue(f),
        m_StringValue(),
        m_pFile(nullptr)
    {
    }

    Value::Value(std::string s) :
        m_Type(typeid(std::string)),
        m_IntValue(),
        m_FloatValue(),
        m_StringValue(s),
        m_pFile(nullptr)
    {
    }

    Value::Value(File* pFile) :
        m_Type(typeid(File*)),
        m_IntValue(),
        m_FloatValue(),
        m_StringValue(),
        m_pFile(pFile)
    {
    }

    Value::Value(const Value& v) :
        m_Type(v.m_Type),
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

    bool Value::Add(Value p)
    {
        std::type_index mytype = GetType();
        std::type_index othertype = p.GetType();

        if (mytype == typeid(int))
        {
            if (othertype == typeid(int))
            {
                m_IntValue += p.GetIntValue();
                return true;
            }

            if (othertype == typeid(float))
            {
                m_Type = typeid(float);
                m_FloatValue = m_IntValue + p.GetFloatValue();
                return true;
            }

            return false;
        }

        if (mytype == typeid(float))
        {
            if (othertype == typeid(int))
            {
                m_FloatValue += (float)p.GetIntValue();
                return true;
            }

            if (othertype == typeid(float))
            {
                m_FloatValue += p.GetFloatValue();
                return true;
            }

            return false;
        }

        if (mytype == typeid(std::string))
        {
            if (othertype == typeid(std::string))
            {
                m_StringValue += p.GetStringValue();
                return true;
            }

            return false;
        }

        return false;
    }

    bool Value::Sub(Value p)
    {
        std::type_index mytype = GetType();
        std::type_index othertype = p.GetType();

        if (mytype == typeid(int))
        {
            if (othertype == typeid(int))
            {
                m_IntValue -= p.GetIntValue();
                return true;
            }

            if (othertype == typeid(float))
            {
                m_Type = typeid(float);
                m_FloatValue = m_IntValue - p.GetFloatValue();
                return true;
            }

            return false;
        }

        if (mytype == typeid(float))
        {
            if (othertype == typeid(int))
            {
                m_FloatValue -= (float)p.GetIntValue();
                return true;
            }

            if (othertype == typeid(float))
            {
                m_FloatValue -= p.GetFloatValue();
                return true;
            }

            return false;
        }

        return false;
    }

    bool Value::Mul(Value p)
    {
        std::type_index mytype = GetType();
        std::type_index othertype = p.GetType();

        if (mytype == typeid(int))
        {
            if (othertype == typeid(int))
            {
                m_IntValue *= p.GetIntValue();
                return true;
            }

            if (othertype == typeid(float))
            {
                m_Type = typeid(float);
                m_FloatValue = m_IntValue * p.GetFloatValue();
                return true;
            }

            return false;
        }

        if (mytype == typeid(float))
        {
            if (othertype == typeid(int))
            {
                m_FloatValue *= (float)p.GetIntValue();
                return true;
            }

            if (othertype == typeid(float))
            {
                m_FloatValue *= p.GetFloatValue();
                return true;
            }

            return false;
        }

        return false;
    }

    bool Value::Div(Value p)
    {
        std::type_index mytype = GetType();
        std::type_index othertype = p.GetType();

        if (mytype == typeid(int))
        {
            if (othertype == typeid(int))
            {
                m_IntValue /= p.GetIntValue();
                return true;
            }

            if (othertype == typeid(float))
            {
                m_Type = typeid(float);
                m_FloatValue = m_IntValue / p.GetFloatValue();
                return true;
            }

            return false;
        }

        if (mytype == typeid(float))
        {
            if (othertype == typeid(int))
            {
                m_FloatValue /= (float)p.GetIntValue();
                return true;
            }

            if (othertype == typeid(float))
            {
                m_FloatValue /= p.GetFloatValue();
                return true;
            }

            return false;
        }

        return false;
    }

    bool Value::Neg()
    {
        std::type_index mytype = GetType();

        if (mytype == typeid(std::string))
        {
            return false;
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

    bool Value::Or(Value v)
    {
        std::type_index mytype = GetType();
        std::type_index theirtype = v.GetType();

        if (mytype == typeid(std::string) || mytype == typeid(float))
        {
            return false;
        }

        if (theirtype == typeid(std::string) || theirtype == typeid(float))
        {
            return false;
        }

        assert(mytype == typeid(int) && theirtype == typeid(int));
        m_IntValue |= v.GetIntValue();
        return true;
    }

    bool Value::And(Value v)
    {
        std::type_index mytype = GetType();
        std::type_index theirtype = v.GetType();

        if (mytype == typeid(std::string) || mytype == typeid(float))
        {
            return false;
        }

        if (theirtype == typeid(std::string) || theirtype == typeid(float))
        {
            return false;
        }

        assert(mytype == typeid(int) && theirtype == typeid(int));
        m_IntValue &= v.GetIntValue();
        return true;
    }

    bool Value::Xor(Value v)
    {
        std::type_index mytype = GetType();
        std::type_index theirtype = v.GetType();

        if (mytype == typeid(std::string) || mytype == typeid(float))
        {
            return false;
        }

        if (theirtype == typeid(std::string) || theirtype == typeid(float))
        {
            return false;
        }

        assert(mytype == typeid(int) && theirtype == typeid(int));
        m_IntValue ^= v.GetIntValue();
        return true;
    }

    bool Value::Lsh(Value v)
    {
        std::type_index mytype = GetType();
        std::type_index theirtype = v.GetType();

        if (mytype == typeid(std::string) || mytype == typeid(float))
        {
            return false;
        }

        if (theirtype == typeid(std::string) || theirtype == typeid(float))
        {
            return false;
        }

        assert(mytype == typeid(int) && theirtype == typeid(int));
        m_IntValue = m_IntValue << v.GetIntValue();
        return true;
    }

    bool Value::Rsh(Value v)
    {
        std::type_index mytype = GetType();
        std::type_index theirtype = v.GetType();

        if (mytype == typeid(std::string) || mytype == typeid(float))
        {
            return false;
        }

        if (theirtype == typeid(std::string) || theirtype == typeid(float))
        {
            return false;
        }

        assert(mytype == typeid(int) && theirtype == typeid(int));
        m_IntValue = m_IntValue >> v.GetIntValue();
        return true;
    }

    std::optional<bool> Value::Les(Value p)
    {
        std::type_index mytype = GetType();
        std::type_index othertype = p.GetType();

        if (mytype == typeid(int))
        {
            if (othertype == typeid(int))
            {
                return m_IntValue < p.GetIntValue();
            }

            if (othertype == typeid(float))
            {
                return m_IntValue < p.GetFloatValue();
            }

            return {};
        }

        if (mytype == typeid(float))
        {
            if (othertype == typeid(int))
            {
                return m_FloatValue < p.GetIntValue();
            }

            if (othertype == typeid(float))
            {
                return m_FloatValue < p.GetFloatValue();
            }

            return {};
        }

        if (mytype == typeid(std::string))
        {
            if (othertype == typeid(std::string))
            {
                return m_StringValue < p.GetStringValue();
            }

            return {};
        }

        return {};
    }

    std::optional<bool> Value::Leq(Value p)
    {
        std::type_index mytype = GetType();
        std::type_index othertype = p.GetType();

        if (mytype == typeid(int))
        {
            if (othertype == typeid(int))
            {
                return m_IntValue <= p.GetIntValue();
            }

            if (othertype == typeid(float))
            {
                return m_IntValue <= p.GetFloatValue();
            }

            return {};
        }

        if (mytype == typeid(float))
        {
            if (othertype == typeid(int))
            {
                return m_FloatValue <= p.GetIntValue();
            }

            if (othertype == typeid(float))
            {
                return m_FloatValue <= p.GetFloatValue();
            }

            return {};
        }

        if (mytype == typeid(std::string))
        {
            if (othertype == typeid(std::string))
            {
                return m_StringValue <= p.GetStringValue();
            }

            return {};
        }

        return {};
    }

    std::optional<bool> Value::Grt(Value p)
    {
        std::type_index mytype = GetType();
        std::type_index othertype = p.GetType();

        if (mytype == typeid(int))
        {
            if (othertype == typeid(int))
            {
                return m_IntValue > p.GetIntValue();
            }

            if (othertype == typeid(float))
            {
                return m_IntValue > p.GetFloatValue();
            }

            return {};
        }

        if (mytype == typeid(float))
        {
            if (othertype == typeid(int))
            {
                return m_FloatValue > p.GetIntValue();
            }

            if (othertype == typeid(float))
            {
                return m_FloatValue > p.GetFloatValue();
            }

            return {};
        }

        if (mytype == typeid(std::string))
        {
            if (othertype == typeid(std::string))
            {
                return m_StringValue > p.GetStringValue();
            }

            return {};
        }

        return {};
    }

    std::optional<bool> Value::Geq(Value p)
    {
        std::type_index mytype = GetType();
        std::type_index othertype = p.GetType();

        if (mytype == typeid(int))
        {
            if (othertype == typeid(int))
            {
                return m_IntValue >= p.GetIntValue();
            }

            if (othertype == typeid(float))
            {
                return m_IntValue >= p.GetFloatValue();
            }

            return {};
        }

        if (mytype == typeid(float))
        {
            if (othertype == typeid(int))
            {
                return m_FloatValue >= p.GetIntValue();
            }

            if (othertype == typeid(float))
            {
                return m_FloatValue >= p.GetFloatValue();
            }

            return {};
        }

        if (mytype == typeid(std::string))
        {
            if (othertype == typeid(std::string))
            {
                return m_StringValue >= p.GetStringValue();
            }

            return {};
        }

        return {};
    }

    std::optional<bool> Value::Deq(Value p)
    {
        std::type_index mytype = GetType();
        std::type_index othertype = p.GetType();

        if (mytype == typeid(int))
        {
            if (othertype == typeid(int))
            {
                return m_IntValue == p.GetIntValue();
            }

            if (othertype == typeid(float))
            {
                return m_IntValue == p.GetFloatValue();
            }

            return {};
        }

        if (mytype == typeid(float))
        {
            if (othertype == typeid(int))
            {
                return m_FloatValue == p.GetIntValue();
            }

            if (othertype == typeid(float))
            {
                return m_FloatValue == p.GetFloatValue();
            }

            return {};
        }

        if (mytype == typeid(std::string))
        {
            if (othertype == typeid(std::string))
            {
                return m_StringValue == p.GetStringValue();
            }

            return {};
        }

        return {};
    }

    std::optional<bool> Value::Neq(Value p)
    {
        std::type_index mytype = GetType();
        std::type_index othertype = p.GetType();

        if (mytype == typeid(int))
        {
            if (othertype == typeid(int))
            {
                return m_IntValue != p.GetIntValue();
            }

            if (othertype == typeid(float))
            {
                return m_IntValue != p.GetFloatValue();
            }

            return {};
        }

        if (mytype == typeid(float))
        {
            if (othertype == typeid(int))
            {
                return m_FloatValue != p.GetIntValue();
            }

            if (othertype == typeid(float))
            {
                return m_FloatValue != p.GetFloatValue();
            }

            return {};
        }

        if (mytype == typeid(std::string))
        {
            if (othertype == typeid(std::string))
            {
                return m_StringValue != p.GetStringValue();
            }

            return {};
        }

        return {};
    }

    std::optional<bool> Value::LogicalOr(Value p)
    {
        std::type_index mytype = GetType();
        std::type_index othertype = p.GetType();

        if (mytype != typeid(int) || othertype != typeid(int))
        {
            return {};
        }

        return GetIntValue() || p.GetIntValue();
   }

    std::optional<bool> Value::LogicalAnd(Value p)
    {
        std::type_index mytype = GetType();
        std::type_index othertype = p.GetType();

        if (mytype != typeid(int) || othertype != typeid(int))
        {
            return {};
        }

        return GetIntValue() && p.GetIntValue();
   }

    void Value::FillStream(std::stringstream& rStream)
    {
        if (m_Type == typeid(int))
        {
            rStream << GetIntValue();
        }
        else if (m_Type == typeid(float))
        {
            rStream << GetFloatValue();
        }
        else
        {
            assert(m_Type == typeid(std::string));
            rStream << GetStringValue();
        }
    } 
    int Value::ConvertBinary(const char* pStr)
    {
        int len = static_cast<int>(strlen(pStr));
        int result = 0;
        for (int i = len-1; i >= 0; i--)
        {
            int v = pStr[i] == '1' ? 1 : 0;
            result = result | (v<<i);
        }
        return result;
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