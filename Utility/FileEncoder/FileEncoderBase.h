#ifndef BEATS_UTILITY_FILEENCODER_FILEENCODERBASE_H__INCLUDE
#define BEATS_UTILITY_FILEENCODER_FILEENCODERBASE_H__INCLUDE

#include "../Serializer/Serializer.h"

enum EFileEncodeType
{
    eFET_Invalid,
    eFET_BytesOrder,
    eFET_BytesFlip,

    eFET_Count,
    eFET_Force32Bite = 0xffffffff
};

struct SEncodeHeader
{
    SEncodeHeader(size_t uHeaderSize, EFileEncodeType type)
        : m_type(type)
        , m_uHeaderSize(uHeaderSize)
        , m_uOriDataSize(0)
        , m_uEncodeDataSize(0)
    {
    }
    ~SEncodeHeader()
    {

    }
    EFileEncodeType m_type;
    size_t m_uHeaderSize;
    long long m_uOriDataSize;
    long long m_uEncodeDataSize;
};

class CFileEncoderBase
{
public:
    CFileEncoderBase();
    virtual ~CFileEncoderBase();

    virtual EFileEncodeType GetType() = 0;

    virtual bool Encode(CSerializer* pSourceSerializer, CSerializer* pEncodeSerializer);
    virtual bool Encode(const TCHAR* pszSourceFilePath, const TCHAR* pszEncodeFilePath = NULL);

    virtual bool Decode(CSerializer* pSourceSerializer, size_t uStartPos, CSerializer* pDecodeSerializer);
    virtual bool Decode(const TCHAR* pszEncodeFilePath, long long uStartPos = 0, const TCHAR* pszDecodeFilePath = NULL);

    virtual bool UnitTest();


private:

    bool ReadEncodeHeader(CSerializer* pEncodeSerializer, size_t uStartPos);
    bool ReadEncodeHeader(HANDLE hEncodeFile, long long uStartPos);

    virtual bool EncodeImpl(CSerializer* pSourceFile, CSerializer* pEncodeFile) = 0;
    virtual bool EncodeImpl(HANDLE hSourceFile, HANDLE hEncodeFile) = 0;

    virtual bool DecodeImpl(CSerializer* pEncodeFile, size_t uStartPos, CSerializer* pDecodeFile) = 0;
    virtual bool DecodeImpl(HANDLE hEncodeFile, long long uStartPos, HANDLE hDecodeFile) = 0;

    virtual SEncodeHeader* GetEncodeHeader() = 0;
};

#endif