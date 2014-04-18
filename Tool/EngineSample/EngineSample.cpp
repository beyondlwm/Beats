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
#include "EngineCenter.h"
#include "Render/ShaderProgram.h"
#include "Render/Texture.h"
#include "Resource/ResourcePublic.h"
#include "Resource/ResourceManager.h"
#include "Render/Model.h"
#include "Render/AnimationManager.h"
#include "Render/Sprite.h"
#include "Render/Skin.h"
#include "Render/Skeleton.h"
#include "Render/Animation.h"
#include "Render/Camera.h"
#include "ParticlesSystem/ParticleSystemManager.h"

#include <mmsystem.h>
#ifdef SHOW_UI_CONTROL
#include "GUI/System.h"
#include "GUI/Window/Window.h"
#include "GUI/Renderer/BaseRenderer.h"
#include "GUI/WindowManager.h"
#include "GUI/Event/WindowEvent.h"
#include "GUI/Window/Button.h"
#include "GUI/Layout/GridLayout.h"
#include "GUI/Layout/ScrollLayout.h"
#include "GUI/Layout/VScrollLayout.h"
#include "GUI/Layout/HScrollLayout.h"
#include "Event/MouseEvent.h"
#include "Render/TextureFragManager.h"
#include "GUI/Window/CheckBox.h"
#include "GUI/Window/Slider.h"
#include "GUI/Window/Progress.h"
#include "GUI/Window/TextEdit.h"
#include "GUI/Window/TextLabel.h"
#include "GUI/Window/ListBox.h"
#include "GUI/Window/ComboBox.h"
#include "GUI/Window/ListControl.h"
#include "GUI/Renderer/SliderRenderer.h"
#include "GUI/Renderer/CheckBoxRenderer.h"
#include "GUI/Renderer/ProgressRenderer.h"
#include "GUI/Renderer/TextEditRenderer.h"
#include "GUI/Renderer/TextLabelRenderer.h"
#include "GUI/Renderer/ListBoxRenderer.h"
#include "GUI/Renderer/ComboBoxRenderer.h"

void TestUI();
#endif

static const size_t Window_Width = 800;
static const size_t Window_Height = 600;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                       _In_opt_ HINSTANCE hPrevInstance,
                       _In_ LPTSTR    lpCmdLine,
                       _In_ int       /*nCmdShow*/)
{
    MSG msg;
    {
        ::ZeroMemory(&msg, sizeof(MSG));
        UNREFERENCED_PARAMETER(hPrevInstance);
        UNREFERENCED_PARAMETER(lpCmdLine);
        HACCEL hAccelTable;

        CEngineCenter::GetInstance()->Initialize(Window_Width, Window_Height);

        hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ENGINESAMPLE));

        CModel *model = CRenderObjectManager::GetInstance()->CreateModel();

        SharePtr<CTexture> pTestTexture( 
            CResourceManager::GetInstance()->GetResource<CTexture>(_T("TestTexture.tga"), false));
        model->AddTexture(pTestTexture);

        SharePtr<CSkin> pSkin(
            CResourceManager::GetInstance()->GetResource<CSkin>(
            _T("org.skin"), false));
        model->SetSkin(pSkin);

        SharePtr<CSkeleton> pSkeleton(
            CResourceManager::GetInstance()->GetResource<CSkeleton>(
            _T("org.ske"), false));
        model->SetSkeleton(pSkeleton);

        SharePtr<CAnimation> pAnimation(
            CResourceManager::GetInstance()->GetResource<CAnimation>(
            _T("org.ani"), false));
        model->SetAnimaton(pAnimation);
        model->PlayAnimationById(0, 0.f, true);
#ifdef SHOW_UI_CONTROL
        TestUI();
#endif
        CCamera* pCamera = new CCamera();
        pCamera->SetWidth(Window_Width);
        pCamera->SetHeight(Window_Height);
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
                CAnimationManager::GetInstance()->Update((float)delta/1000);
                CRenderManager::GetInstance()->SetCamera(pCamera);
                CRenderManager::GetInstance()->Render();
#ifdef SHOW_UI_CONTROL
                FCGUI::System::GetInstance()->Update((float)delta/1000);
#endif
                CRenderManager::GetInstance()->SetCamera(pCamera);
                last = curr;
            }
        }
        BEATS_SAFE_DELETE(pCamera);
    }
    CEngineCenter::Destroy();
    return (int) msg.wParam;
}

#ifdef SHOW_UI_CONTROL
void OnSelChange(BaseEvent *event);

