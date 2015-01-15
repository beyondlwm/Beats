#include "stdafx.h"
#if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
#include "FileEncoderBase.h"
#include "UtilityManager.h"
#include "MD5/md5.h"
#include <sys/stat.h>

static const uint32_t Read_Buffer_Size = 1024 * 1024 * 10; // 10M

CFileEncoderBase::CFileEncoderBase()
{
}

CFileEncoderBase::~CFileEncoderBase()
{

}

bool CFileEncoderBase::Encode(const TCHAR* pszSourceFilePath, const TCHAR* pszEncodeFilePath /*= NULL*/)
{
    bool bRet = false;
    BEATS_ASSERT(pszSourceFilePath != NULL, _T("Source file path can't be NULL"));
    FILE* pSourceFile = _tfopen(pszSourceFilePath, _T("rb+"));

    BEATS_ASSERT(NULL != pSourceFile, _T("Read File %s Failed in CFileEncoderBase::Encode"), pszSourceFilePath);
    if (NULL != pSourceFile)
    {
        TCHAR newFileName[MAX_PATH];
        if (pszEncodeFilePath == NULL)
        {
            _stprintf(newFileName, _T("%s_encode"), pszSourceFilePath);
            pszEncodeFilePath = newFileName;
        }
        FILE* pEncodeFile = _tfopen(pszEncodeFilePath, _T("wb+"));
        BEATS_ASSERT(NULL != pEncodeFile, _T("Create File %s Failed in CFileEncoderBase::Encode"), pszEncodeFilePath);
        if (NULL != pEncodeFile)
        {
            SEncodeHeader* pHeader = GetEncodeHeader();
            BEATS_ASSERT(pHeader->m_type == this->GetType(), _T("Header type doesn't match, header type %d encoder type %d"), pHeader->m_type, this->GetType());

            struct _stati64 sourceFs;
            int nSourceFsRet = _fstat64(_fileno(pSourceFile), &sourceFs );
            BEATS_ASSERT(nSourceFsRet == 0);
            if (nSourceFsRet == 0)
            {
                pHeader->m_uOriDataSize = sourceFs.st_size;
                // 1. Write header info at the beginning first for take the place, these data will be rewrite after EncodeFileImpl.
                CUtilityManager::GetInstance()->WriteDataToFile(pEncodeFile, pHeader, pHeader->m_uHeaderSize);

                EncodeImpl(pSourceFile, pEncodeFile);
                fflush(pEncodeFile);
                struct _stati64 encodeFs;
                int nEncodeRet = _fstat64(_fileno(pEncodeFile), &encodeFs );
                BEATS_ASSERT(nEncodeRet == 0);
                if (nEncodeRet == 0)
                {
                    // 2. Rewrite header
                    pHeader->m_uEncodeDataSize = encodeFs.st_size;
                    fseek(pEncodeFile, 0, FILE_BEGIN);
                    bRet = CUtilityManager::GetInstance()->WriteDataToFile(pEncodeFile, pHeader, pHeader->m_uHeaderSize);
                    BEATS_ASSERT(bRet, _T("write encode file failed!"));
                }
            }
            fclose(pEncodeFile);
        }
        fclose(pSourceFile);
    }
    return bRet;
}

bool CFileEncoderBase::Encode(CSerializer* pSourceSerializer, CSerializer* pEncodeSerializer)
{
    bool bRet = false;
    BEATS_ASSERT(pSourceSerializer != NULL, _T("Source serializer can't be null!"));
    BEATS_ASSERT(pEncodeSerializer != NULL, _T("encode serializer can't be null!"));
    BEATS_ASSERT(pEncodeSerializer != pSourceSerializer, _T("encode serializer and source serializer can't be the same!"));
    
    if (pSourceSerializer != NULL && 
        pEncodeSerializer != NULL &&
        pEncodeSerializer != pSourceSerializer &&
        pSourceSerializer->GetWritePos() > 0)
    {
        SEncodeHeader* pHeader = GetEncodeHeader();
        BEATS_ASSERT(pHeader != NULL, _T("Header info is not filled in EncodeFileImpl"));

        pHeader->m_uOriDataSize = pSourceSerializer->GetWritePos();

        // Write header info at the beginning first. Write dirty value for take the place, these data will be rewrite after EncodeFileImpl.
        pEncodeSerializer->Serialize(pHeader, pHeader->m_uHeaderSize);

        EncodeImpl(pSourceSerializer, pEncodeSerializer);

        pHeader->m_uEncodeDataSize = pEncodeSerializer->GetWritePos();
        // Rewrite the header after every data is confirmed.
        pEncodeSerializer->RewriteData(0, pHeader, pHeader->m_uHeaderSize);
        bRet = true;
    }
    return bRet;
}

