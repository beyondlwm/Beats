#include "stdafx.h"
#include <string>
#include "Serializer.h"
#include "FilePath/FilePathTool.h"

CSerializer::CSerializer(size_t size, void* data)
: m_size(size)
, m_pBuffer(NULL)
, m_pReadPtr(NULL)
, m_pWritePtr(NULL)
, m_pUserData(NULL)
{
    Create(size, data);
}

CSerializer::CSerializer( const CSerializer& rhs )
: m_size(0)
, m_pBuffer(NULL)
, m_pReadPtr(NULL)
, m_pWritePtr(NULL)
, m_pUserData(NULL)
{
    Create(rhs.m_size, rhs.m_pBuffer);
    SetReadPos(rhs.GetReadPos());
    SetWritePos(rhs.GetWritePos());
}

CSerializer::CSerializer(const TCHAR* pFilePath, const TCHAR* pszMode /*= _T("rb")*/, size_t uStartPos/* = 0*/, size_t uDataLength/* = 0*/)
: m_size(0)
, m_pBuffer(NULL)
, m_pReadPtr(NULL)
, m_pWritePtr(NULL)
, m_pUserData(NULL)
{
    Serialize(pFilePath, pszMode, uStartPos, uDataLength);
}

CSerializer::CSerializer(FILE* pFile, size_t startPos, size_t dataLength)
: m_size(0)
, m_pBuffer(NULL)
, m_pReadPtr(NULL)
, m_pWritePtr(NULL)
, m_pUserData(NULL)
{
    Serialize(pFile, startPos, dataLength);
}

CSerializer::~CSerializer()
{
    Release();
}

CSerializer& CSerializer::operator = (const CSerializer& rhs)
{
    ValidateBuffer(m_size);
    memcpy(m_pBuffer, rhs.GetBuffer(), rhs.GetWritePos());
    SetReadPos(rhs.GetReadPos());
    SetWritePos(rhs.GetWritePos());
    return *this;
}

void CSerializer::SetReadPos(size_t offset)
{
    m_pReadPtr = static_cast<unsigned char*>(m_pBuffer) + offset;
    BEATS_ASSERT(m_pReadPtr <= m_pWritePtr);
}

void CSerializer::SetWritePos(size_t offset)
{
    m_pWritePtr = static_cast<unsigned char*>(m_pBuffer) + offset;
    BEATS_ASSERT(m_pWritePtr <= static_cast<unsigned char*>(m_pBuffer) + m_size);
}

size_t CSerializer::GetReadPos() const
{
    return static_cast<unsigned char*>(m_pReadPtr) - static_cast<unsigned char*>(m_pBuffer);
}

size_t CSerializer::GetWritePos() const
{
    return static_cast<unsigned char*>(m_pWritePtr) - static_cast<unsigned char*>(m_pBuffer);
}

void* CSerializer::GetReadPtr() const
{
    return m_pReadPtr;
}

void* CSerializer::GetWritePtr() const
{
    return m_pWritePtr;
}

size_t CSerializer::GetBufferSize() const
{
    return m_size;
}

void CSerializer::Reset()
{
    m_pWritePtr = m_pBuffer;
    m_pReadPtr = m_pBuffer;
}

const unsigned char* CSerializer::GetBuffer() const
{
    return static_cast<unsigned char*>(m_pBuffer);
}

void CSerializer::Serialize( const void* pData, size_t size )
{
    ValidateBuffer(size);
    memcpy(m_pWritePtr, pData, size);
    m_pWritePtr = static_cast<unsigned char*>(m_pWritePtr) + size;

}

bool CSerializer::Serialize( const TCHAR* pFilePath, const TCHAR* pszMode /* = _T("rb")*/, size_t uStartPos/* = 0*/, size_t uDataLength/* = 0*/)
{
    return CFilePathTool::GetInstance()->LoadFile(this, pFilePath, pszMode, uStartPos, uDataLength);
}

