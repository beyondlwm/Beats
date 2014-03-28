#include "stdafx.h"
#include "Utility/BeatsUtility/ComponentSystem/ComponentPublic.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentManager.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Utility/BeatsUtility/EnumStrGenerator.h"
#include "Resource/ResourcePathManager.h"
#include "Render/Model.h"
#include "Render/Skeleton.h"
#include "Render/RenderStateParam.h"

void ForceReferenceSymbolForComponent()
{
    // When you are writing in a lib project, this cpp may not be referenced by the exe project.
    // so we add this empty function and make it extern in componentpublic.h, every cpp in the exe project,
    // which contain the componentpublic.h file will reference this symbol.
}

START_REGISTER_COMPONET
REGISTER_COMOPNENT(CModel, _T("Ä£ÐÍ"), _T("Render\\CModel"))
REGISTER_COMOPNENT(CSkeleton, _T("¹Ç÷À"), _T("Render\\CSkeleton"))
REGISTER_COMOPNENT(CStateParam, _T("CStateParam"), _T("Render\\CStateParam"))
REGISTER_COMOPNENT(CBoolParam, _T("CBoolParam"), _T("Render\\CBoolParam"))
REGISTER_COMOPNENT(CUintParam, _T("CUintParam"), _T("Render\\CUintParam"))
REGISTER_COMOPNENT(CFunctionParam, _T("CFunctionParam"), _T("Render\\CFunctionParam"))
REGISTER_COMOPNENT(CBlendParam, _T("CBlendParam"), _T("Render\\CBlendParam"))
REGISTER_COMOPNENT(CBlendEquationParam, _T("CBlendEquationParam"), _T("Render\\CBlendEquationParam"))
REGISTER_COMOPNENT(CCullModeParam, _T("CCullModeParam"), _T("Render\\CCullModeParam"))
REGISTER_COMOPNENT(CClockWiseParam, _T("CClockWiseParam"), _T("Render\\CClockWiseParam"))
REGISTER_COMOPNENT(CPolygonModeParam, _T("CPolygonModeParam"), _T("Render\\CPolygonModeParam"))
REGISTER_COMOPNENT(CShadeModeParam, _T("CShadeModeParam"), _T("Render\\CShadeModeParam"))
REGISTER_COMOPNENT(CStencilParam, _T("CStencilParam"), _T("Render\\CStencilParam"))
END_REGISTER_COMPONET

struct SLuancher
{
    SLuancher()
    {
        CResourcePathManager::GetInstance()->Init();
        const TString& strSourceCodePath = CResourcePathManager::GetInstance()->GetResourcePath(eRPT_SourceCode);
        std::vector<TString> scanPath;
        scanPath.push_back(strSourceCodePath);
        CEnumStrGenerator::GetInstance()->Init(scanPath);
    }
    ~SLuancher()
    {
        CEnumStrGenerator * pInstance= CEnumStrGenerator::GetInstance();
        if (pInstance !=NULL)
        {
            CEnumStrGenerator::Destroy();
        }

        CComponentManager::Destroy();
        CResourcePathManager::Destroy();
    }
}launchme;
