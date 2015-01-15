#include "stdafx.h"
#if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
#include "BytesFlipEncoder.h"
#include "UtilityManager.h"

static const uint32_t Read_Buffer_Size = 1024 * 1024 * 10; // 10M

CBytesFlipEncoder::CBytesFlipEncoder()
{
}

CBytesFlipEncoder::~CBytesFlipEncoder()
{

}

void CBytesFlipEncoder::SetStepRange(unsigned short iStepMin, unsigned short iStepMax)
{
    BEATS_ASSERT(iStepMin >= 0 && iStepMax >= iStepMin);
    m_header.m_iStepMin = iStepMin;
    m_header.m_iStepMax = iStepMax;
}

void CBytesFlipEncoder::SetRandomSeed(uint32_t uRadomSeed)
{
    m_header.m_uRandomSeed = uRadomSeed;
}

void CBytesFlipEncoder::SetFlipCountRange(unsigned short iFlipCountMin, unsigned short iFlipCountMax)
{
    BEATS_ASSERT(iFlipCountMin > 0 && iFlipCountMax >= iFlipCountMin);

    m_header.m_iFlipCountMin = iFlipCountMin;
    m_header.m_iFlipCountMax = iFlipCountMax;
}

bool CBytesFlipEncoder::EncodeImpl(CSerializer* pSourceSerializer, CSerializer* pEncodeSerializer)
{
    bool bRet = false;
    if (pSourceSerializer != NULL && pEncodeSerializer != NULL && pSourceSerializer->GetWritePos() > 0)
    {
        pEncodeSerializer->ValidateBuffer(pSourceSerializer->GetBufferSize());
        if (CheckParameter())
        {
            srand((unsigned int)m_header.m_uRandomSeed);
            unsigned short uFlipDeltaValue = m_header.m_iFlipCountMax - m_header.m_iFlipCountMin;
            unsigned short uStepDeltaValue = m_header.m_iStepMax - m_header.m_iStepMin;
            uint32_t uTotalReadCounter = 0;
            while (pSourceSerializer->GetReadPos() < pSourceSerializer->GetWritePos())
            {
                unsigned short uFlipCount = (unsigned short)(((double)rand() / RAND_MAX) * uFlipDeltaValue + m_header.m_iFlipCountMin);
                unsigned short uFlipSizeEachTime = std::min<unsigned short>(uFlipCount, pSourceSerializer->GetWritePos() - pSourceSerializer->GetReadPos());
                unsigned char* pSourceData = (unsigned char*)pSourceSerializer->GetReadPtr();
                unsigned char* pEncodeData = (unsigned char*)pEncodeSerializer->GetWritePtr();
                for (uint32_t i = 0; i < uFlipSizeEachTime; ++i)
                {
                    pEncodeData[i] = ~pSourceData[i];
                }
                pSourceSerializer->SetReadPos(pSourceSerializer->GetReadPos() + uFlipSizeEachTime);
                pEncodeSerializer->SetWritePos(pEncodeSerializer->GetWritePos() + uFlipSizeEachTime);
                uTotalReadCounter += uFlipSizeEachTime;
                // Do step logic.
                unsigned short uStepCount = (unsigned short)(((double)rand() / RAND_MAX) * uStepDeltaValue) + m_header.m_iStepMin;
                uint32_t uCurrendReadPos = pSourceSerializer->GetReadPos();
                uint32_t uNewPos = uCurrendReadPos + uStepCount;
                if (uNewPos >= pSourceSerializer->GetWritePos())
                {
                    uStepCount = (unsigned short)(pSourceSerializer->GetWritePos() - uCurrendReadPos);
                }
                pEncodeSerializer->Serialize(*pSourceSerializer, uStepCount);
                uTotalReadCounter += uStepCount;
            }
            bRet = uTotalReadCounter == m_header.m_uOriDataSize;
        }
    }
    return bRet;
}

