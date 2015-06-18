#ifndef BEATS_UTILITY_STRINGHELPER_STRINGHELPER_H__INCLUDE
#define BEATS_UTILITY_STRINGHELPER_STRINGHELPER_H__INCLUDE

#include <set>

class CStringHelper
{
    BEATS_DECLARE_SINGLETON(CStringHelper);
public:
    enum EStringCharacterType
    {
        eSCT_Unknown,
        eSCT_Number,
        eSCT_LowerCaseCharacter,
        eSCT_UpperCaseCharacter,
        eSCT_MathOperator,
        eSCT_ControlCharacter,
        eSCT_Chinese,
        eSCT_Japanese,
        eSCT_Korean,
        eSCT_Arabia,
        eSCT_Thai,
        eSCT_Russian,
        eSCT_GermanFrench,
        eSCT_Greek,

        eSCT_Count,
        eSCT_Force32Bit = 0xFFFFFFFF
    };
public:
    bool SplitString(const char* pParameter, const char* pSpliter, std::vector<std::string>& result, bool bIgnoreSpace = true);

    // Filter and Find
    std::string FilterString(const char* pData, const std::set<std::string>& filters);
    int FindFirstString(const char* pSource, const char* pTarget, bool bCaseSensive);
    int FindLastString(const char* pSource, const char* pTarget, bool bCaseSensive);

    // Check character.
#if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
    // Only available in utf-16
    CStringHelper::EStringCharacterType GetCharacterType(wchar_t character) const;
    CStringHelper::EStringCharacterType GetCharacterType(const char* pszChar) const;
#endif
    bool WildMatch(const TCHAR* pat, const TCHAR* str); 
};

#endif