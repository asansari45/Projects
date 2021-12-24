#pragma once
#include <string>
#include <typeindex>

namespace Interpreter
{
    class Value
    {
    public:
        Value() :
            m_Type(typeid(int)),
            m_IntValue(),
            m_FloatValue(),
            m_StringValue()
        {
        }

        Value(const Value& v) :
            m_Type(v.m_Type),
            m_IntValue(v.m_IntValue),
            m_FloatValue(v.m_FloatValue),
            m_StringValue(v.m_StringValue)
        {
        }

        ~Value()
        {
        }

        std::type_index GetType() { return m_Type; }
        void SetIntValue(int value) { m_Type = typeid(int); m_IntValue = value; }
        int GetIntValue() { return m_IntValue; }
        void SetFloatValue(float value) { m_Type = typeid(float); m_FloatValue = value; }
        float GetFloatValue() { return m_FloatValue; }
        void SetStringValue(std::string value) { m_Type = typeid(std::string); m_StringValue = value; }
        std::string GetStringValue() { return m_StringValue; }

        std::string GetRepresentation()
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

            return m_StringValue;
        }

        bool IfEval()
        {
            if (m_Type == typeid(int))
            {
                return m_IntValue != 0;
            }

            if (m_Type == typeid(float))
            {
                return m_FloatValue != 0;
            }

            return m_StringValue.size() != 0;
        }

        bool Add(Value& p)
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

        bool Sub(Value& p)
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

        bool Mul(Value& p)
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

        bool Div(Value& p)
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

        bool Les(Value& p)
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

                return false;
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

                return false;
            }

            if (mytype == typeid(std::string))
            {
                if (othertype == typeid(std::string))
                {
                    return m_StringValue < p.GetStringValue();
                }

                return false;
            }

            return false;
        }

        bool Leq(Value& p)
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

                return false;
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

                return false;
            }

            if (mytype == typeid(std::string))
            {
                if (othertype == typeid(std::string))
                {
                    return m_StringValue <= p.GetStringValue();
                }

                return false;
            }

            return false;
        }

        bool Grt(Value& p)
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

                return false;
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

                return false;
            }

            if (mytype == typeid(std::string))
            {
                if (othertype == typeid(std::string))
                {
                    return m_StringValue > p.GetStringValue();
                }

                return false;
            }

            return false;
        }

        bool Geq(Value& p)
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

                return false;
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

                return false;
            }

            if (mytype == typeid(std::string))
            {
                if (othertype == typeid(std::string))
                {
                    return m_StringValue >= p.GetStringValue();
                }

                return false;
            }

            return false;
        }

        bool Deq(Value& p)
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

                return false;
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

                return false;
            }

            if (mytype == typeid(std::string))
            {
                if (othertype == typeid(std::string))
                {
                    return m_StringValue == p.GetStringValue();
                }

                return false;
            }

            return false;
        }

        bool Neq(Value& p)
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

                return false;
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

                return false;
            }

            if (mytype == typeid(std::string))
            {
                if (othertype == typeid(std::string))
                {
                    return m_StringValue != p.GetStringValue();
                }

                return false;
            }

            return false;
        }

        bool Neg()
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

private:
        std::type_index m_Type;
        int m_IntValue;
        float m_FloatValue;
        std::string m_StringValue;
    };
}