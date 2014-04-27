#include "stdafx.h"
#include <wx/statline.h>
#include "EditAnimationDialog.h"
#include "timebarframe.h"
#include "EditorMainFrame.h"
#include "Utility/BeatsUtility/UtilityManager.h"
#include "Render/RenderObjectManager.h"
#include "Render/Texture.h"
#include "Render/SkeletonBone.h"
#include "Render/AnimationController.h"
#include "Resource/ResourceManager.h"
#include "EngineEditor.h"
#include "AnimationGLWindow.h"
#include "Render/RenderManager.h"

BEGIN_EVENT_TABLE(EditAnimationDialog, EditDialogBase)
    EVT_BUTTON(ID_BUTTON_SELECTANIDIR, EditAnimationDialog::OnSelectFile)
    EVT_BUTTON(ID_BUTTON_SELECTSKEDIR, EditAnimationDialog::OnSelectFile)
    EVT_BUTTON(ID_BUTTON_SELECTSKIDIR, EditAnimationDialog::OnSelectFile)
    EVT_BUTTON(ID_BUTTON_CLEANLOG, EditAnimationDialog::OnClearLog)
    EVT_BUTTON(ID_BUTTON_PLAY, EditAnimationDialog::OnPlayAnimation)
    EVT_BUTTON(ID_BUTTON_PAUSE, EditAnimationDialog::OnPauseAnimation)
    EVT_BUTTON(ID_BUTTON_STOP, EditAnimationDialog::OnStopAnimation)
    EVT_CHECKBOX(ID_CHECKBOX_LOOP,EditAnimationDialog::OnLoopPlayAnimation)
    EVT_LISTBOX(ID_LISTBOX_ANIMATION, EditAnimationDialog::OnAnimationListBox)
    EVT_LISTBOX(ID_LISTBOX_SKELETON, EditAnimationDialog::OnSkeketonListBox)
    EVT_LISTBOX(ID_LISTBOX_SKIN, EditAnimationDialog::OnSkinListBox)
    EVT_CHOICE(ID_CHOICE_SKELETON, EditAnimationDialog::OnSkeletonChoice)
    EVT_BUTTON(ID_BUTTON_SELECT,EditAnimationDialog::OnSelectBoneButton)
    EVT_CHECKBOX(ID_CHECKBOX_SKELETONVISIBLE,EditAnimationDialog::OnSkeletonCheckbox)
    EVT_CHECKBOX(ID_CHECKBOX_SKELETONBONEVISIBLE,EditAnimationDialog::OnSkeletonBoneCheckbox)