bool CSerializer::Serialize( FILE* pFile, size_t startPos /*= 0*/, size_t dataLength /*= 0*/ )
{
    bool bRet = false;
    BEATS_ASSERT(pFile != NULL);
    size_t curPos = ftell(pFile);
    fseek(pFile, 0, SEEK_END);
    size_t uFileSize = ftell(pFile);
    BEATS_WARNING(uFileSize > 0, _T("Trying to serialize an empty file!"));
    if (uFileSize > 0)
    {
        BEATS_ASSERT(uFileSize > startPos);
        if (uFileSize > startPos)
        {
            fseek(pFile, (long)startPos, SEEK_SET);
            dataLength = ((dataLength != 0) && (uFileSize - startPos > dataLength)) ? dataLength : uFileSize - startPos;
            ValidateBuffer(dataLength);
            fread(m_pBuffer, 1, dataLength, pFile);
            m_pWritePtr = static_cast<unsigned char*>(m_pBuffer) + dataLength;
            bRet = true;
        }
        fseek(pFile, (long)curPos, SEEK_SET);
    }
    return bRet;
}

void CSerializer::Serialize(CSerializer& serializer, size_t uDataSize/* = 0xFFFFFFFF*/)
{
    if(uDataSize > 0)
    {
        size_t uReadPos = serializer.GetReadPos();
        if (uDataSize == 0xFFFFFFFF)
        {
            uDataSize = serializer.GetWritePos() - uReadPos;
        }
        this->Serialize(serializer.GetReadPtr(), uDataSize);
        serializer.SetReadPos(uReadPos + uDataSize);
    }        
}

void CSerializer::RewriteData(size_t uPos, void* pData, size_t uDataSize)
{
    size_t uCurWritePos = GetWritePos();
    bool bIsPosValid = uPos < uCurWritePos;
    BEATS_ASSERT(bIsPosValid, _T("Rewrite means we can only write on the data which we have wrote."));
    if (bIsPosValid)
    {
        if (uDataSize + uPos > m_size)
        {
            ValidateBuffer(uDataSize + uPos - m_size);
        }
        memcpy(static_cast<unsigned char*>(m_pBuffer) + uPos, pData, uDataSize);
    }
}

size_t CSerializer::Deserialize( void* pData, size_t size )
{
    size_t uCurReadPos = GetReadPos();
    size_t uDataSize = GetWritePos();
    if(uCurReadPos + size > uDataSize)
    {
        size = uDataSize - uCurReadPos;
    }
    memcpy(pData, m_pReadPtr, size);
    m_pReadPtr = static_cast<unsigned char*>(m_pReadPtr) + size;

    return size;
}

bool CSerializer::Deserialize( const TCHAR* pFilePath, const TCHAR* pszMode/* = _T("wb+")*/ )
{
    BEATS_ASSERT(pFilePath != NULL, _T("File path is NULL!"));
    FILE* pFile = _tfopen(pFilePath, pszMode);
    BEATS_ASSERT(pFile != NULL, _T("Can't open file %s"), pFilePath);
    if (pFile != NULL)
    {
        fwrite(m_pReadPtr, GetWritePos() - GetReadPos(), sizeof(unsigned char), pFile);
        m_pReadPtr = m_pWritePtr;
        fclose(pFile);
    }
    return pFile != NULL;

}

void CSerializer::Deserialize(CSerializer& serializer, size_t uDataSize/* = 0xFFFFFFFF*/)
{
    serializer.Serialize(*this, uDataSize);
}

