#include "stdafx.h"
#include "FileEncoderBase.h"
#include "../UtilityManager.h"
#include "../MD5/md5.h"

static const size_t Read_Buffer_Size = 1024 * 1024 * 10; // 10M

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
    HANDLE hSourceFileHandle = CreateFile(pszSourceFilePath, FILE_ALL_ACCESS, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    BEATS_ASSERT(INVALID_HANDLE_VALUE != hSourceFileHandle, _T("Read File %s Failed in CFileEncoderBase::Encode"), pszSourceFilePath);
    if (INVALID_HANDLE_VALUE != hSourceFileHandle)
    {
        TCHAR newFileName[MAX_PATH];
        if (pszEncodeFilePath == NULL)
        {
            _stprintf(newFileName, _T("%s_encode"), pszSourceFilePath);
            pszEncodeFilePath = newFileName;
        }
        HANDLE hEncodeFileHandle = CreateFile(pszEncodeFilePath, FILE_ALL_ACCESS, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        BEATS_ASSERT(INVALID_HANDLE_VALUE != hEncodeFileHandle, _T("Create File %s Failed in CFileEncoderBase::Encode"), pszEncodeFilePath);
        if (hEncodeFileHandle != INVALID_HANDLE_VALUE)
        {
            SEncodeHeader* pHeader = GetEncodeHeader();
            BEATS_ASSERT(pHeader->m_type == this->GetType(), _T("Header type doesn't match, header type %d encoder type %d"), pHeader->m_type, this->GetType());

            LARGE_INTEGER uFileSize;
            if (GetFileSizeEx(hSourceFileHandle, &uFileSize) == TRUE)
            {
                pHeader->m_uOriDataSize = uFileSize.QuadPart;
                // 1. Write header info at the beginning first for take the place, these data will be rewrite after EncodeFileImpl.
                CUtilityManager::GetInstance()->WriteDataToFile(hEncodeFileHandle, pHeader, pHeader->m_uHeaderSize);

                EncodeImpl(hSourceFileHandle, hEncodeFileHandle);
                if (GetFileSizeEx(hEncodeFileHandle, &uFileSize) == TRUE)
                {
                    // 2. Rewrite header
                    pHeader->m_uEncodeDataSize = uFileSize.QuadPart;
                    SetFilePointer(hEncodeFileHandle, 0, NULL, FILE_BEGIN);
                    bRet = CUtilityManager::GetInstance()->WriteDataToFile(hEncodeFileHandle, pHeader, pHeader->m_uHeaderSize);
                    BEATS_ASSERT(bRet, _T("write encode file failed!"));
                }
            }
            CloseHandle(hEncodeFileHandle);
        }
        CloseHandle(hSourceFileHandle);
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
    HANDLE hEncodeFileHandle = CreateFile(pszSourceFilePath, FILE_ALL_ACCESS, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    BEATS_ASSERT(INVALID_HANDLE_VALUE != hEncodeFileHandle, _T("Read File %s Failed in CFileEncoderBase::Encode"), pszSourceFilePath);
    if (INVALID_HANDLE_VALUE != hEncodeFileHandle)
    {
        TCHAR newFileName[MAX_PATH];
        if (pszDecodeFilePath == NULL)
        {
            _stprintf(newFileName, _T("%s_decode"), pszSourceFilePath);
            pszDecodeFilePath = newFileName;
        }
        HANDLE hDecodeFileHandle = CreateFile(pszDecodeFilePath, FILE_ALL_ACCESS, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        BEATS_ASSERT(INVALID_HANDLE_VALUE != hDecodeFileHandle, _T("Create File %s Failed in CFileEncoderBase::Encode"), pszDecodeFilePath);
        if (hDecodeFileHandle != INVALID_HANDLE_VALUE)
        {
            bool bValidateFile = ReadEncodeHeader(hEncodeFileHandle, uStartPos);
            if (bValidateFile)
            {
                bRet = DecodeImpl(hEncodeFileHandle, uStartPos, hDecodeFileHandle);
            }
            CloseHandle(hDecodeFileHandle);
        }
        CloseHandle(hEncodeFileHandle);
    }
    return bRet;
}

bool CFileEncoderBase::Decode(CSerializer* pSourceSerializer, size_t uStartPos, CSerializer* pDecodeSerializer)
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

bool CFileEncoderBase::ReadEncodeHeader(CSerializer* pEncodeSerializer, size_t uStartPos)
{
    bool bRet = false;
    BEATS_ASSERT(pEncodeSerializer != NULL);
    // 1. Read basic info of header
    pEncodeSerializer->SetReadPos(uStartPos);
    SEncodeHeader* pHeader = GetEncodeHeader();
    size_t uReadCount = pEncodeSerializer->Deserialize(pHeader, sizeof(SEncodeHeader));
    bool bReadHeaderSuccess = uReadCount == sizeof(SEncodeHeader);
    BEATS_ASSERT(bReadHeaderSuccess, _T("Read encode file header failed!"));
    if (bReadHeaderSuccess)
    {
        bool bExamType = pHeader->m_type == GetType();
        BEATS_ASSERT(bExamType, _T("Analyse failed, this file is encoded by type %d, current is %d"), pHeader->m_type, GetType());
        if (bExamType)
        {
            size_t uEncodeFileSize = pEncodeSerializer->GetWritePos();
            bool bExamEncodeDataSize = (uEncodeFileSize - uStartPos) >= pHeader->m_uEncodeDataSize;
            BEATS_ASSERT(bExamEncodeDataSize, _T("Analyse failed, this file's size is too short!"));
            if (bExamEncodeDataSize)
            {
                // 2. Reload the header according to the basic info.
                pEncodeSerializer->SetReadPos(uStartPos);
                size_t uRealHeaderSize = pHeader->m_uHeaderSize;
                bRet = pEncodeSerializer->Deserialize(pHeader, uRealHeaderSize) == uRealHeaderSize;
            }
        }
    }
    return bRet;
}

bool CFileEncoderBase::ReadEncodeHeader(HANDLE hEncodeFile, long long uStartPos)
{
    bool bRet = false;

    // 1. Read basic info of header
    if (INVALID_HANDLE_VALUE != hEncodeFile)
    {
        LARGE_INTEGER uFileSize;
        if (GetFileSizeEx(hEncodeFile, &uFileSize) == TRUE)
        {
            BEATS_ASSERT(uStartPos < uFileSize.QuadPart, _T("Invalid start pos in ReadEncodeHeader! start pos %lld, file size: %lld"), uStartPos, uFileSize.QuadPart);
            LARGE_INTEGER startDistance;
            startDistance.QuadPart = uStartPos;
            SetFilePointerEx(hEncodeFile, startDistance, NULL, FILE_BEGIN);
            SEncodeHeader* pHeader = GetEncodeHeader();
            bRet = CUtilityManager::GetInstance()->ReadDataFromFile(hEncodeFile, pHeader, sizeof(SEncodeHeader));
            BEATS_ASSERT(bRet, _T("Read encode file header failed!"));
            if (bRet)
            {
                bool bExamType = pHeader->m_type == GetType();
                BEATS_ASSERT(bExamType, _T("Analyse failed, this file is encoded by type %d, current is %d"), pHeader->m_type, GetType());
                if (bExamType)
                {
                    long long uEncodeFileSize = uFileSize.QuadPart;
                    bool bExamEncodeDataSize = (uEncodeFileSize - uStartPos) >= pHeader->m_uEncodeDataSize;
                    BEATS_ASSERT(bExamEncodeDataSize, _T("Analyse failed, this file's size is too short!"));
                    if (bExamEncodeDataSize)
                    {
                        // 2. Reload the header according to the basic info.
                        SetFilePointerEx(hEncodeFile, startDistance, NULL, FILE_BEGIN);
                        bRet = CUtilityManager::GetInstance()->ReadDataFromFile(hEncodeFile, pHeader, pHeader->m_uHeaderSize);
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

    HANDLE hSourceFile = CreateFile(pszSourceFileName, FILE_ALL_ACCESS, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSourceFile != INVALID_HANDLE_VALUE)
    {
        srand(GetTickCount());
        size_t* pBuffer = new size_t[Read_Buffer_Size];
        for (size_t i = 0; i < Read_Buffer_Size; i ++)
        {
            pBuffer[i] = rand();
        }
        sourceMd5.Update(pBuffer, sizeof(size_t) * Read_Buffer_Size);
        bRet = CUtilityManager::GetInstance()->WriteDataToFile(hSourceFile, pBuffer, Read_Buffer_Size * sizeof(size_t));
        BEATS_ASSERT(bRet, _T("Write data to file %s failed! Create source file failed in CBytesOrderEncoder::UnitTest"), pszSourceFileName);
        BEATS_SAFE_DELETE_ARRAY(pBuffer);
        CloseHandle(hSourceFile);
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
