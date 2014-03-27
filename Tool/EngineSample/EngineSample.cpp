// EngineSample.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "EngineSample.h"
#include "GL/glew.h"
#include "glfw3.h"
#include "CCConsole.h"
#include "CCGeometry.h"
#include "PublicDef.h"
#include "Render/RenderPublic.h"
#include "Resource/ResourcePublic.h"
#include "Utility/BeatsUtility/SharePtr.h"
#include "Render/RenderManager.h"
#include "Render/RenderObjectManager.h"
#include "Render/ShaderProgram.h"
#include "Render/Texture.h"
#include "Resource/ResourcePublic.h"
#include "Resource/ResourceManager.h"
#include "Render/Model.h"
#include "Render/AnimationManager.h"
#include "Render/SpriteFrame.h"
#include "Render/SpriteAnimation.h"
#include "Render/Sprite.h"
#include "Render/Skin.h"
#include "Render/Skeleton.h"
#include "Render/Animation.h"
#include <mmsystem.h>

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                       _In_opt_ HINSTANCE hPrevInstance,
                       _In_ LPTSTR    lpCmdLine,
                       _In_ int       /*nCmdShow*/)
{
    MSG msg;
    ::ZeroMemory(&msg, sizeof(MSG));
#ifdef _DEBUG
    SymSetOptions(SYMOPT_LOAD_LINES);
    bool bInitializeSuccess = SymInitialize(GetCurrentProcess(), NULL, TRUE) == TRUE;
    BEATS_ASSERT(bInitializeSuccess);
#endif
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    HACCEL hAccelTable;

    CRenderManager::GetInstance()->InitializeWithWindow(800, 600);

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ENGINESAMPLE));

    CModel *model = CRenderObjectManager::GetInstance()->CreateModel();

    SharePtr<CTexture> pTestTexture( 
        CResourceManager::GetInstance()->GetResource<CTexture>(_T("TestTexture.tga"), false));
    model->AddTexture(pTestTexture);

    SharePtr<CSkin> pSkin(
        CResourceManager::GetInstance()->GetResource<CSkin>(
        _T("..\\Resource\\skin\\org.skin"), false));
    model->SetSkin(pSkin);

    SharePtr<CSkeleton> pSkeleton(
        CResourceManager::GetInstance()->GetResource<CSkeleton>(
        _T("..\\Resource\\skeleton\\org.ske"), false));
    model->SetSkeleton(pSkeleton);

    SharePtr<CAnimation> pAnimation(
        CResourceManager::GetInstance()->GetResource<CAnimation>(
        _T("..\\Resource\\Animation\\org.ani"), false));
    model->SetAnimaton(pAnimation);

    model->PlayAnimationById(0, 0.f, true);

    // Main message loop:
    while (!glfwWindowShouldClose(CRenderManager::GetInstance()->GetMainWindow()))
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (GetMessage(&msg, NULL, 0, 0))
            {
                if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                } 
            }
        }
        else
        {
            static DWORD last = timeGetTime();
            DWORD curr = timeGetTime();
            DWORD delta = curr - last;
            last = curr;
            CAnimationManager::GetInstance()->Update((float)delta/1000);
            CRenderManager::GetInstance()->Render();
        }
    }

    CRenderObjectManager::Destroy();
    CAnimationManager::Destroy();
    CRenderManager::Destroy();
    CResourceManager::Destroy();
#ifdef _DEBUG
    SymCleanup(GetCurrentProcess());
#endif
    return (int) msg.wParam;
}