void CSerializer::ValidateBuffer(size_t size)
{
    BEATS_ASSERT(size > 0, _T("size can't be less than zero£¡ current size: %d"), size);
    bool overFlow = false;
    size_t wrottenCount = GetWritePos();
    while (wrottenCount + size > m_size)
    {
        if (m_size == 0)
        {
            m_size = 1;
        }
        m_size *= 2;
        overFlow = true;
    }
    if (overFlow)
    {
        size_t readCount = static_cast<unsigned char*>(m_pReadPtr) - static_cast<unsigned char*>(m_pBuffer);
        unsigned char* pNewBuffer = new unsigned char[m_size];
        memcpy(pNewBuffer, m_pBuffer, wrottenCount);
        BEATS_SAFE_DELETE_ARRAY(m_pBuffer);
        m_pBuffer = pNewBuffer;
        m_pWritePtr = static_cast<unsigned char*>(m_pBuffer) + wrottenCount;
        m_pReadPtr = static_cast<unsigned char*>(m_pBuffer) + readCount;
    }
    BEATS_ASSERT(m_pBuffer != NULL && m_pWritePtr != NULL && m_pReadPtr != NULL, _T("Invalid serliazer buffer!"));
}

void CSerializer::Create( size_t size /*= DefaultSerializerBufferSize*/, void* data /*= NULL*/ )
{
    BEATS_ASSERT(m_pBuffer == NULL, _T("Can't create a serliazer twice, Try to release it before!"));
    m_pBuffer = new unsigned char[size];
    m_pWritePtr = m_pBuffer;
    m_pReadPtr = m_pBuffer;
    if (data != NULL)
    {
        memcpy(m_pBuffer, data, size);
        m_pWritePtr = static_cast<unsigned char*>(m_pBuffer) + size;
    }
    BEATS_ASSERT(m_pBuffer != NULL && m_pWritePtr != NULL && m_pReadPtr != NULL, _T("Invalid serliazer buffer!"));
}

void CSerializer::Release()
{
    BEATS_SAFE_DELETE_ARRAY(m_pBuffer);
    m_pBuffer = NULL;
    m_pReadPtr = NULL;
    m_pWritePtr = NULL;
}
void CSerializer::SerializeCharPtr(const char* data)
{
    BEATS_ASSERT(data != NULL, _T("Data is null to serialize"));
    size_t size = (strlen(data) + 1) * sizeof(char);
    ValidateBuffer(size);
    memcpy(m_pWritePtr, data, size);
    m_pWritePtr = static_cast<unsigned char*>(m_pWritePtr) + size;
}

void CSerializer::SerializeWcharPtr(const wchar_t* data)
{
    BEATS_ASSERT(data != NULL, _T("Data is null to serialize"));
    size_t size = (wcslen(data) + 1) * sizeof(wchar_t);
    ValidateBuffer(size);
    memcpy(m_pWritePtr, data, size);
    m_pWritePtr = static_cast<unsigned char*>(m_pWritePtr) + size;
}

