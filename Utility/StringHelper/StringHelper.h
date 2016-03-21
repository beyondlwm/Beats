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

    uint32_t Utf8GetByteNum(char byte);
    uint32_t UnicodeGetByteNumFrom(uint32_t code);
    const char *Utf8ExtractCodePoint(const char* pBuffer, uint32_t& code);
    std::wstring Utf8ToWString(const char* utf8str);
    std::string WStringToUtf8(const wchar_t* wstr);
    uint32_t BKDRHash(const char* strData) const;
    TString ToLower(const TString& strIn) const;
    TString ToUpper(const TString& strIn) const;

    // Check character.
#if (BEATS_PLATFORM == BEATS_PLATFORM_WIN32)
    // Only available in utf-16
    CStringHelper::EStringCharacterType GetCharacterType(wchar_t character) const;
    CStringHelper::EStringCharacterType GetCharacterType(const char* pszChar) const;
    std::string Utf8ToString(const char* utf8str);
    std::string StringToUtf8(const char* str);

#endif
    bool WildMatch(const char* pat, const char* str); 
};

#endif