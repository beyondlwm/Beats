#include "stdafx.h"
#include "StringHelper.h"
static const uint32_t MAX_CACH_SIZE = 10240;
CStringHelper* CStringHelper::m_pInstance = NULL;

CStringHelper::CStringHelper()
{

}

CStringHelper::~CStringHelper()
{

}

bool CStringHelper::SplitString( const TCHAR* pParameter, const TCHAR* pSpliter, std::vector<TString>& result, bool bIgnoreSpace /*= true*/ )
{
    TCHAR cache[MAX_CACH_SIZE] = {0};
    const TCHAR* pFindStr = _tcsstr(pParameter, pSpliter);
    const TCHAR* pReader = pParameter;
    while (pFindStr != NULL)
    {
        if (bIgnoreSpace)
        {
            TCHAR* pWriter = cache;
            for (uint32_t i = 0; i < ((ptrdiff_t)pFindStr - (ptrdiff_t)pReader) / sizeof(TCHAR); ++i)
            {
                if (pReader[i] != _T(' '))
                {
                    *pWriter = pReader[i];
                    ++pWriter;
                    BEATS_ASSERT(pWriter - cache + 32 < MAX_CACH_SIZE, _T("Buffer too small"));
                    if (pWriter - cache + 32 >= MAX_CACH_SIZE)
                    {
                        _tcscpy(pWriter, _T("BEATS_OMIT"));
                        break;
                    }
                }
            }
        }
        else
        {
            BEATS_ASSERT((uint32_t)pFindStr - uint32_t(pReader) < MAX_CACH_SIZE);
            memcpy(cache, pReader, (ptrdiff_t)pFindStr - (ptrdiff_t)pReader);
        }
        result.push_back(cache);
        memset(cache, 0, MAX_CACH_SIZE);
        pReader = pFindStr + _tcslen(pSpliter);
        pFindStr = _tcsstr(pReader, pSpliter);
    }
    memset(cache, 0, MAX_CACH_SIZE);
    const uint32_t restCount = _tcslen(pReader);
    if (restCount > 0)
    {
        BEATS_ASSERT(restCount < MAX_CACH_SIZE);
        if (bIgnoreSpace)
        {
            TCHAR* pWriter = cache;
            for (uint32_t i = 0; i < restCount; ++i)
            {
                if (pReader[i] != _T(' '))
                {
                    *pWriter = pReader[i];
                    ++pWriter;
                    BEATS_ASSERT(pWriter - cache + 32 < MAX_CACH_SIZE, _T("Buffer too small"));
                    if (pWriter - cache + 32 >= MAX_CACH_SIZE)
                    {
                        _tcscpy(pWriter, _T("BEATS_OMIT"));
                        break;
                    }
                }
            }
            result.push_back(cache);
        }
        else
        {
            result.push_back(pReader);
        }
    }

    return true;
}

TString CStringHelper::FilterString(const TCHAR* pData, const std::vector<TString>& filters )
{
    TString strRet;
    const TCHAR* pReader = pData;
    while (*pReader != 0)
    {
        for (int i = 0; i < (int)filters.size(); ++i)
        {
            if (memcmp(pReader, filters[i].c_str(), filters[i].length()*sizeof(TCHAR)) == 0)
            {
                pReader += filters[i].length();
                i = -1; // To reset the whole loop because the pReader is updated.
            }
        }
        if (*pReader != 0)
        {
            strRet.append(pReader++, 1);
        }
    }
    return strRet;
}

int CStringHelper::FindFirstString( const TCHAR* pSource, const TCHAR* pTarget, bool bCaseSensive )
{
    int iResult = -1;
    uint32_t uTargetLength = _tcslen(pTarget);
    uint32_t uSourceLength = _tcslen(pSource);
    int iCounter = 0;
    if (!bCaseSensive)
    {
        TCHAR* pszCache = new TCHAR[uSourceLength + 1];
        _tcscpy(pszCache, pSource);
        while (uTargetLength <= uSourceLength - iCounter)
        {
            TCHAR lastChar = pszCache[iCounter + uTargetLength];
            if (lastChar == 0)
            {
                break;
            }
            pszCache[iCounter + uTargetLength] = 0;
            bool bMatched = _tcsicmp(pTarget, &pszCache[iCounter]) == 0;
            if (bMatched)
            {
                iResult = iCounter;
                break;
            }
            pszCache[iCounter + uTargetLength] = lastChar;
            ++iCounter;            
        }
        BEATS_SAFE_DELETE_ARRAY(pszCache);
    }
    else
    {
        while (uTargetLength <= uSourceLength - iCounter)
        {
            if (pSource[iCounter] == pTarget[0])
            {
                bool bMatched = memcmp(&pSource[iCounter], pTarget, uTargetLength * sizeof(TCHAR)) == 0;
                if (bMatched)
                {
                    iResult = iCounter;
                    break;
                }
            }
            ++iCounter;
        }
    }

    return iResult;
}