void TestUI()
{
    SharePtr<CTexture> texture = 
        CResourceManager::GetInstance()->GetResource<CTexture>(_T("DemoShop0.png"), false);

    kmVec2 point;
    kmVec2Fill(&point, 168, 0);
    kmVec2 size;
    kmVec2Fill(&size, 421, 318);
    CTextureFragManager::GetInstance()->CreateTextureFrag(_T("BG"), texture, point, size);
    kmVec2Fill(&point, 338, 18);
    kmVec2Fill(&size, 1, 1);
    CTextureFragManager::GetInstance()->CreateTextureFrag(_T("ShopPanel"), texture, point, size);
    kmVec2Fill(&point, 591, 0);
    kmVec2Fill(&size, 50, 23);
    CTextureFragManager::GetInstance()->CreateTextureFrag(_T("ButtonNormal"), texture, point, size);
    kmVec2Fill(&point, 643, 0);
    kmVec2Fill(&size, 50, 23);
    CTextureFragManager::GetInstance()->CreateTextureFrag(_T("ButtonPressed"), texture, point, size);
    kmVec2Fill(&point, 602, 345);
    kmVec2Fill(&size, 1, 1);
    CTextureFragManager::GetInstance()->CreateTextureFrag(_T("ButtonDisabled"), texture, point, size);

    kmVec2Fill(&point, 590, 321); 
    kmVec2Fill(&size, 74, 43);
    CTextureFragManager::GetInstance()->CreateTextureFrag(_T("CheckBoxBG"), texture, point, size);

    kmVec2Fill(&point, 546, 321); 
    kmVec2Fill(&size, 44, 43);
    CTextureFragManager::GetInstance()->CreateTextureFrag(_T("CheckBoxIndicator"), texture, point, size);

    kmVec2Fill(&point, 494, 325); 
    kmVec2Fill(&size, 41, 40);
    CTextureFragManager::GetInstance()->CreateTextureFrag(_T("CheckBoxIndicatorChecked"), texture, point, size);

    kmVec2Fill(&point, 454, 6); 
    kmVec2Fill(&size, 82, 11);
    CTextureFragManager::GetInstance()->CreateTextureFrag(_T("SliderBar"), texture, point, size);

    kmVec2Fill(&point, 643, 0);
    kmVec2Fill(&size, 49, 23);
    CTextureFragManager::GetInstance()->CreateTextureFrag(_T("SliderBall"), texture, point, size);

    kmVec2Fill(&point, 458, 280);
    kmVec2Fill(&size, 123, 28);
    CTextureFragManager::GetInstance()->CreateTextureFrag(_T("EditBoxBK"), texture, point, size);

    using namespace FCGUI;
    Window *wndBG = WindowManager::GetInstance()->Create<Window>(_T("BG"), nullptr);
    wndBG->SetPosSize(0.f, 0.f, 0.5f, 0.5f, 0.f, 0.f, 1.f, 1.f);
    wndBG->SetRenderer(new BaseRenderer());
    wndBG->Renderer()->AddLayer(_T("BG"));
    wndBG->SetAnchor(0.5f, 0.5f);

    Window *wndShopPanel = WindowManager::GetInstance()->Create<Window>(_T("ShopPanel"), wndBG);
    wndShopPanel->SetPosSize(250.f, 320.f, 0.f, 0.f, 400.f, 400.f, 0.f, 0.f);
    wndShopPanel->SetAnchor(0.5f, 0.5f);
    wndShopPanel->SetRenderer(new BaseRenderer());
    wndShopPanel->Renderer()->AddLayer(_T("ShopPanel"));

    ComboBox *combo = WindowManager::GetInstance()->Create<ComboBox>(_T("ComboBox"), wndShopPanel);
    combo->SetArea(150.f, 10.f, 0.f, 0.f, 250.f, 40.f, 0.f, 0.f);
    combo->AddString(_T("BUTTON"));
    combo->AddString(_T("SLIDER"));
    combo->AddString(_T("CHECKBOX"));
    combo->AddString( _T("PROGRESS"));
    combo->AddString( _T("EDITBOX"));
    combo->AddString( _T("TEXTLABEL"));
    combo->AddString( _T("LISTBOX"));
    combo->AddString( _T("LISTCONTROL"));
    combo->SubscribeEvent(ComboBox::EVENT_SELECTOK, OnSelChange);

    ComboBoxRenderer *comboRender = static_cast<ComboBoxRenderer*>(combo->Renderer());
    comboRender->AddDropDownLayer(_T("ButtonNormal"),Button::STATE_NORMAL);
    comboRender->AddComboBoxLayer( _T("SliderBar"), _T("ButtonDisabled"), _T("ButtonNormal"));

}

