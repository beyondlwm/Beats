#ifndef FCENGINEEDITOR_LANGUAGEMANAGER_H__INCLUDE
#define FCENGINEEDITOR_LANGUAGEMANAGER_H__INCLUDE
#include "Language.h"

enum ELanguageType
{
    eLT_Chinese,
    eLT_English,
};
class CLanguageManager
{
    BEATS_DECLARE_SINGLETON(CLanguageManager);
public:
    const TString GetText(ELanguage language);
    void LoadFromFile(ELanguageType languageType);
private:
    std::vector<TString> m_languageVector;
};
#endif