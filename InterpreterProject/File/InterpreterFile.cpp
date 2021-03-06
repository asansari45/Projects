#include <cassert>
#include <memory>
#include "InterpreterFile.h"

namespace Interpreter
{

File::FILEMAP File::m_FileMap;

#if 0
File* File::Open(std::string name, std::string fileName, std::string mode)
{
    if (m_FileMap.find(name) != m_FileMap.end())
    {
        // already have that one.
        return nullptr;
    }

    FILE* pFile = nullptr;
    fopen_s(&pFile, fileName.c_str(), mode.c_str());
    if (pFile == nullptr)
    {
        return nullptr;
    }

    File* pRetFile = new File(name, pFile);
    assert(pRetFile != nullptr);
    m_FileMap.insert(std::make_pair(name, pRetFile));
    return pRetFile;
}
#endif

File* File::Open(std::string fileName, std::string mode)
{
    FILE* pFile = nullptr;
    fopen_s(&pFile, fileName.c_str(), mode.c_str());
    if (pFile == nullptr)
    {
        return nullptr;
    }

    File* pRetFile = new File(pFile);
    assert(pRetFile != nullptr);
    return pRetFile;
}

File* File::Get(std::string name)
{
    FILEMAP::iterator i = m_FileMap.find(name);
    if (i != m_FileMap.end())
    {
        return i->second;
    }

    return nullptr;
}

void File::Close()
{
    fclose(m_pFile);
#if 0
    // FILEMAP::iterator i = m_FileMap.find(m_Name);
    if (i != m_FileMap.end())
    {
        delete this;
        m_FileMap.erase(i);
    }
#endif

}

bool File::Write(Value v)
{
    // Write the header first
    Header hdr;
    hdr.m_Array = false;

    if (v.GetType() == typeid(int))
    {
        hdr.m_SubType = INTEGER;
    }
    else if (v.GetType() == typeid(float))
    {
        hdr.m_SubType = FLOAT;
    }
    else 
    {
        assert(v.GetType() == typeid(std::string));
        hdr.m_SubType = STRING;
    }

    hdr.m_Dims[0] = 0;
    hdr.m_Dims[1] = 0;
    hdr.m_Dims[2] = 0;
    size_t elementsWritten = fwrite(&hdr, sizeof(hdr), 1, m_pFile);
    if (elementsWritten != 1)
    {
        return false;
    }

    // Write the value second.
    if (v.GetType() == typeid(int))
    {
        int value = v.GetValue<int>();
        elementsWritten = fwrite(&value, sizeof(value), 1, m_pFile);
    }
    else if (v.GetType() == typeid(float))
    {
        float value = v.GetValue<float>();
        elementsWritten = fwrite(&value, sizeof(value), 1, m_pFile);
    }
    else if (v.GetType() == typeid(std::string))
    {
        assert(v.GetType() == typeid(std::string));
        size_t sz = v.GetValue<std::string>().size();
        elementsWritten = fwrite(&sz, sizeof(sz), 1, m_pFile);
        if (elementsWritten != 1)
        {
            return false;
        }

        if (sz != 0)
        {
            elementsWritten = fwrite(v.GetValue<std::string>().c_str(), 1, sz, m_pFile);
        }

        if (elementsWritten != sz)
        {
            return false;
        }

    }
    else
    {
        return false;
    }

    return true;
}

bool File::Write(ArrayValue* v)
{
    // Write the header first
    Header hdr;
    hdr.m_Array = true;
    if (v->GetType() == typeid(int))
    {
        hdr.m_SubType = INTEGER;
    }
    else if (v->GetType() == typeid(float))
    {
        hdr.m_SubType = FLOAT;
    }
    else if (v->GetType() == typeid(std::string)) 
    {
        assert(v->GetType() == typeid(std::string));
        hdr.m_SubType = STRING;
    }
    else
    {
        return false;
    }

    hdr.m_Dims[0] = 0;
    hdr.m_Dims[1] = 0;
    hdr.m_Dims[2] = 0;
    std::vector<int> dims = v->GetDims();
    int elementCount = 0;
    if (dims.size() == 3)
    {
        hdr.m_Dims[0] = dims[0];
        hdr.m_Dims[1] = dims[1];
        hdr.m_Dims[2] = dims[2];
        elementCount = dims[0] * dims[1] * dims[2];
    }
    else if (dims.size() == 2)
    {
        hdr.m_Dims[0] = dims[0];
        hdr.m_Dims[1] = dims[1];
        elementCount = dims[0] * dims[1];
    }
    else 
    {
        assert(dims.size() == 1);
        hdr.m_Dims[0] = dims[0];
        elementCount = dims[0];
    }

    size_t elementsWritten = fwrite(&hdr, sizeof(hdr), 1, m_pFile);
    if (elementsWritten != 1)
    {
        return false;
    }

    // Write the value second.
    if (v->GetType() == typeid(int))
    {
        elementsWritten = fwrite(v->GetIntData(), sizeof(int), elementCount, m_pFile);
        if (elementsWritten != elementCount)
        {
            return false;
        }
    }
    else if (v->GetType() == typeid(float))
    {
        elementsWritten = fwrite(v->GetFloatData(), sizeof(float), elementCount, m_pFile);
        if (elementsWritten != elementCount)
        {
            return false;
        }
    }
    else 
    {
        assert(v->GetType() == typeid(std::string));
        std::string* pStrings = v->GetStringData();
        for ( int i = 0; i < elementCount; i++)
        {
            std::string s = pStrings[i];
            size_t sz = s.size();
            elementsWritten = fwrite(&sz, sizeof(sz), 1, m_pFile);
            if (elementsWritten != 1)
            {
                return false;
            }

            if (sz != 0)
            {
                elementsWritten = fwrite(s.c_str(), sizeof(s.c_str()[0]), sz, m_pFile);
                if (elementsWritten != sz)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

bool File::Read(bool& rArray, Value& rValue, ArrayValue** ppArrayValue)
{
    // Read the header from the file.
    Header hdr;
    size_t elementsRead = fread(&hdr, sizeof(hdr), 1, m_pFile);
    if (elementsRead != 1)
    {
        return false;
    }

    rArray = hdr.m_Array;

    if (rArray)
    {
        std::vector<int> dims;
        int elementCount = 1;
        for (int i=0; i < 3; i++)
        {
            if (hdr.m_Dims[i] != 0)
            {
                dims.push_back(hdr.m_Dims[i]);
                elementCount *= hdr.m_Dims[i];
            }
        }

        std::unique_ptr<ArrayValue> pArrValue;

        if (hdr.m_SubType == INTEGER)
        {
            pArrValue.reset(ArrayValue::Create(dims, typeid(int)));
            elementsRead = fread(pArrValue->GetIntData(), sizeof(int), elementCount, m_pFile);
            if (elementsRead != elementCount)
            {
                return false;
            }
            *ppArrayValue = pArrValue.release();
            return true;
        }
        
        if (hdr.m_SubType == FLOAT)
        {
            pArrValue.reset(ArrayValue::Create(dims, typeid(float)));
            elementsRead = fread(pArrValue->GetFloatData(), sizeof(float), elementCount, m_pFile);
            if (elementsRead != elementCount)
            {
                return false;
            }
            *ppArrayValue = pArrValue.release();
            return true;
        }
        
        if (hdr.m_SubType == STRING)
        {
            pArrValue.reset(ArrayValue::Create(dims, typeid(std::string)));
            // todo fix max_string
            const int MAX_STRING = 1024;
            char s[MAX_STRING];
            std::string* pStringData = pArrValue->GetStringData();
            for (int i=0; i < elementCount; i++)
            {
                // Read size of string
                size_t sz=0;
                elementsRead = fread(&sz, sizeof(sz), 1, m_pFile);
                if (elementsRead != 1)
                {
                    return false;
                }

                elementsRead = fread(s, 1, sz, m_pFile);
                if (elementsRead != sz)
                {
                    return false;
                }

                std::string finalString(s, sz);
                pStringData[i] = finalString;
            }

            *ppArrayValue = pArrValue.release();
            return true;
        }
        return false;
    }

    if (hdr.m_SubType == INTEGER)
    {
        int v = 0;
        elementsRead = fread(&v, sizeof(v), 1, m_pFile);
        if (elementsRead != 1)
        {
            return false;
        }
        rValue.SetValue(v);
        return true;
    }

    if (hdr.m_SubType == FLOAT)
    {
        float v = 0;
        elementsRead = fread(&v, sizeof(v), 1, m_pFile);
        if (elementsRead != 1)
        {
            return false;
        }
        rValue.SetValue(v);
        return true;
    }

    if (hdr.m_SubType == STRING)
    {
        size_t sz;
        const int MAX_STRING = 1024;
        char s[MAX_STRING];
        elementsRead = fread(&sz, sizeof(sz), 1, m_pFile);
        if (elementsRead != 1)
        {
            return false;
        }

        elementsRead = fread(s, 1, sz, m_pFile);
        if (elementsRead != sz)
        {
            return false;
        }

        std::string v(s, sz);
        rValue.SetValue(v);
        return true;
    }

    return false;
}

bool File::Eof()
{
    return feof(m_pFile) != 0;
}

}