void CSerializer::UnitTest()
{
    CSerializer serializer;

    // Test string.
    serializer << "1";
    serializer << L"2";
    char* pChar = "3";
    serializer << pChar;
    const char* pConstChar = "4";
    serializer << pConstChar;
    wchar_t* pWChar = L"5";

    serializer << pWChar;
    const wchar_t* pConstWChar = L"6";
    serializer << pConstWChar;
    char charArray[128] = "777";
    serializer << charArray;
    const char constCharArray[128] = "8888";
    serializer << constCharArray;
    wchar_t wcharArray[128] = L"999";
    serializer << wcharArray;
    const wchar_t constWcharhArray[128] = L"aaa";
    serializer << constWcharhArray;
    std::string sStr("this is a test str.");
    serializer << sStr;
    std::wstring sWStr(L"this is a test wstr.");
    serializer << sWStr;

    serializer << "this is a test str too.";
    serializer << L"this is a test wstr too.";
    serializer.Deserialize(_T("BeatsSerializerUnitTest.Cache"));
    CSerializer deserializer(_T("BeatsSerializerUnitTest.Cache"));
    CSerializer readDeserializer(deserializer);

    char charArrayOut[128];
    wchar_t wcharArrayOut[128];
    deserializer >> charArrayOut;
    BEATS_ASSERT(strcmp(charArrayOut, "1") == 0, _T("Failed to pass the unit test!"));
    char** ppChar = &pChar;
    wchar_t** ppWcharPtr = &pWChar;
    readDeserializer.Read(ppChar);

    BEATS_ASSERT(strcmp(*ppChar, "1") == 0, _T("Failed to pass the unit test!"));

    deserializer >> wcharArrayOut;
    BEATS_ASSERT(wcscmp(wcharArrayOut, L"2") == 0, _T("Failed to pass the unit test!"));
    readDeserializer.Read(ppWcharPtr);
    BEATS_ASSERT(wcscmp(*ppWcharPtr, L"2") == 0, _T("Failed to pass the unit test!"));

    deserializer >> charArrayOut;
    BEATS_ASSERT(strcmp(charArrayOut, "3") == 0, _T("Failed to pass the unit test!"));
    readDeserializer.Read(ppChar);
    BEATS_ASSERT(strcmp(*ppChar, "3") == 0, _T("Failed to pass the unit test!"));

    deserializer >> charArrayOut;
    BEATS_ASSERT(strcmp(charArrayOut, "4") == 0, _T("Failed to pass the unit test!"));
    readDeserializer.Read(ppChar);
    BEATS_ASSERT(strcmp(*ppChar, "4") == 0, _T("Failed to pass the unit test!"));

    deserializer >> wcharArrayOut;
    BEATS_ASSERT(wcscmp(wcharArrayOut, L"5") == 0, _T("Failed to pass the unit test!"));
    readDeserializer.Read(ppWcharPtr);
    BEATS_ASSERT(wcscmp(wcharArrayOut, L"5") == 0, _T("Failed to pass the unit test!"));

    deserializer >> wcharArrayOut;
    BEATS_ASSERT(wcscmp(wcharArrayOut, L"6") == 0, _T("Failed to pass the unit test!"));
    readDeserializer.Read(ppWcharPtr);
    BEATS_ASSERT(wcscmp(*ppWcharPtr, L"6") == 0, _T("Failed to pass the unit test!"));

    deserializer >> charArrayOut;
    BEATS_ASSERT(strcmp(charArrayOut, "777") == 0, _T("Failed to pass the unit test!"));
    readDeserializer.Read(ppChar);
    BEATS_ASSERT(strcmp(*ppChar, "777") == 0, _T("Failed to pass the unit test!"));

    deserializer >> charArrayOut;
    BEATS_ASSERT(strcmp(charArrayOut, "8888") == 0, _T("Failed to pass the unit test!"));
    readDeserializer.Read(ppChar);
    BEATS_ASSERT(strcmp(*ppChar, "8888") == 0, _T("Failed to pass the unit test!"));

    deserializer >> wcharArrayOut;
    BEATS_ASSERT(wcscmp(wcharArrayOut, L"999") == 0, _T("Failed to pass the unit test!"));
    deserializer >> wcharArrayOut;
    readDeserializer.Read(ppWcharPtr);
    BEATS_ASSERT(wcscmp(*ppWcharPtr, L"999") == 0, _T("Failed to pass the unit test!"));

    BEATS_ASSERT(wcscmp(wcharArrayOut, L"aaa") == 0, _T("Failed to pass the unit test!"));
    deserializer >> sStr;
    readDeserializer.Read(ppWcharPtr);
    BEATS_ASSERT(wcscmp(*ppWcharPtr, L"aaa") == 0, _T("Failed to pass the unit test!"));

    BEATS_ASSERT(sStr.compare("this is a test str.") == 0, _T("Failed to pass the unit test!"));
    deserializer >> sWStr;
    BEATS_ASSERT(sWStr.compare(L"this is a test wstr.") == 0, _T("Failed to pass the unit test!"));
    deserializer >> sStr;
    BEATS_ASSERT(sStr.compare("this is a test str too.") == 0, _T("Failed to pass the unit test!"));
    deserializer >> sWStr;
    BEATS_ASSERT(sWStr.compare(L"this is a test wstr too.") == 0, _T("Failed to pass the unit test!"));
    BEATS_ASSERT(deserializer.GetReadPos() == serializer.GetWritePos(), _T("Test String serializer failed!"));

    serializer.Reset();
    deserializer.Reset();
    //test basic variable type.
    serializer << 10;
    serializer << -15;
    serializer << 5.5f;
    serializer << -15.5f;
    serializer << true;
    serializer << false;
    serializer << 'k';
    size_t nUint = 0xfffffffe;
    serializer << nUint;
    float fFloat = 0.7890F;
    serializer << fFloat;
    int nInt = -1;
    serializer << nInt;
    bool result = true;
    serializer << result;
    char cChar = 'a';
    serializer << cChar;
    double dDouble = 0.9988775;
    serializer << dDouble;
    serializer.Deserialize(_T("BeatsSerializerUnitTest.Cache"));
    deserializer.Serialize(_T("BeatsSerializerUnitTest.Cache"));
    readDeserializer = deserializer;

    deserializer >> nUint;
    BEATS_ASSERT(nUint == 10, _T("Failed to pass the unit test!"));
    size_t* pUint;
    readDeserializer.Read(pUint);
    BEATS_ASSERT(*pUint == 10, _T("Failed to pass the unit test!"));

    deserializer >> nInt;
    BEATS_ASSERT(nInt == -15, _T("Failed to pass the unit test!"));
    int* pInt; 
    readDeserializer.Read(pInt);
    BEATS_ASSERT(*pInt == -15, _T("Failed to pass the unit test!"));

    deserializer >> fFloat;
    BEATS_ASSERT(BEATS_FLOAT_EQUAL(fFloat, 5.5), _T("Failed to pass the unit test!"));
    float* pFloat;
    readDeserializer.Read(pFloat);
    BEATS_ASSERT(BEATS_FLOAT_EQUAL(*pFloat, 5.5), _T("Failed to pass the unit test!"));

    deserializer >> fFloat;
    BEATS_ASSERT(BEATS_FLOAT_EQUAL(fFloat, -15.5f), _T("Failed to pass the unit test!"));
    readDeserializer.Read(pFloat);
    BEATS_ASSERT(BEATS_FLOAT_EQUAL(*pFloat, -15.5f), _T("Failed to pass the unit test!"));

    deserializer >> result;
    BEATS_ASSERT(result == true, _T("Failed to pass the unit test!"));
    bool* pBool;
    readDeserializer.Read(pBool);
    BEATS_ASSERT(*pBool == true, _T("Failed to pass the unit test!"));

    deserializer >> result;
    BEATS_ASSERT(result == false, _T("Failed to pass the unit test!"));
    readDeserializer.Read(pBool);
    BEATS_ASSERT(*pBool == false, _T("Failed to pass the unit test!"));

    deserializer >> cChar;
    BEATS_ASSERT(cChar == 'k', _T("Failed to pass the unit test!"));
    readDeserializer.Read(pChar);
    BEATS_ASSERT(*pChar == 'k', _T("Failed to pass the unit test!"));

    deserializer >> nUint;
    BEATS_ASSERT(nUint == 0xfffffffe, _T("Failed to pass the unit test!"));
    readDeserializer.Read(pUint);
    BEATS_ASSERT(*pUint == 0xfffffffe, _T("Failed to pass the unit test!"));

    deserializer >> fFloat;
    BEATS_ASSERT(BEATS_FLOAT_EQUAL(fFloat, 0.7890), _T("Failed to pass the unit test!"));
    readDeserializer.Read(pFloat);
    BEATS_ASSERT(BEATS_FLOAT_EQUAL(*pFloat, 0.7890), _T("Failed to pass the unit test!"));

    deserializer >> nInt;
    BEATS_ASSERT(nInt == -1, _T("Failed to pass the unit test!"));
    readDeserializer.Read(pInt);
    BEATS_ASSERT(*pInt == -1, _T("Failed to pass the unit test!"));


    deserializer >> result;
    BEATS_ASSERT(result == true, _T("Failed to pass the unit test!"));
    readDeserializer.Read(pBool);
    BEATS_ASSERT(*pBool == true, _T("Failed to pass the unit test!"));

    deserializer >> cChar;
    BEATS_ASSERT(cChar == 'a', _T("Failed to pass the unit test!"));
    readDeserializer.Read(pChar);
    BEATS_ASSERT(*pChar == 'a', _T("Failed to pass the unit test!"));

    deserializer >> dDouble;
    BEATS_ASSERT(BEATS_FLOAT_EQUAL(dDouble, 0.9988775), _T("Failed to pass the unit test!"));
    double* pDouble = NULL;
    readDeserializer.Read(pDouble);
    BEATS_ASSERT(BEATS_FLOAT_EQUAL(*pDouble, 0.9988775), _T("Failed to pass the unit test!"));

    BEATS_ASSERT(deserializer.GetReadPos() == serializer.GetWritePos(), _T("Test String serializer failed!"));
}

