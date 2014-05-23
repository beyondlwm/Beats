#include "stdafx.h"
#include "BytesOrderEncoder.h"
#include <algorithm>
#include <vector>
#include "UtilityManager.h"
#include "MD5/md5.h"

static const size_t Read_Buffer_Size = 1024 * 1024 * 10; // 10M

CBytesOrderEncoder::CBytesOrderEncoder()
{
}

CBytesOrderEncoder::~CBytesOrderEncoder()
{

}

bool CBytesOrderEncoder::EncodeImpl(CSerializer* pSourceSerializer, CSerializer* pEncodeSerializer)
{
    bool bRet = false;
    BEATS_ASSERT(m_header.m_uSectionCount > 0, _T("Section count can't be zero for CBytesOrderEncoder"));
    if (pSourceSerializer && pEncodeSerializer)
    {
        std::vector<size_t> orders;
        GetShuffleOrder(orders);
        size_t BytesCountForEachSection = (size_t)m_header.m_uOriDataSize / m_header.m_uSectionCount;
        size_t uTotalReadCounter = 0;
        for (size_t i = 0; i < orders.size(); ++i)
        {
            size_t offset = orders[i] * BytesCountForEachSection;
            pSourceSerializer->SetReadPos(offset);
            size_t uSectionSize = min((size_t)m_header.m_uOriDataSize - offset, BytesCountForEachSection);
            size_t uCounter = 0;
            while (uCounter < uSectionSize)
            {
                size_t uCopySizeEachTime = min(Read_Buffer_Size, uSectionSize - uCounter);
                pEncodeSerializer->Serialize(*pSourceSerializer, sizeof(unsigned char) * uCopySizeEachTime);
                uCounter += uCopySizeEachTime;
            }
            uTotalReadCounter += uSectionSize;
        }
        bRet = uTotalReadCounter == m_header.m_uOriDataSize;
    }
    return bRet;
}

bool CBytesOrderEncoder::EncodeImpl(FILE* pSourceFile, FILE* pEncodeFile)
{
    bool bRet = false;
    std::vector<size_t> orders;
    GetShuffleOrder(orders);

    long long BytesCountForEachSection = m_header.m_uOriDataSize / m_header.m_uSectionCount;
    long long uSourceFileStartPos;
    unsigned char* pBuffer = new unsigned char[Read_Buffer_Size];
    for (size_t i = 0; i < orders.size(); ++i)
    {
        uSourceFileStartPos = orders[i] * BytesCountForEachSection;
        _fseeki64(pSourceFile, uSourceFileStartPos, FILE_BEGIN);
        long long uSectionSize = min(m_header.m_uOriDataSize - uSourceFileStartPos, BytesCountForEachSection);
        long long uCounter = 0;
        while (uCounter < uSectionSize)
        {
            size_t uCopySizeEachTime = min(Read_Buffer_Size, (size_t)(uSectionSize - uCounter));
            bRet = CUtilityManager::GetInstance()->ReadDataFromFile(pSourceFile, pBuffer, uCopySizeEachTime);
            bRet = bRet && CUtilityManager::GetInstance()->WriteDataToFile(pEncodeFile, pBuffer, uCopySizeEachTime);
            BEATS_ASSERT(bRet, _T("Copy Data Failed!"));
            uCounter += uCopySizeEachTime;
        }
    }
    BEATS_SAFE_DELETE_ARRAY(pBuffer);
    return bRet;
}

void CBytesOrderEncoder::SetSectionCount(size_t sectionCount)
{
    m_header.m_uSectionCount = sectionCount;
}

void CBytesOrderEncoder::SetRandomSeed(size_t randomSeed)
{
    m_header.m_uRandomSeed = randomSeed;
}