bool CBytesFlipEncoder::EncodeImpl(FILE* pSourceFile, FILE* pEncodeFile)
{
    bool bRet = CheckParameter();
    if (bRet)
    {
        srand((unsigned int)m_header.m_uRandomSeed);
        unsigned short uFlipDeltaValue = m_header.m_iFlipCountMax - m_header.m_iFlipCountMin;
        unsigned short uStepDeltaValue = m_header.m_iStepMax - m_header.m_iStepMin;
        long long uTotalReadCounter = 0;
        unsigned char* pBuffer = new unsigned char[Read_Buffer_Size];
        while (uTotalReadCounter < m_header.m_uOriDataSize && bRet)
        {
            unsigned short uFlipCount = (unsigned short)(((double)rand() / RAND_MAX) * uFlipDeltaValue + m_header.m_iFlipCountMin);
            unsigned short uFlipSizeEachTime = std::min<unsigned short>(uFlipCount, (unsigned short)(m_header.m_uOriDataSize - uTotalReadCounter));
            if (CUtilityManager::GetInstance()->ReadDataFromFile(pSourceFile, pBuffer, uFlipSizeEachTime))
            {
                for (unsigned short i = 0; i < uFlipSizeEachTime; ++i)
                {
                    pBuffer[i] = ~pBuffer[i];
                }
                if (CUtilityManager::GetInstance()->WriteDataToFile(pEncodeFile, pBuffer, uFlipSizeEachTime))
                {
                    uTotalReadCounter += uFlipSizeEachTime;
                    // Do step logic.
                    unsigned short uStepCount = (unsigned short)(((double)rand() / RAND_MAX) * uStepDeltaValue) + m_header.m_iStepMin;
                    long long uNewPos = uTotalReadCounter + uStepCount;
                    if (uNewPos >= m_header.m_uOriDataSize)
                    {
                        uStepCount = (unsigned short)(m_header.m_uOriDataSize - uTotalReadCounter);
                    }
                    if (uStepCount > 0)
                    {
                        CUtilityManager::GetInstance()->ReadDataFromFile(pSourceFile, pBuffer, uStepCount);
                        CUtilityManager::GetInstance()->WriteDataToFile(pEncodeFile, pBuffer, uStepCount);

                        uTotalReadCounter += uStepCount;
                    }
                }
            }
        }
        bRet = uTotalReadCounter == m_header.m_uOriDataSize;
        BEATS_SAFE_DELETE_ARRAY(pBuffer);
    }

    return bRet;
}

bool CBytesFlipEncoder::DecodeImpl(CSerializer* pEncodeSerializer, uint32_t uStartPos, CSerializer* pDecodeSerializer)
{
    // Just Flip the encode data without header part.
    CSerializer tmpSerializer;
    tmpSerializer.Serialize((void*)(pEncodeSerializer->GetBuffer() + m_header.m_uHeaderSize + uStartPos), (uint32_t)m_header.m_uOriDataSize);
    bool bRet = EncodeImpl(&tmpSerializer, pDecodeSerializer);
    return bRet;
}

bool CBytesFlipEncoder::DecodeImpl(FILE* pEncodeFile, long long uStartPos, FILE* pDecodeFile)
{
    long long offset;
    offset = uStartPos + m_header.m_uHeaderSize;
    _fseeki64(pEncodeFile, offset, FILE_BEGIN);
    bool bRet = EncodeImpl(pEncodeFile, pDecodeFile);
    return bRet;
}


SEncodeHeader* CBytesFlipEncoder::GetEncodeHeader()
{
    return &m_header;
}

bool CBytesFlipEncoder::CheckParameter()
{
    bool bCheckFlipCount = m_header.m_iFlipCountMin <= m_header.m_iFlipCountMax && m_header.m_iFlipCountMin > 0;
    bool bCheckStep = m_header.m_iStepMin <= m_header.m_iStepMax && m_header.m_iStepMin >= 0;
    BEATS_ASSERT(bCheckStep, _T("Step Parameter invalid for CBytesFlipEncoder. StepMin %d StepMax %d"), m_header.m_iStepMin, m_header.m_iStepMax);
    BEATS_ASSERT(bCheckFlipCount, _T("FlipCount Parameter invalid for CBytesFlipEncoder.FlipCountMin %d FlipCountMax %d"), m_header.m_iFlipCountMin, m_header.m_iFlipCountMax);
    return bCheckStep && bCheckFlipCount;
}
#endif