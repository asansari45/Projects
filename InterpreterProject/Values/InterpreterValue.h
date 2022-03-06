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
        Value(int i);
        Value(float f);
        Value(std::string s);

        Value(const Value& v);

        ~Value();

        std::type_index GetType() { return m_Type; }
        void SetIntValue(int value) { m_Type = typeid(int); m_IntValue = value; }
        int GetIntValue() { return m_IntValue; }
        void SetFloatValue(float value) { m_Type = typeid(float); m_FloatValue = value; }
        float GetFloatValue() { return m_FloatValue; }
        void SetStringValue(std::string value) { m_Type = typeid(std::string); m_StringValue = value; }
        std::string GetStringValue() { return m_StringValue; }
        void SetFileValue(File* pFile) { m_pFile = pFile; }
        File* GetFileValue() { return m_pFile; }

        bool IfEval();
        bool Add(Value& p);
        bool Sub(Value& p);
        bool Mul(Value& p);
        bool Div(Value& p);
        bool Neg();
        std::optional<bool> Les(Value& p);
        std::optional<bool> Leq(Value& p);
        std::optional<bool> Grt(Value& p);
        std::optional<bool> Geq(Value& p);
        std::optional<bool> Deq(Value& p);
        std::optional<bool> Neq(Value& p);

        std::string GetTypeRepr();
        std::string GetValueRepr();

    private:
        std::type_index m_Type;
        int m_IntValue;
        float m_FloatValue;
        std::string m_StringValue;
        File* m_pFile;
    };
};