bool CBytesOrderEncoder::DecodeImpl(CSerializer* pEncodeSerializer, size_t uStartPos, CSerializer* pDecodeSerializer)
{
    bool bRet = false;
    pEncodeSerializer->SetReadPos(uStartPos + m_header.m_uHeaderSize);
    pDecodeSerializer->ValidateBuffer((size_t)m_header.m_uOriDataSize);
    std::vector<size_t> orders;
    GetShuffleOrder(orders);
    size_t BytesCountForEachSection = (size_t)m_header.m_uOriDataSize / m_header.m_uSectionCount;
    size_t uTotalWriteCount = 0;
    for (size_t i = 0; i < orders.size(); ++i)
    {
        size_t offset = orders[i] * BytesCountForEachSection;
        pDecodeSerializer->SetWritePos(offset);

        size_t uCounter = 0;
        size_t uSectionSize = min((size_t)m_header.m_uOriDataSize - offset, BytesCountForEachSection);
        while (uCounter < uSectionSize)
        {
            size_t uReadCountForEach = min(uSectionSize - uCounter, Read_Buffer_Size);
            pEncodeSerializer->Deserialize(*pDecodeSerializer, uReadCountForEach);
            uCounter += uReadCountForEach;
        }
        uTotalWriteCount += uCounter;
    }
    bRet = uTotalWriteCount == m_header.m_uOriDataSize;
    pDecodeSerializer->SetWritePos(uTotalWriteCount);
    return bRet;
}

bool CBytesOrderEncoder::DecodeImpl(FILE* pEncodeFile, long long uStartPos, FILE* pDecodeFile)
{
    bool bRet = true;
    BEATS_ASSERT(m_header.m_uEncodeDataSize - m_header.m_uHeaderSize == m_header.m_uOriDataSize, 
                _T("Data size not match for CBytesOrderEncoder, Encode File Size %lld, Origin File Size %lld"),
                m_header.m_uEncodeDataSize, m_header.m_uOriDataSize);
    long long uEncodeOffset;
    uEncodeOffset = uStartPos + m_header.m_uHeaderSize;
    _fseeki64(pEncodeFile, uEncodeOffset, FILE_BEGIN);

    std::vector<size_t> orders;
    GetShuffleOrder(orders);
    long long BytesCountForEachSection = m_header.m_uOriDataSize / m_header.m_uSectionCount;
    unsigned char* pBuffer = new unsigned char[Read_Buffer_Size];
    long long uTotalWriteCount = 0;
    for (size_t i = 0; i < orders.size() && bRet; ++i)
    {
        long long uDecodeOffset;
        uDecodeOffset = orders[i] * BytesCountForEachSection;
        _fseeki64(pDecodeFile, uDecodeOffset, FILE_BEGIN);
        long long uSectionSize = min(m_header.m_uOriDataSize - uDecodeOffset, BytesCountForEachSection);
        BEATS_ASSERT(uSectionSize > 0, _T("Section size can't be less than 1"));
        size_t uCounter = 0;
        while (uCounter < uSectionSize && bRet)
        {
            size_t uCopySizeEachTime = min(Read_Buffer_Size, (size_t)(uSectionSize - uCounter));
            bRet = CUtilityManager::GetInstance()->ReadDataFromFile(pEncodeFile, pBuffer, uCopySizeEachTime);
            bRet = bRet && CUtilityManager::GetInstance()->WriteDataToFile(pDecodeFile, pBuffer, uCopySizeEachTime);
            BEATS_ASSERT(bRet, _T("Copy Data Failed!"));
            uCounter += uCopySizeEachTime;
        }
        BEATS_ASSERT(uCounter == uSectionSize);
        uTotalWriteCount += uSectionSize;
    }
    bRet = bRet && uTotalWriteCount == m_header.m_uOriDataSize;
    BEATS_SAFE_DELETE_ARRAY(pBuffer);
    return bRet;
}

SEncodeHeader* CBytesOrderEncoder::GetEncodeHeader()
{
    return &m_header;
}

void CBytesOrderEncoder::GetShuffleOrder( std::vector<size_t>& shuffleOrder )
{
    shuffleOrder.clear();
    BEATS_ASSERT(m_header.m_uOriDataSize >= m_header.m_uSectionCount, _T("File size %d can't be less than section count:%d!"), m_header.m_uOriDataSize, m_header.m_uSectionCount);
    size_t realSectionCount = m_header.m_uSectionCount + ((m_header.m_uOriDataSize % m_header.m_uSectionCount) > 0 ? 1 : 0);
    for (size_t i = 0; i < realSectionCount; ++i)
    {
        shuffleOrder.push_back(i);
    }
    srand((unsigned int)m_header.m_uRandomSeed);
    random_shuffle(shuffleOrder.begin(), shuffleOrder.end());
}