void OnSelChange(BaseEvent *event)
{
    using namespace FCGUI;
    WindowEvent *winevent = static_cast<WindowEvent *>(event);

    FCGUI::ComboBox *combo = static_cast<FCGUI::ComboBox *>(winevent->SourceWindow());
    int currSel = combo->GetCurrSel();
    Window *wndShopPanel = combo->Parent();
    switch (currSel)
    {
    case 0:
        {
            Button *wndBtn = WindowManager::GetInstance()->Create<Button>(_T("TestButton"), wndShopPanel);
            wndBtn->SetArea(10.f, 10.f, 0.f, 0.f, 110.f, 40.f, 0.f, 0.f);
            BaseRenderer *rdrBtn = static_cast<BaseRenderer*>(wndBtn->Renderer());
            rdrBtn->AddLayer(_T("ButtonNormal"), Button::STATE_NORMAL);
            rdrBtn->AddLayer(_T("ButtonPressed"), Button::STATE_PRESSED);
            rdrBtn->AddLayer(_T("ButtonDisabled"), Button::STATE_DISABLED);
        }
        break;
    case  1:
        {
            Slider *slider = WindowManager::GetInstance()->Create<Slider>(_T("TestSliser"),wndShopPanel);
            slider->SetArea(10.0f, 80.0f, 0.0f, 0.0f, 120.0f, 91.0f, 0.0f, 0.0f);
            SliderRenderer* sliderrender = static_cast<SliderRenderer*>(slider->Renderer());
            sliderrender->AddLayer(_T("SliderBall"),_T("SliderBar"),Slider::STATE_NORMAL);
        }
        break;
    case  2:
        {
            CheckBox *checBox = WindowManager::GetInstance()->Create<CheckBox>(_T("TestCheckBox1"),wndShopPanel);
            checBox->SetArea(10.0f, 140.0f, 0.f,  0.f, 50.f, 180.f, 0.f, 0.f);

            CheckBoxRenderer *renderCheckBox = static_cast<CheckBoxRenderer*>(checBox->Renderer());
            renderCheckBox->AddLayer(_T("CheckBoxBG"),_T("CheckBoxIndicator"),CheckBox::STATE_NORMAL_UNCHECKED);
            renderCheckBox->AddLayer(_T("CheckBoxBG"),_T("CheckBoxIndicatorChecked"),CheckBox::STATE_NORMAL_CHECKED);
        }
        break;
    case 3:
        {
            Progress *progress = WindowManager::GetInstance()->Create<Progress>(_T("process"), wndShopPanel);
            progress->SetArea(10.0f,240.0f,0.0f, 0.0f, 210.0f, 260.0f, 0.f, 0.f);

            progress->SetPos(70);
            ProgressRenderer* progressRender = static_cast<ProgressRenderer*>(progress->Renderer());
            progressRender->AddLayer(_T("SliderBall"), _T("SliderBar"), Progress::STATE_NORMAL);
        }
        break;
    case 4:
        {
            TextEdit *textedit = WindowManager::GetInstance()->Create<TextEdit>(_T("TextEdit"), wndShopPanel);
            textedit->SetArea(0.0f, 320.0f,0.0f, 0.0f, 200.0f, 380.0f, 0.f, 0.f);
            textedit->SetText(_T("±à¼­¿ò"));

            TextEditRenderer* editrender = static_cast<TextEditRenderer*>(textedit->Renderer());
            editrender->AddLayer(_T("EditBoxBK"));
        }
        break;
    case  5:
        {
            TextLabel* label = WindowManager::GetInstance()->Create<TextLabel>(_T("TextLabel"), wndShopPanel);
            label->SetArea(300.0f, 250.0f, 0.0f, 0.0f, 380.0f, 300.0f, 0.f, 0.f);
            label->SetText(_T("This is a Text Label"));
        }
        break;
    case  6:
        {
            ListBox *listbox = WindowManager::GetInstance()->Create<ListBox>(_T("ListBox"), wndShopPanel);
            listbox->SetArea(250.f, 280.f , 0.f, 0.f, 350.f, 380.f, 0.f, 0.f);
            listbox->AddString(_T("SEL1"));
            listbox->AddString(_T("SEL2"));
            listbox->AddString(_T("SEL3"));
            listbox->AddString(_T("SEL4"));
            listbox->AddString(_T("SEL5"));
            listbox->SetCurrSel(0);
            ListBoxRenderer *listboxRenderer = static_cast<ListBoxRenderer*>(listbox->Renderer());
            listboxRenderer->AddLayer(_T("ButtonDisabled"), _T("ButtonNormal"));
        }
        break;
    case 7:
        {
            ListControl* listctl = WindowManager::GetInstance()->Create<ListControl>(_T("ListControl"), wndShopPanel);
            listctl->SetArea(0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f);
            listctl->SetColWidth(350.f);
            listctl->SetRowHeight(350.f);
            listctl->Renderer()->AddLayer(_T("EditBoxBK"));
            Window *child = WindowManager::GetInstance()->Create<Window>(_T("child0"), listctl);
            listctl->AddItem(child);
            child->Renderer()->AddLayer(_T("EditBoxBK"));

            Window *child1 = WindowManager::GetInstance()->Create<Window>(_T("child1"), listctl);
            listctl->AddItem(child1);
            child1->Renderer()->AddLayer( _T("SliderBar"));

            Window *child2 = WindowManager::GetInstance()->Create<Window>(_T("child2"), listctl);
            listctl->AddItem(child2);
            child2->Renderer()->AddLayer(_T("ButtonDisabled"));

            Window *child3 = WindowManager::GetInstance()->Create<Window>(_T("child3"), listctl);
            listctl->AddItem(child3);
            child3->Renderer()->AddLayer(_T("ButtonNormal"));
        }
        break;
    default:
        break;
    }
}
#endif