int CStringHelper::FindLastString( const TCHAR* pSource, const TCHAR* pTarget, bool bCaseSensive )
{
    int iResult = -1;
    BEATS_ASSERT(pSource != NULL && pTarget != NULL);
    uint32_t uTargetLength = _tcslen(pTarget);
    uint32_t uSourceLength = _tcslen(pSource);
    BEATS_ASSERT(uTargetLength > 0 && uSourceLength > 0);
    int iCounter = (int)uSourceLength - (int)uTargetLength;
    if (iCounter >= 0)
    {
        if (!bCaseSensive)
        {        
            TCHAR* pszCache = new TCHAR[uSourceLength + 1];
            _tcscpy(pszCache, pSource);

            while (iCounter >= 0)
            {
                TCHAR lastChar = pszCache[iCounter + uTargetLength];
                pszCache[iCounter + uTargetLength] = 0;
                bool bMatched = _tcsicmp(pTarget, &pszCache[iCounter]) == 0;
                if (bMatched)
                {
                    iResult = iCounter;
                    break;
                }
                pszCache[iCounter + uTargetLength] = lastChar;
                --iCounter;
            }
            BEATS_SAFE_DELETE_ARRAY(pszCache);
        }
        else
        {
            while (iCounter > 0)
            {
                if (pSource[iCounter] == pTarget[0])
                {
                    bool bMatched = memcmp(&pSource[iCounter], pTarget, uTargetLength * sizeof(TCHAR)) == 0;
                    if (bMatched)
                    {
                        iResult = iCounter;
                        break;
                    }
                }
                --iCounter;
            }
        }
    }

    return iResult;
}

#if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
CStringHelper::EStringCharacterType CStringHelper::GetCharacterType(wchar_t character) const
{
    EStringCharacterType ret = eSCT_Unknown;

    if (character >= L'0' && character <= L'9')
    {
        ret = eSCT_Number;
    }
    else if (character >= L'a' && character <= L'z')
    {
        ret = eSCT_LowerCaseCharacter;
    }
    else if (character >= L'A' && character <= L'Z')
    {
        ret = eSCT_UpperCaseCharacter;
    }
    else if (character == L'+' ||
             character == L'-' ||
             character == L'*' ||
             character == L'/' ||
             character == L'=' ||
             character == L'>' ||
             character == L'<')
    {
        ret = eSCT_MathOperator;
    }
    else if (character >= 0 && character <= 31)
    {
        ret = eSCT_ControlCharacter;
    }
    else if (character >= 0x4E00 && character <= 0x9FA5)
    {
        ret = eSCT_Chinese;
    }
    else if ((character >= 0x3040 && character <= 0x30FF) || 
            (character >= 0x31F0 && character <= 0x31FF))
    {
        ret = eSCT_Japanese;
    }
    else if ((character >= 0x1100 && character <= 0x11FF) ||
             (character >= 0x3130 && character <= 0x318F) ||
             (character >= 0xAC00 && character <= 0xD7AF) )
    {
        ret = eSCT_Korean;
    }
    else if ((character >= 0x0600 && character <= 0x06FF) ||
            (character >= 0x0750 && character <= 0x077F))
    {
        ret = eSCT_Arabia;
    }
    else if (character >= 0x0E00 && character <= 0x0E7F)
    {
        ret = eSCT_Thai;
    }
    else if (character >= 0x0400 && character <= 0x052F)
    {
        ret = eSCT_Russian;
    }
    else if (character >= 0x00C0 && character <= 0x00FF) 
    {
        ret = eSCT_GermanFrench;
    }
    else if ((character >= 0x0370 && character <= 0x03FF) ||
            (character >= 0x1F00 && character <= 0x1FFF) ||
            (character >= 0x2C89 && character <= 0x2CFF) )
    {
        ret = eSCT_Greek;
    }

    return ret;
}

CStringHelper::EStringCharacterType CStringHelper::GetCharacterType(const char* pszChar) const
{
    wchar_t buffer;
    MultiByteToWideChar(CP_ACP, 0, pszChar, -1, &buffer, sizeof(wchar_t));
    return GetCharacterType(buffer);
}
#endif

bool CStringHelper::WildMatch( const TCHAR* pat, const TCHAR* str )
{
    while (*str) 
    {
        switch (*pat) 
        {
        case '?':
            if (*str == '.') 
                return false;
            break;
        case '*':
            do 
            {
                ++pat;
            }
            while (*pat == '*'); /* enddo */
            if (!*pat)
                return true;
            while (*str) 
                if (WildMatch(pat, str++)) 
                    return true;
            return false;
        default:
            if (_totupper(*str) != _totupper(*pat)) 
                return false;
            break;
        } /* endswitch */
        ++pat, ++str;
    } /* endwhile */
    while (*pat == '*')
        ++pat;
    return !*pat;
}
