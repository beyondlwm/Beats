#include "stdafx.h"
#include "EngineEditor.h"
#include "EditorMainFrame.h"
#include "EngineSplash.h"
#include "wxPtrButtonEditor.h"
#include "EngineCenter.h"
#include "ComponentGraphics_GL.h"

CEngineEditor::CEngineEditor()
    : m_glRC(NULL)
    , m_pMainFrame(NULL)
    , m_pPtrButtonEditor(NULL)
{

}

CEngineEditor::~CEngineEditor()
{
    BEATS_SAFE_DELETE(m_glRC);
}

bool CEngineEditor::OnInit()
{
    bool ret = wxApp::OnInit();
    if ( ret )
    {
        CLanguageManager::GetInstance()->LoadFromFile(eLT_English);
        m_pMainFrame = new CEditorMainFrame(wxT("FCEngineEditor"));
        m_pMainFrame->InitFrame();
        m_pMainFrame->ChangeLanguage();
        m_pMainFrame->Show(true);
        CEngineCenter::GetInstance()->Initialize();
        CreatePtrEditor();
        m_pMainFrame->InitComponentsPage();
#ifndef _DEBUG
        wxBitmap bmp;
        bmp.LoadFile(wxT("../resource/splash.png"), wxBITMAP_TYPE_PNG);
        EngineSplash* pSplash = new EngineSplash(&bmp,
            wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_NO_TIMEOUT,
            0, m_pMainFrame, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxSIMPLE_BORDER|wxSTAY_ON_TOP);
        pSplash->Instance();
        BEATS_SAFE_DELETE(pSplash);
#endif
        // Set working path.
        TCHAR szPath[MAX_PATH];
        GetModuleFileName(NULL, szPath, MAX_PATH);
        int iStrPos = CStringHelper::GetInstance()->FindLastString(szPath, _T("\\"), false);
        BEATS_ASSERT(iStrPos >= 0);
        szPath[iStrPos] = 0;
        m_szWorkingPath.assign(szPath);
    }

    return ret;
}
int CEngineEditor::OnExit()
{
    wxApp::OnExit();
    CEngineCenter::Destroy();
    return 0;
}

wxPGEditor* CEngineEditor::GetPtrEditor() const
{
    return m_pPtrButtonEditor;
}

void CEngineEditor::CreatePtrEditor()
{
    m_pPtrButtonEditor = wxPropertyGrid::RegisterEditorClass(new wxPtrButtonEditor());;
}

CEditorMainFrame* CEngineEditor::GetMainFrame() const
{
    return m_pMainFrame;
}

void CEngineEditor::CreateGLContext(wxGLCanvas* pCanvas)
{
    m_glRC = new wxGLContext(pCanvas);
}

wxGLContext* CEngineEditor::GetGLContext() const
{
    return m_glRC;
}

const TString& CEngineEditor::GetWorkingPath()
{
    return m_szWorkingPath;
}

IMPLEMENT_APP(CEngineEditor)