#ifndef BEATS_UTILITY_FILEENCODER_BYTESORDERENCODER_H__INCLUDE
#define BEATS_UTILITY_FILEENCODER_BYTESORDERENCODER_H__INCLUDE

#include "FileEncoderBase.h"

struct SBytesOrderEncodeHeader : public SEncodeHeader
{
    SBytesOrderEncodeHeader()
        : SEncodeHeader(sizeof(*this), eFET_BytesOrder)
        , m_uSectionCount(0)
        , m_uRandomSeed(0)
    {
    }
    ~SBytesOrderEncodeHeader()
    {

    }
    size_t m_uSectionCount;
    size_t m_uRandomSeed;
};
class CBytesOrderEncoder : public CFileEncoderBase
{
    typedef CFileEncoderBase super;
public:
    CBytesOrderEncoder();
    virtual ~CBytesOrderEncoder();

public:

    void SetSectionCount(size_t sectionCount);
    void SetRandomSeed(size_t randomSeed);

    virtual EFileEncodeType GetType() {return eFET_BytesOrder;}

private:
    virtual bool EncodeImpl(CSerializer* pSourceFile, CSerializer* pEncodeFile);
    virtual bool EncodeImpl(HANDLE hSourceFile, HANDLE hEncodeFile);
    virtual bool DecodeImpl(CSerializer* pEncodeFile, size_t uStartPos, CSerializer* pDecodeFile);
    virtual bool DecodeImpl(HANDLE hEncodeFile, long long uStartPos, HANDLE hDecodeFile);

    virtual SEncodeHeader* GetEncodeHeader();

    void GetShuffleOrder(std::vector<size_t>& shuffleOrder);


private:
    SBytesOrderEncodeHeader m_header;
};

#endif