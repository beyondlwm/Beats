#include "stdafx.h"
#include "Initializer.h"
#include "Sound/SoundManager.h"
#include "Control/InputManager.h"
#include "Control/Keyboard.h"
#include "Control/Mouse.h"
#include "UI/UIManager.h"
#include "UI/UIFactory.h"
#include "Render/RenderManager.h"
#include "Render/Particle/ParticleManager.h"
#include "Game/Game.h"
#include "Game/Tasks/RecordTrack.h"
#include "Game/Tasks/PlayTrack.h"
#include "Game/Tasks/TitleTask.h"
#include "Game/Object/ObjectManager.h"
#include "Physics/PhysicsManager.h"
#include "DebugTool/DebugTool.h"
#include "..\..\Utility\UtilityManager.h"

CInitializer* CInitializer::m_pInstance = NULL;

CInitializer::CInitializer()
{

}

CInitializer::~CInitializer()
{

}

void CInitializer::Init(const HWND& hwnd, const HINSTANCE& hInstance)
{
    CSoundManager::GetInstance()->Init(hwnd);
    CInputManager::GetInstance()->Init(hwnd, hInstance);
    CRenderManager::GetInstance()->Init(hwnd, BEATS_WINDOW_WIDTH, BEATS_WINDOW_HEIGHT, true);
    CObjectManager::GetInstance()->Init();
    CUIManager::GetInstance()->Init();
    CUIFactory::GetInstance()->Init();
    CParticleManager::GetInstance()->Init();
    CPhysicsManager::GetInstance()->Init();
    CGame::GetInstance()->RegisterTask(eTT_Title, new CTitleTask());
    CGame::GetInstance()->RegisterTask(eTT_RecordTrack, new CRecordTrack());
    CGame::GetInstance()->RegisterTask(eTT_PlayTrack, new CPlayTrack());

    CGame::GetInstance()->Init(hwnd);
}


void CInitializer::Uninit()
{
    CSoundManager::Destroy();
    CInputManager::Destroy();
    CRenderManager::Destroy();
    CUIFactory::Destroy();
    CUIManager::Destroy();
    CObjectManager::Destroy();
    CGame::Destroy();
    CParticleManager::Destroy();
    CDebugTool::Destroy();
    CPhysicsManager::Destroy();
    CUtilityManager::Destroy();
}