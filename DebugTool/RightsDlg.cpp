#include "stdafx.h"
#include "RightsDlg.h"
#include "Resource.h"
#include <wx/mstream.h>
///////////////////////////////////////////////////////////////////////////

CRightsDialog::CRightsDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) 
: wxDialog( parent, id, title, pos, size, style )
{
    this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxBoxSizer* pAboutDlgMainSizer;
    pAboutDlgMainSizer = new wxBoxSizer( wxVERTICAL );
    m_pAboutLabel = new wxStaticText( this, wxID_ANY, wxT("Beats Debug Tool is a general utility debug tool empowered by:\n\nBEATS_GAME_SOLUTION\n\nAll rights are reserved"), wxPoint( -1,-1 ), wxDefaultSize, 0 );
    m_pAboutLabel->Wrap( 420 );
    m_pAboutLabel->SetFont( wxFont( 15, 70, 90, 90, false, wxEmptyString ) );
    pAboutDlgMainSizer->Add( m_pAboutLabel, 0, wxALL, 5 );
    SetLogoImage();
    pAboutDlgMainSizer->Add( m_pLogoBitmap, 0, wxALL, 5 );

    SetSizer( pAboutDlgMainSizer );
    pAboutDlgMainSizer->SetSizeHints(this);
    Layout();

    Centre( wxBOTH );
}

CRightsDialog::~CRightsDialog()
{
}

void CRightsDialog::SetLogoImage()
{
    uint32_t eipForHandleFetch = 0;
    BEATS_ASSI_GET_EIP(eipForHandleFetch);
    HMODULE curModule;
    bool getModuleSuccess = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCTSTR) eipForHandleFetch, &curModule) == TRUE;
    BEATS_ASSERT(getModuleSuccess, _T("Get Module Failed!"));
    HRSRC data = FindResource(curModule, MAKEINTRESOURCE(IDR_BeatsLogo), _T("JPG"));
    BEATS_ASSERT(data);
    DWORD dataLength = SizeofResource(curModule, data);
    HGLOBAL resGlobal = LoadResource(curModule, data);
    LPVOID pData = LockResource(resGlobal);
    wxMemoryInputStream memoryInputStream(pData, dataLength);
    wxInitAllImageHandlers();
    wxImage tempImage(memoryInputStream, wxBITMAP_TYPE_JPEG);
    wxBitmap scaledLogomap(tempImage.Scale(265, 150));
    m_pLogoBitmap = new wxStaticBitmap( this, wxID_ANY, scaledLogomap, wxDefaultPosition, wxDefaultSize, 0 );
}
