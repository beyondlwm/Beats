#ifndef BEATS_UTILITY_STRINGHELPER_STRINGHELPER_H__INCLUDE
#define BEATS_UTILITY_STRINGHELPER_STRINGHELPER_H__INCLUDE

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
    bool SplitString(const TCHAR* pParameter, const TCHAR* pSpliter, std::vector<TString>& result, bool bIgnoreSpace = true);

    // Filter and Find
    TString FilterString(const TCHAR* pData, const std::vector<TString>& filters);
    int FindFirstString(const TCHAR* pSource, const TCHAR* pTarget,  bool bCaseSensive);
    int FindLastString(const TCHAR* pSource, const TCHAR* pTarget,  bool bCaseSensive);

    // Check character.
#if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
    // Only available in utf-16
    CStringHelper::EStringCharacterType GetCharacterType(wchar_t character) const;
    CStringHelper::EStringCharacterType GetCharacterType(const char* pszChar) const;
#endif
    bool WildMatch(const TCHAR* pat, const TCHAR* str); 
};

#endif