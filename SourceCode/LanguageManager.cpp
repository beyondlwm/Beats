#include "stdafx.h"
#include "LanguageManager.h"
#include "Resource\ResourcePathManager.h"
#include "Utility\BeatsUtility\Serializer.h"

CLanguageManager* CLanguageManager::m_pInstance = NULL;

CLanguageManager::CLanguageManager()
{

}

CLanguageManager::~CLanguageManager()
{

}

const TString CLanguageManager::GetText(ELanguage language)
{
    TString strLanguage;
    for (int i = 0; i < (int)m_languageVector.size(); ++i)
    {
        if (language == i)
        {
            strLanguage = m_languageVector[i];
        }
    }
    return strLanguage;
}

void CLanguageManager::LoadFromFile(ELanguageType languageType)
{
    TString tmpPath = _T("Language.bin");
    TString FilePath = CResourcePathManager::GetInstance()->GetFullPathForFilename("Language.bin");
    if (tmpPath == FilePath)
    {
        return;
    }
    int count = 0;
    CSerializer tmp(FilePath.c_str());
    tmp >> count;
    m_languageVector.clear();
    for (int i = 0; i < count; ++i)
    {
        TString strEnum;
        tmp >> strEnum;
        TString strChinese;
        tmp >> strChinese;
        TString strEnglish;
        tmp >> strEnglish;
        switch (languageType)
        {
        case eLT_Chinese:
            m_languageVector.push_back(strChinese);
            break;
        case eLT_English:
            m_languageVector.push_back(strEnglish);
            break;
        default:
            break;
        }
    }
}