bool CFileEncoderBase::Decode(const TCHAR* pszSourceFilePath, long long uStartPos/* = 0*/, const TCHAR* pszDecodeFilePath /*= NULL*/)
{
    bool bRet = false;
    BEATS_ASSERT(pszSourceFilePath != NULL && pszSourceFilePath[0] != 0, _T("Source file path is invalid"));
    FILE* pEncodeFile = _tfopen(pszSourceFilePath, _T("rb+"));

    BEATS_ASSERT(NULL != pEncodeFile, _T("Read File %s Failed in CFileEncoderBase::Encode"), pszSourceFilePath);
    if (NULL != pEncodeFile)
    {
        TCHAR newFileName[MAX_PATH];
        if (pszDecodeFilePath == NULL)
        {
            _stprintf(newFileName, _T("%s_decode"), pszSourceFilePath);
            pszDecodeFilePath = newFileName;
        }
        FILE* pDecodeFile = _tfopen(pszDecodeFilePath, _T("wb+"));
        BEATS_ASSERT(NULL != pDecodeFile, _T("Create File %s Failed in CFileEncoderBase::Encode"), pszDecodeFilePath);
        if (pDecodeFile != NULL)
        {
            bool bValidateFile = ReadEncodeHeader(pEncodeFile, uStartPos);
            if (bValidateFile)
            {
                bRet = DecodeImpl(pEncodeFile, uStartPos, pDecodeFile);
            }
            fclose(pDecodeFile);
        }
        fclose(pEncodeFile);
    }
    return bRet;
}

bool CFileEncoderBase::Decode(CSerializer* pSourceSerializer, uint32_t uStartPos, CSerializer* pDecodeSerializer)
{
    bool bRet = false;
    bool bSeralizerValid = pSourceSerializer != NULL && pDecodeSerializer != NULL && pSourceSerializer != pDecodeSerializer;
    BEATS_ASSERT(bSeralizerValid, _T("pSourceSerializer or pDecodeSerializer is NULL or the same"));
    bool bStartPosValid = pSourceSerializer->GetWritePos() > uStartPos;
    BEATS_ASSERT(bStartPosValid, _T("pSourceSerializer or pDecodeSerializer is NULL"));

    if (bSeralizerValid && bStartPosValid)
    {
        bool bValidateFile = ReadEncodeHeader(pSourceSerializer, uStartPos);
        if (bValidateFile)
        {
            bRet = DecodeImpl(pSourceSerializer, uStartPos, pDecodeSerializer);
        }
    }
    return bRet;
}

bool CFileEncoderBase::ReadEncodeHeader(CSerializer* pEncodeSerializer, uint32_t uStartPos)
{
    bool bRet = false;
    BEATS_ASSERT(pEncodeSerializer != NULL);
    // 1. Read basic info of header
    pEncodeSerializer->SetReadPos(uStartPos);
    SEncodeHeader* pHeader = GetEncodeHeader();
    uint32_t uReadCount = pEncodeSerializer->Deserialize(pHeader, sizeof(SEncodeHeader));
    bool bReadHeaderSuccess = uReadCount == sizeof(SEncodeHeader);
    BEATS_ASSERT(bReadHeaderSuccess, _T("Read encode file header failed!"));
    if (bReadHeaderSuccess)
    {
        bool bExamType = pHeader->m_type == GetType();
        BEATS_ASSERT(bExamType, _T("Analyse failed, this file is encoded by type %d, current is %d"), pHeader->m_type, GetType());
        if (bExamType)
        {
            uint32_t uEncodeFileSize = pEncodeSerializer->GetWritePos();
            bool bExamEncodeDataSize = (uEncodeFileSize - uStartPos) >= pHeader->m_uEncodeDataSize;
            BEATS_ASSERT(bExamEncodeDataSize, _T("Analyse failed, this file's size is too short!"));
            if (bExamEncodeDataSize)
            {
                // 2. Reload the header according to the basic info.
                pEncodeSerializer->SetReadPos(uStartPos);
                uint32_t uRealHeaderSize = pHeader->m_uHeaderSize;
                bRet = pEncodeSerializer->Deserialize(pHeader, uRealHeaderSize) == uRealHeaderSize;
            }
        }
    }
    return bRet;
}