END_EVENT_TABLE()

    EditAnimationDialog::EditAnimationDialog(wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
    : EditDialogBase(parent, id, title, pos, size, style, name)
    , m_bAnimation(false)
    , m_bSkeleton(false)
    , m_bSkin(false)
    , m_bIsLoop(false)
    , m_bIsVisibleBone(false)
    , m_bIsVisibleCoordinate(false)
    , m_bIsSelectAll(false)
    , m_bISLanguageSwitch(false)
{
    wxToolTip::Enable(true);
    wxButton* pButton = NULL;

    wxSize client_size = GetClientSize();
    m_Manager.SetManagedWindow(this);
    wxBoxSizer* pTopRightSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pTopRightTopSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pTopRightCenterSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pTopRightBottomSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pLogPanelSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pTimeBarPanelSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* pTimeBarPanelTopSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pTopRightTopBottomSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pTopRightCenterBottomSizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer* pTopRightBottomBottomSizer = new wxBoxSizer(wxHORIZONTAL);

    wxPanel* pLogPanel = new wxPanel(this);
    wxPanel* pTimeBarPanel = new wxPanel(this);

    CRenderManager::GetInstance()->Initialize();
    
    wxGLContext* pContext = static_cast<CEngineEditor*>(wxApp::GetInstance())->GetGLContext();
    m_pAnimationGLWindow = new CAnimationGLWindow(this, pContext);
    m_pTopRight = new wxPanel(this);
    
    wxTextCtrl* pTextCtrl = new wxTextCtrl(pLogPanel, wxID_ANY, _T(""), wxPoint(0,0), client_size, wxNO_BORDER | wxTE_MULTILINE);
    m_pTimeBar = new TimeBarFrame(pTimeBarPanel);
    m_pTimeBar->SetFrameWindow(this);

    wxSize ButtonSize = wxSize(DEFAULT_PGBUTTONWIDTH,DEFAULT_PGBUTTONWIDTH);
    pTextCtrl->SetEditable(false);
    wxLog::DisableTimestamp();
    m_pLogOld = wxLog::SetActiveTarget(new wxLogTextCtrl(pTextCtrl));
    m_pLog = pTextCtrl;
    pLogPanel->SetSizer(pLogPanelSizer);
    pButton = new wxButton(pLogPanel, ID_BUTTON_CLEANLOG, _T("X"), wxDefaultPosition, ButtonSize);
    pButton->SetToolTip(_T("clean log"));
    pLogPanelSizer->Add(pButton, 0, wxALIGN_RIGHT|wxALL, 0);
    pLogPanelSizer->Add(new wxStaticLine(pLogPanel, wxLI_HORIZONTAL), 0, wxGROW|wxALL, 0);
    pLogPanelSizer->Add(m_pLog, 1, wxGROW|wxALL, 0);

    pTimeBarPanel->SetSizer(pTimeBarPanelSizer);
    pTimeBarPanelSizer->Add(pTimeBarPanelTopSizer, 0, wxGROW|wxALL, 0);
    pTimeBarPanelSizer->Add(m_pTimeBar, 1, wxGROW|wxALL, 0);
    m_pButtonPlay = new wxButton(pTimeBarPanel, ID_BUTTON_PLAY, _T("|>"), wxDefaultPosition, ButtonSize);
    m_pButtonPlay->SetToolTip(_T("Play"));
    pTimeBarPanelTopSizer->Add(m_pButtonPlay, 0, wxALIGN_RIGHT|wxALL, 0);
    m_pButtonPause = new wxButton(pTimeBarPanel, ID_BUTTON_PAUSE, _T("||"), wxDefaultPosition, ButtonSize);
    m_pButtonPause->SetToolTip(_T("Pause"));
    pTimeBarPanelTopSizer->Add(m_pButtonPause, 0, wxALIGN_RIGHT|wxALL, 0);
    m_pButtonStop = new wxButton(pTimeBarPanel, ID_BUTTON_STOP, _T("[]"), wxDefaultPosition, ButtonSize);
    m_pButtonStop->SetToolTip(_T("Stop"));
    pTimeBarPanelTopSizer->Add(m_pButtonStop, 0, wxALIGN_RIGHT|wxALL, 0);
    m_pLoopCheckBox = new wxCheckBox(pTimeBarPanel, ID_CHECKBOX_LOOP, _("Loop"));
    pTimeBarPanelTopSizer->Add(m_pLoopCheckBox, 0, wxALIGN_RIGHT|wxALL, 0);


    m_pAnimitionListBox = new wxListBox(m_pTopRight, ID_LISTBOX_ANIMATION);
    m_pSkeletonListBox = new wxListBox(m_pTopRight, ID_LISTBOX_SKELETON, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_MULTIPLE);
    m_pSkinListBox = new wxListBox(m_pTopRight, ID_LISTBOX_SKIN);
    m_pSkeletonChoice = new wxChoice(m_pTopRight, ID_CHOICE_SKELETON);

    GetResourceList(CResourcePathManager::GetInstance()->GetResourcePath(eRPT_Animation));
    GetResourceList(CResourcePathManager::GetInstance()->GetResourcePath(eRPT_Skeleton));
    GetResourceList(CResourcePathManager::GetInstance()->GetResourcePath(eRPT_Skin));

    m_pTopRight->SetSizer(pTopRightSizer);
    pTopRightSizer->Add(pTopRightTopSizer, 1, wxGROW|wxALL, 5);
    pTopRightSizer->Add(new wxStaticLine(m_pTopRight, wxLI_HORIZONTAL), 0, wxGROW|wxALL, 5);
    pTopRightSizer->Add(pTopRightCenterSizer, 1, wxGROW|wxALL, 5);
    pTopRightSizer->Add(new wxStaticLine(m_pTopRight, wxLI_HORIZONTAL), 0, wxGROW|wxALL, 5);
    pTopRightSizer->Add(pTopRightBottomSizer, 1, wxGROW|wxALL, 5);


    pTopRightTopSizer->Add(m_pAnimitionListBox, 1, wxGROW|wxALL, 0);
    pTopRightTopSizer->Add(pTopRightTopBottomSizer, 0, wxGROW|wxALL, 0);
    pTopRightCenterSizer->Add(m_pSkeletonChoice, 0, wxGROW|wxALL, 0);
    pTopRightCenterSizer->Add(m_pSkeletonListBox, 1, wxGROW|wxALL, 1);
    pTopRightCenterSizer->Add(pTopRightCenterBottomSizer, 0, wxGROW|wxALL, 0);
    pTopRightBottomSizer->Add(m_pSkinListBox, 1, wxGROW|wxALL, 0);
    pTopRightBottomSizer->Add(pTopRightBottomBottomSizer, 0, wxGROW|wxALL, 0);

    m_pAnimationCheckBox = new wxCheckBox(m_pTopRight, wxID_ANY, _T(""));
    m_pAnimationCheckBox->Disable();
    pTopRightTopBottomSizer->AddStretchSpacer();
    pTopRightTopBottomSizer->Add(m_pAnimationCheckBox, 0, wxGROW|wxALL, 5);
    m_pButtonAnimaton = new wxButton(m_pTopRight, ID_BUTTON_SELECTANIDIR, _T("Animation"));
    pTopRightTopBottomSizer->Add(m_pButtonAnimaton, 0, wxGROW|wxALL, 0);
    pTopRightTopBottomSizer->AddStretchSpacer();

    m_pSKECheckBox = new wxCheckBox(m_pTopRight, ID_CHECKBOX_SKELETONVISIBLE, _T("SKE"));
    m_pSKEBCheckBox = new wxCheckBox(m_pTopRight, ID_CHECKBOX_SKELETONBONEVISIBLE, _T("SKEB"));
    pTopRightCenterBottomSizer->Add(m_pSKECheckBox, 0, wxGROW|wxALL, 0);
    pTopRightCenterBottomSizer->Add(m_pSKEBCheckBox, 0, wxGROW|wxALL, 0);
    m_pSkeletonCheckBox = new wxCheckBox(m_pTopRight, wxID_ANY, _T(""));
    m_pSkeletonCheckBox->Disable();
    pTopRightCenterBottomSizer->AddStretchSpacer();
    pTopRightCenterBottomSizer->Add(m_pSkeletonCheckBox, 0, wxGROW|wxALL, 5);
    wxSize buttonSize = wxSize(60,0);
    m_pSkeSelectButton = new wxButton(m_pTopRight, ID_BUTTON_SELECT, _T("SelectAll"), wxDefaultPosition, buttonSize);
    m_pButtonSkeleton = new wxButton(m_pTopRight, ID_BUTTON_SELECTSKEDIR, _T("Skeleton"), wxDefaultPosition, buttonSize);
    pTopRightCenterBottomSizer->Add(m_pButtonSkeleton, 0, wxGROW|wxALL, 0);
    pTopRightCenterBottomSizer->Add(m_pSkeSelectButton, 0, wxGROW|wxALL, 0);
    pTopRightCenterBottomSizer->AddStretchSpacer();

    m_pSkinCheckBox = new wxCheckBox(m_pTopRight, wxID_ANY, _T(""));
    m_pSkinCheckBox->Disable();
    pTopRightBottomBottomSizer->AddStretchSpacer();
    pTopRightBottomBottomSizer->Add(m_pSkinCheckBox, 0, wxGROW|wxALL, 5);
    m_pButtonSkin = new wxButton(m_pTopRight, ID_BUTTON_SELECTSKIDIR, _T("Skin"));
    pTopRightBottomBottomSizer->Add(m_pButtonSkin, 0, wxGROW|wxALL, 0);
    pTopRightBottomBottomSizer->AddStretchSpacer();

    m_Manager.AddPane(m_pAnimationGLWindow, wxAuiPaneInfo().CenterPane().
        Name(wxT("left")).
        Center());
    m_Manager.AddPane(m_pTopRight, wxAuiPaneInfo().MinSize(260,0).CenterPane().
        Name(wxT("right")).
        Right());
    m_Manager.AddPane(pTimeBarPanel, wxAuiPaneInfo().MinSize(100,200).CenterPane().
        Name(wxT("timebar")).
        Bottom());
    m_Manager.AddPane(pLogPanel, wxAuiPaneInfo().CenterPane().
        Name(wxT("log")).
        Bottom());

    wxLogMessage(wxT("This is edit log."));
    m_Manager.Update();
}

EditAnimationDialog::~EditAnimationDialog()
{
    m_Manager.UnInit();
    wxLog* p = wxLog::SetActiveTarget(m_pLogOld);
    BEATS_SAFE_DELETE(p);
}

void EditAnimationDialog::OnSelectFile(wxCommandEvent& event)
{
    if (event.GetId() == ID_BUTTON_SELECTANIDIR)
    {
        m_pAnimitionListBox->Clear();
        GetResourceList(CResourcePathManager::GetInstance()->GetResourcePath(eRPT_Animation));
    }
    else if (event.GetId() == ID_BUTTON_SELECTSKEDIR)
    {
        m_pSkeSelectButton->SetLabel(_T("Selected"));
        SetAllSkeletonAndBoneVisible();
        m_pSkeletonListBox->Clear();
        m_pSkeletonChoice->Clear();
        GetResourceList(CResourcePathManager::GetInstance()->GetResourcePath(eRPT_Skeleton));
    }
    else if (event.GetId() == ID_BUTTON_SELECTSKIDIR)
    {
        m_pSkinListBox->Clear();
        GetResourceList(CResourcePathManager::GetInstance()->GetResourcePath(eRPT_Skin));
    }
}

void EditAnimationDialog::OnClearLog(wxCommandEvent& /*event*/)
{
    m_pLog->Clear();
}

void EditAnimationDialog::OnPlayAnimation( wxCommandEvent& /*event*/ )
{
    bool bIsOK = true;
    wxString strlog;
    if (!m_pAnimationCheckBox->GetValue())
    {
        bIsOK = false;
        strlog = _T("Animation is not ready \n");
    }
    if (!m_pSkeletonCheckBox->GetValue())
    {
        bIsOK = false;
        strlog = _T("Skeleton is not ready \n");
    }
    if (!m_pSkinCheckBox->GetValue())
    {
        bIsOK = false;
        strlog = _T("Skin is not ready \n");
    }

    if (bIsOK)
    {
        m_pAnimationGLWindow->GetModel()->PlayAnimationById(0, 0, m_bIsLoop);
    }
    else
    {
        wxLogMessage(strlog);
    }
}

void EditAnimationDialog::OnPauseAnimation( wxCommandEvent& /*event*/ )
{
    if (m_pAnimationGLWindow->GetModel()->GetAnimationController()->IsPlaying())
    {
        m_pAnimationGLWindow->GetModel()->GetAnimationController()->Pause();
    }
    else
    {
        m_pAnimationGLWindow->GetModel()->GetAnimationController()->Resume();
    }
}

void EditAnimationDialog::OnStopAnimation( wxCommandEvent& /*event*/ )
{
    m_pAnimationGLWindow->GetModel()->GetAnimationController()->Stop();
    m_pTimeBar->SetCurrentCursor(0);
}

void EditAnimationDialog::OnAnimationListBox( wxCommandEvent& event )
{
    TString strFileName = event.GetString();
    wxString logMessage;
    m_pAnimation = CResourceManager::GetInstance()->GetResource<CAnimation>(strFileName, false);
    if (m_pAnimation)
    {
        m_pAnimationGLWindow->GetModel()->SetAnimaton(m_pAnimation);
        m_pAnimationCheckBox->SetValue(true);
        logMessage = strFileName;
        logMessage.append(_T(" load success"));
        m_bAnimation = true;
        ShowAnima();
    }
    else
    {
        m_pAnimationCheckBox->SetValue(false);
        logMessage = WARNING + strFileName + _T(" load failed");
    }
    wxLogMessage(logMessage);
}

void EditAnimationDialog::OnSkeketonListBox( wxCommandEvent& /*event*/ )
{
    SetAllSkeletonAndBoneVisible();
    SetSkeletonAndBoneVisible(m_bIsVisibleBone,m_bIsVisibleCoordinate);
}

void EditAnimationDialog::OnSkeletonChoice( wxCommandEvent& event )
{
    if (m_pSkeletonListBox->GetCount() > 0)
    {
        m_pSkeletonListBox->Clear();
        SetAllSkeletonAndBoneVisible();
    }
    m_skeletonBoneType.clear();
    TString strFileName = event.GetString();
    wxString logMessage;
    m_pSkeleton = CResourceManager::GetInstance()->GetResource<CSkeleton>(strFileName, false);
    if (m_pSkeleton)
    {
        m_pAnimationGLWindow->GetModel()->SetSkeleton(m_pSkeleton);
        m_pSkeletonCheckBox->SetValue(true);
        logMessage = strFileName + _T(" load success");
        m_bSkeleton = true;
        ShowAnima();
        for (auto iter = m_pSkeleton->GetBoneMap().begin(); iter != m_pSkeleton->GetBoneMap().end(); iter++)
        {
            if (iter->first != eSBT_Null)
            {
                 m_pSkeletonListBox->Append(pszSkeletonBoneName[iter->first + 1]);
                m_skeletonBoneType.push_back(iter->first);
            }
        }
    }
    else
    {
        m_pSkeletonCheckBox->SetValue(false);
        logMessage = WARNING + strFileName + _T(" load failed");
    }
    wxLogMessage(logMessage);
}

void EditAnimationDialog::OnSkinListBox( wxCommandEvent& event )
{
    TString strFileName = event.GetString();
    wxString logMessage;
    m_pSkin = CResourceManager::GetInstance()->GetResource<CSkin>(strFileName, false);
    if (m_pSkin)
    {
        m_pAnimationGLWindow->GetModel()->SetSkin(m_pSkin);
        m_pSkinCheckBox->SetValue(true);
        logMessage = strFileName + _T(" load success");

        SharePtr<CTexture> pTestTexture = CResourceManager::GetInstance()->GetResource<CTexture>(_T("TestTexture.tga"), false);
        m_pAnimationGLWindow->GetModel()->AddTexture(pTestTexture);

        m_bSkin = true;
        ShowAnima();
    }
    else
    {
        m_pSkinCheckBox->SetValue(false);
        logMessage = WARNING + strFileName + _T(" load failed");
    }
    wxLogMessage(logMessage);
}

void EditAnimationDialog::GetResourceList( wxString path)
{
    SDirectory directory(NULL, path.c_str());
    CUtilityManager::GetInstance()->FillDirectory(directory, false, NULL);  
    for (auto iter = directory.m_pFileList->begin(); iter != directory.m_pFileList->end(); iter++)  
    {  
        wxString temp = (*iter)->cFileName;
        temp.MakeLower();
        if (temp.EndsWith(_T(".ani")))
        {
            m_pAnimitionListBox->Append((*iter)->cFileName);
        }
        else if (temp.EndsWith(_T(".ske")))
        {
            m_pSkeletonChoice->Append((*iter)->cFileName);
        }
        else if (temp.EndsWith(_T(".skin")))
        {
            m_pSkinListBox->Append((*iter)->cFileName);
        }
    }
}

void EditAnimationDialog::InitTimeBar()
{
    m_pTimeBar->SetFps(m_pAnimation->GetFPS());
    m_pTimeBar->AddItems(m_pAnimation);
    if (m_pTimeBar->GetItemCount() > 1)
    {
        m_pTimeBar->DeleteItems(0);
    }
}

void EditAnimationDialog::ShowAnima()
{
    if (m_bAnimation && m_bSkeleton && m_bSkin)
    {
        InitTimeBar();
        m_pAnimationGLWindow->GetModel()->PlayAnimationById(0, 0, true);
    }
}

TimeBarFrame* EditAnimationDialog::GetTimeBar()
{
    return m_pTimeBar;
}

void EditAnimationDialog::OnLoopPlayAnimation( wxCommandEvent& event )
{
    m_bIsLoop = event.IsChecked();
}

SharePtr<CAnimation> EditAnimationDialog::GetAnimation()
{
    return m_pAnimation;
}

bool EditAnimationDialog::IsLoadAllAnimationResource()
{
    return (m_bAnimation && m_bSkeleton && m_bSkin) ? true : false;
}

void EditAnimationDialog::SetAllSkeletonAndBoneVisible(bool bSke, bool bSkeBone)
{
    for (auto iter = m_pSkeleton->GetBoneMap().begin(); iter != m_pSkeleton->GetBoneMap().end(); iter++)
    {
        m_pSkeleton->GetSkeletonBone(iter->first)->SetVisible(bSke);
        m_pSkeleton->GetSkeletonBone(iter->first)->SetCoordinateVisible(bSkeBone);
    }
}

void EditAnimationDialog::SetSkeletonAndBoneVisible(bool bSke,bool bSkeBone)
{
    wxArrayInt    selections = NULL;
    int index = 0;
    int count = m_pSkeletonListBox->GetSelections(selections);
    for (auto i = 0; i != count; i++)
    {
        index = selections.Item(i);
        m_pSkeleton->GetSkeletonBone(m_skeletonBoneType[index])->SetVisible(bSke);
        m_pSkeleton->GetSkeletonBone(m_skeletonBoneType[index])->SetCoordinateVisible(bSkeBone);
    }
}

void EditAnimationDialog::OnSelectBoneButton( wxCommandEvent& event )
{
    wxButton *clicked = (wxButton *)event.GetEventObject();
    if(m_pSkeletonListBox->GetCount() > 0)
    {
        if (clicked->GetLabelText() == _T("SelectAll") ||
            clicked->GetLabelText() == CLanguageManager::GetInstance()->GetText(eL_SelectAll))
        {
            clicked->SetLabel( m_bISLanguageSwitch ? CLanguageManager::GetInstance()->GetText(eL_DeselectAll) : _T("Deselect"));
            for(unsigned i = 0; i != m_pSkeletonListBox->GetCount(); i++)
            {
                m_pSkeletonListBox->SetSelection(i);
            }
            SetAllSkeletonAndBoneVisible(m_bIsVisibleBone,m_bIsVisibleCoordinate);
            m_bIsSelectAll = true;
        }
        else
        {
            SetAllSkeletonAndBoneVisible();
            DelListboxSelect();
            clicked->SetLabel(m_bISLanguageSwitch ? CLanguageManager::GetInstance()->GetText(eL_SelectAll) :_T("SelectAll"));
            m_bIsSelectAll = false;
        }
    }
}

void EditAnimationDialog::OnSkeletonCheckbox( wxCommandEvent& event )
{
    m_bIsVisibleBone = event.IsChecked();
    SetSkeletonAndBoneVisible(m_bIsVisibleBone, m_bIsVisibleCoordinate);
}

void EditAnimationDialog::OnSkeletonBoneCheckbox( wxCommandEvent& event )
{
    m_bIsVisibleCoordinate = event.IsChecked();
    SetSkeletonAndBoneVisible(m_bIsVisibleBone, m_bIsVisibleCoordinate);
}

void EditAnimationDialog::DelListboxSelect()
{
    for (unsigned i = 0; i != m_pSkeletonListBox->GetCount(); i++)
    {
        m_pSkeletonListBox->Deselect(i);
    }
}

wxWindow* EditAnimationDialog::GetCanvas() const
{
    return m_pAnimationGLWindow;
}

void EditAnimationDialog::LanguageSwitch()
{
    m_bISLanguageSwitch = true;
    CLanguageManager* pLanguageManager = CLanguageManager::GetInstance();
    m_pButtonAnimaton->SetLabel(pLanguageManager->GetText(eL_Animation));
    m_pSKECheckBox->SetLabel(pLanguageManager->GetText(eL_SKE));
    m_pSKEBCheckBox->SetLabel(pLanguageManager->GetText(eL_SKEB));
    m_pButtonSkeleton->SetLabel(pLanguageManager->GetText(eL_Skeleton));
    m_pButtonSkin->SetLabel(pLanguageManager->GetText(eL_Skin));

    m_pButtonPlay->SetToolTip(pLanguageManager->GetText(eL_Play));
    m_pButtonPause->SetToolTip(pLanguageManager->GetText(eL_Pause));
    m_pButtonStop->SetToolTip(pLanguageManager->GetText(eL_Stop));

    m_pLoopCheckBox->SetLabel(pLanguageManager->GetText(eL_Loop));
    SetLanguage();
}

void EditAnimationDialog::SetLanguage()
{
    if (m_bIsSelectAll)
    {
        m_pSkeSelectButton->SetLabel(CLanguageManager::GetInstance()->GetText(eL_DeselectAll));
    } 
    else
    {
        m_pSkeSelectButton->SetLabel(CLanguageManager::GetInstance()->GetText(eL_SelectAll));
    }
}
