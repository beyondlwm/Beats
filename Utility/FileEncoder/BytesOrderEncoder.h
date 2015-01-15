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
    uint32_t m_uSectionCount;
    uint32_t m_uRandomSeed;
};
class CBytesOrderEncoder : public CFileEncoderBase
{
    typedef CFileEncoderBase super;
public:
    CBytesOrderEncoder();
    virtual ~CBytesOrderEncoder();

public:

    void SetSectionCount(uint32_t sectionCount);
    void SetRandomSeed(uint32_t randomSeed);

    virtual EFileEncodeType GetType() {return eFET_BytesOrder;}

private:
    virtual bool EncodeImpl(CSerializer* pSourceFile, CSerializer* pEncodeFile);
    virtual bool EncodeImpl(FILE* hSourceFile, FILE* hEncodeFile);
    virtual bool DecodeImpl(CSerializer* pEncodeFile, uint32_t uStartPos, CSerializer* pDecodeFile);
    virtual bool DecodeImpl(FILE* hEncodeFile, long long uStartPos, FILE* hDecodeFile);

    virtual SEncodeHeader* GetEncodeHeader();

    void GetShuffleOrder(std::vector<uint32_t>& shuffleOrder);


private:
    SBytesOrderEncodeHeader m_header;
};

#endif