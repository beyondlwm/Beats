#include "stdafx.h"
#include "EngineCenter.h"
#include "Render/RenderObjectManager.h"
#include "Render/AnimationManager.h"
#include "Render/RenderManager.h"
#include "Resource/ResourceManager.h"
#include "Resource/ResourcePathManager.h"
#include "Render/renderer.h"
#include "Utility/BeatsUtility/ComponentSystem/ComponentPublic.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentManager.h"
#include "Utility/BeatsUtility/StringHelper.h"
#include "Utility/BeatsUtility/EnumStrGenerator.h"
#include "Render/SpriteFrameBatchManager.h"
#include "ParticlesSystem/ParticleSystemManager.h"
#include "GUI/System.h"
#include "GUI/Font/FontManager.h"

CEngineCenter* CEngineCenter::m_pInstance = NULL;

CEngineCenter::CEngineCenter()
{

}

CEngineCenter::~CEngineCenter()
{
    CSpriteFrameBatchManager::Destroy();
    FCEngine::ParticleSystemManager::Destroy();
    FCGUI::System::Destroy();
    FCGUI::FontManager::Destroy();
    CRenderObjectManager::Destroy();
    CAnimationManager::Destroy();
    CRenderManager::Destroy();
    CResourcePathManager::Destroy();
    CComponentManager::Destroy();
    CEnumStrGenerator::Destroy();
    CStringHelper::Destroy();
    CRenderer::Destroy();
    CResourceManager::Destroy();
#ifdef _DEBUG
    SymCleanup(GetCurrentProcess());
#endif
}

bool CEngineCenter::Initialize(size_t uWindowWidth/* = 0xFFFFFFFF*/, size_t uWindowHeight/* = 0xFFFFFFFF*/)
{
#ifdef _DEBUG
    SymSetOptions(SYMOPT_LOAD_LINES);
    bool bInitializeSuccess = SymInitialize(GetCurrentProcess(), NULL, TRUE) == TRUE;
    BEATS_ASSERT(bInitializeSuccess);
#endif

    const TString& strSourceCodePath = CResourcePathManager::GetInstance()->GetResourcePath(eRPT_SourceCode);
    std::vector<TString> scanPath;
    scanPath.push_back(strSourceCodePath);
    CEnumStrGenerator::GetInstance()->Init(scanPath);

    if (0xFFFFFFFF == uWindowWidth && 0xFFFFFFFF == uWindowHeight)
    {
        CRenderManager::GetInstance()->Initialize();
    }
    else
    {
        CRenderManager::GetInstance()->InitializeWithWindow(uWindowWidth, uWindowHeight);
    }
    FCGUI::System::GetInstance()->InitFontFace();
    pComponentLauncherFunc();

    return true;
}