bool CFileEncoderBase::ReadEncodeHeader(FILE* pEncodeFile, long long uStartPos)
{
    bool bRet = false;

    // 1. Read basic info of header
    if (NULL != pEncodeFile)
    {
        struct _stati64 encodeFs;
        int nEncodeFsRet = _fstat64(_fileno(pEncodeFile), &encodeFs );

        if (nEncodeFsRet == 0)
        {
            BEATS_ASSERT(uStartPos < encodeFs.st_size, _T("Invalid start pos in ReadEncodeHeader! start pos %lld, file size: %lld"), uStartPos, encodeFs.st_size);
            long long startDistance;
            startDistance = uStartPos;
            _fseeki64(pEncodeFile, startDistance, FILE_BEGIN);
            SEncodeHeader* pHeader = GetEncodeHeader();
            bRet = CUtilityManager::GetInstance()->ReadDataFromFile(pEncodeFile, pHeader, sizeof(SEncodeHeader));
            BEATS_ASSERT(bRet, _T("Read encode file header failed!"));
            if (bRet)
            {
                bool bExamType = pHeader->m_type == GetType();
                BEATS_ASSERT(bExamType, _T("Analyse failed, this file is encoded by type %d, current is %d"), pHeader->m_type, GetType());
                if (bExamType)
                {
                    long long uEncodeFileSize = encodeFs.st_size;
                    bool bExamEncodeDataSize = (uEncodeFileSize - uStartPos) >= pHeader->m_uEncodeDataSize;
                    BEATS_ASSERT(bExamEncodeDataSize, _T("Analyse failed, this file's size is too short!"));
                    if (bExamEncodeDataSize)
                    {
                        // 2. Reload the header according to the basic info.
                        _fseeki64(pEncodeFile, startDistance, FILE_BEGIN);
                        bRet = CUtilityManager::GetInstance()->ReadDataFromFile(pEncodeFile, pHeader, pHeader->m_uHeaderSize);
                    }
                }
            }
        }
    }
    
    return bRet;
}

bool CFileEncoderBase::UnitTest()
{
    bool bRet = false;
    CMD5 sourceMd5;

    // 1. Create source file.
    static const TCHAR* pszSourceFileName = _T("BytesOrderEncoderUnitTestFile.del");
    static const TCHAR* pszEncodeFileName = _T("BytesOrderEncoderUnitTestFileEncode.del");
    static const TCHAR* pszDecodeFileName = _T("BytesOrderEncoderUnitTestFileDecode.del");

    FILE* pSourceFile = _tfopen(pszSourceFileName, _T("wb+"));
    if (pSourceFile != NULL)
    {
        srand(GetTickCount());
        uint32_t* pBuffer = new uint32_t[Read_Buffer_Size];
        for (uint32_t i = 0; i < Read_Buffer_Size; i ++)
        {
            pBuffer[i] = rand();
        }
        sourceMd5.Update(pBuffer, sizeof(uint32_t) * Read_Buffer_Size);
        bRet = CUtilityManager::GetInstance()->WriteDataToFile(pSourceFile, pBuffer, Read_Buffer_Size * sizeof(uint32_t));
        BEATS_ASSERT(bRet, _T("Write data to file %s failed! Create source file failed in CBytesOrderEncoder::UnitTest"), pszSourceFileName);
        BEATS_SAFE_DELETE_ARRAY(pBuffer);
        fclose(pSourceFile);
    }

    //2. Encode it with serializer.
    CSerializer sourceSerializer(pszSourceFileName);
    CSerializer encodeSerializer, decodeSerializer;
    bRet = bRet && Encode(&sourceSerializer, &encodeSerializer);
    BEATS_ASSERT(bRet, _T("Encode data with serializer failed in CBytesOrderEncoder::UnitTest"));
    bRet = bRet && Decode(&encodeSerializer, 0, &decodeSerializer);
    BEATS_ASSERT(bRet, _T("Decode data with serializer failed in CBytesOrderEncoder::UnitTest"));

    CMD5 decodeMd5WithSerializer(decodeSerializer.GetBuffer(), decodeSerializer.GetWritePos());
    bRet = bRet && decodeMd5WithSerializer == sourceMd5;
    BEATS_ASSERT(bRet, _T("Decode data with serializer failed in CBytesOrderEncoder::UnitTest, Data doesn't match to the source file!"));

    //3. Encode it with file.
    bRet = bRet && Encode(pszSourceFileName, pszEncodeFileName);
    BEATS_ASSERT(bRet, _T("Encode data with File failed in CBytesOrderEncoder::UnitTest"));
    bRet = bRet && Decode(pszEncodeFileName, 0, pszDecodeFileName);
    BEATS_ASSERT(bRet, _T("Decode data with File failed in CBytesOrderEncoder::UnitTest"));
    FILE* pFile = _tfopen(pszDecodeFileName, _T("rb"));
    bRet = bRet && pFile != NULL;
    if (bRet)
    {
        CMD5 decodeMD5WithFile(pFile);
        bRet = decodeMD5WithFile == sourceMd5;
        BEATS_ASSERT(bRet, _T("Decode data with File failed in CBytesOrderEncoder::UnitTest, Data doesn't match to the source file!"));
        fclose(pFile);
    }

    //4. Clean up.
    DeleteFile(pszSourceFileName);
    DeleteFile(pszEncodeFileName);
    DeleteFile(pszDecodeFileName);

    return bRet;
}
#endif