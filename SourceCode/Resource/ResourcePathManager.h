#ifndef FCENGINEEDITOR_RESOURCEPATHMANAGER_H__INCLUDE
#define FCENGINEEDITOR_RESOURCEPATHMANAGER_H__INCLUDE

enum EResourcePathType
{
    eRPT_Work,
    eRPT_SourceCode,
    eRPT_Resource,
    eRPT_Shader,
    eRPT_Animation,
    eRPT_SpriteAnimation,
    eRPT_Skin,
    eRPT_Skeleton,
    eRPT_Material,
    eRPT_Texture,

    eRPT_Count,
};
 
static const TCHAR* pszResourcePathName[] =
{
    _T(""),
    _T("SourceCode"),
    _T("Resource"),
    _T("SourceCode/Shader"),
    _T("Resource/Animation"),
    _T("Resource/SpriteAnimation"),
    _T("Resource/Skin"),
    _T("Resource/Skeleton"),
    _T("Resource/Material"),
    _T("Resource/Texture"),
};

class CResourcePathManager
{
    BEATS_DECLARE_SINGLETON(CResourcePathManager);
public:
    void Init();
    const TString& GetResourcePath(EResourcePathType type) const;
    EResourcePathType GetResourcePathType(EResourceType type) const;

private:
    TString m_szPath[eRPT_Count];
};
#endif