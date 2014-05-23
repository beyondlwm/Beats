#ifndef BEATS_UTILITY_FILEENCODER_BYTESFLIPENCODER_H__INCLUDE
#define BEATS_UTILITY_FILEENCODER_BYTESFLIPENCODER_H__INCLUDE

#include "FileEncoderBase.h"

struct SBytesFlipEncodeHeader : public SEncodeHeader
{
    SBytesFlipEncodeHeader()
        : SEncodeHeader(sizeof(*this), eFET_BytesFlip)
        , m_iStepMin(0)
        , m_iStepMax(0)
        , m_uRandomSeed(0)
        , m_iFlipCountMin(0)
        , m_iFlipCountMax(0)
    {
    }
    ~SBytesFlipEncodeHeader()
    {

    }
    unsigned short m_iStepMin;
    unsigned short m_iStepMax;
    unsigned short m_iFlipCountMin;
    unsigned short m_iFlipCountMax;
    size_t m_uRandomSeed;
};

/*
This encoder used to flip some unsigned char of the data with random position.
we flip random count of data BYTES with SetFlipCountRange, and we will ignore next random count of data BYTES with SetStepRange.
it means if you want to flip all data BYTES, you should SetStepRange(0, 0).
*/
class CBytesFlipEncoder : public CFileEncoderBase
{
    typedef CFileEncoderBase super;
public:
    CBytesFlipEncoder();
    virtual ~CBytesFlipEncoder();

    void SetStepRange(unsigned short iStepMin, unsigned short iStepMax);
    void SetRandomSeed(size_t uRadomSeed);
    void SetFlipCountRange(unsigned short iFlipCountMin, unsigned short iFlipCountMax);

    virtual EFileEncodeType GetType() {return eFET_BytesFlip;}

private:
    bool CheckParameter();

    virtual bool EncodeImpl(CSerializer* pSourceFile, CSerializer* pEncodeFile) override;
    virtual bool EncodeImpl(FILE* hSourceFile, FILE* hEncodeFile) override;
    virtual bool DecodeImpl(CSerializer* pEncodeFile, size_t uStartPos, CSerializer* pDecodeFile) override;
    virtual bool DecodeImpl(FILE* hEncodeFile, long long uStartPos, FILE* hDecodeFile) override;

    virtual SEncodeHeader* GetEncodeHeader();

private:
    SBytesFlipEncodeHeader m_header;
};

#endif