#include "stdafx.h"
#include "StringHelper.h"

static const uint32_t MAX_CACH_SIZE = 10240;
static char szStringHelperCacheBuffer[MAX_CACH_SIZE] = { 0 };
CStringHelper* CStringHelper::m_pInstance = NULL;

CStringHelper::CStringHelper()
{

}

CStringHelper::~CStringHelper()
{

}

bool CStringHelper::SplitString(const char* pParameter, const char* pSpliter, std::vector<std::string>& result, bool bIgnoreSpace /*= true*/)
{
    static std::string strIgnoreSpaceParam;
    if (bIgnoreSpace)
    {
        std::set<std::string> filter;
        filter.insert(" ");
        strIgnoreSpaceParam = FilterString(pParameter, filter);
        pParameter = strIgnoreSpaceParam.c_str();
    }
    const char* pFindStr = strstr(pParameter, pSpliter);
    const char* pReader = pParameter;
    while (pFindStr != NULL)
    {
        BEATS_ASSERT((uint32_t)pFindStr - uint32_t(pReader) < MAX_CACH_SIZE);
        uint32_t uCount = (ptrdiff_t)pFindStr - (ptrdiff_t)pReader;
        memcpy(szStringHelperCacheBuffer, pReader, uCount);
        szStringHelperCacheBuffer[uCount] = 0;
        result.push_back(szStringHelperCacheBuffer);
        pReader = pFindStr + strlen(pSpliter);
        pFindStr = strstr(pReader, pSpliter);
    }
    const uint32_t restCount = (const uint32_t)(strlen(pReader));
    if (restCount > 0)
    {
        result.push_back(pReader);
    }

    return true;
}

std::string CStringHelper::FilterString(const char* pData, const std::set<std::string>& filters)
{
    std::string strRet;
    const char* pReader = pData;
    while (*pReader != 0)
    {
        // Reverse iterate, so the longer string filter will be test first, this can handle filters like : filters = {a,ab,abc} data = abcdefg 
        // of course will get "defg" not "bcdefg"
        for (std::set<std::string>::const_reverse_iterator rIter = filters.rbegin(); rIter != filters.rend() && *pReader != 0;)
        {
            const std::string& strFilter = *rIter;
            if (memcmp(pReader, strFilter.c_str(), strFilter.length() * sizeof(char)) == 0)
            {
                pReader += strFilter.length();
                rIter = filters.rbegin(); // Re-detect the filter at new pos.
            }
            else
            {
                ++rIter;
            }
        }
        if (*pReader != 0)
        {
            strRet.append(pReader++, 1);
        }
    }
    return strRet;
}

int CStringHelper::FindFirstString( const char* pSource, const char* pTarget, bool bCaseSensive )
{
    std::string strSource = pSource;
    std::string strTarget = pTarget;
    if (!bCaseSensive)
    {
        std::transform(strSource.begin(), strSource.end(), strSource.begin(), ::toupper);
        std::transform(strTarget.begin(), strTarget.end(), strTarget.begin(), ::toupper);
    }
    return strSource.find(strTarget);
}

int CStringHelper::FindLastString( const char* pSource, const char* pTarget, bool bCaseSensive )
{
    std::string strSource = pSource;
    std::string strTarget = pTarget;
    if (!bCaseSensive)
    {
        std::transform(strSource.begin(), strSource.end(), strSource.begin(), ::toupper);
        std::transform(strTarget.begin(), strTarget.end(), strTarget.begin(), ::toupper);
    }
    return strSource.rfind(strTarget);
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

bool CStringHelper::WildMatch( const char* pat, const char* str )
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