size_t CSerializer::ReadToDataInList( const std::vector<SBufferData>& dataList, bool bReverse/* = false*/, int* pMatchDataId /* =NULL */)
{
    if (pMatchDataId != NULL)
    {
        *pMatchDataId = -1;
    }
    bool bMatched = false;
    while (!bMatched && m_pReadPtr <= m_pWritePtr && m_pReadPtr >= m_pBuffer)
    {
        for (size_t i = 0; i < dataList.size(); ++i)
        {
            if (((size_t)m_pWritePtr - (size_t)m_pReadPtr >= dataList[i].dataLength) && memcmp(dataList[i].pData, m_pReadPtr, dataList[i].dataLength) == 0)
            {
                if (pMatchDataId != NULL)
                {
                    *pMatchDataId = (int)i;
                    bMatched = true;
                }
                break;
            }
        }
        if (!bMatched)
        {
            m_pReadPtr = static_cast<unsigned char*>(m_pReadPtr) + 1 * (bReverse ? -1 : 1);
        }
    }
    if (!bMatched)
    {
        //Reset ptr because if nothing found, it must be out of scope by the while condition.
        m_pReadPtr = bReverse ? m_pBuffer : m_pWritePtr;
    }

    return GetReadPos();
}

size_t CSerializer::ReadToData(const SBufferData& data, bool bReverse/* = false*/)
{
    bool bFindData = false;
    while (m_pReadPtr <= m_pWritePtr && m_pReadPtr >= m_pBuffer)
    {
        if (((size_t)m_pWritePtr - (size_t)m_pReadPtr >= data.dataLength) && memcmp(data.pData, m_pReadPtr, data.dataLength) == 0)
        {
            bFindData = true;
            break;
        }
        m_pReadPtr = static_cast<unsigned char*>(m_pReadPtr) + 1 * (bReverse ? -1 : 1);
    }
    if (!bFindData)
    {
        //Reset ptr because if nothing found, it must be out of scope by the while condition.
        m_pReadPtr = bReverse ? m_pBuffer : m_pWritePtr;
    }
    return GetReadPos();
}

void* CSerializer::GetUserData()
{
    return m_pUserData;
}

void CSerializer::SetUserData( void* pData )
{
    m_pUserData = pData;
}

void CSerializer::IncreaseBufferSize()
{
    ValidateBuffer(m_size * 2);
}

void CSerializer::SetBuffer( const unsigned char* buffer, size_t uBufferLength )
{
    BEATS_SAFE_DELETE(m_pBuffer);
    m_pBuffer = (void*)buffer; 
    m_size = uBufferLength;
    Reset();
};
