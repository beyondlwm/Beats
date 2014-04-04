// 3DExporter.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "3DSceneExport.h"


C3DSceneExport::C3DSceneExport( void )
{
	GetConversionManager()->SetCoordSystem(IGameConversionManager::IGAME_OGL);

	m_pDataExporter = new CDataExporter;
}

C3DSceneExport::~C3DSceneExport( void )
{
	if(m_pDataExporter)
	{
		BEATS_SAFE_DELETE(m_pDataExporter);
	}
}

int C3DSceneExport::ExtCount()
{
	return 1;
}

const TCHAR* C3DSceneExport::Ext( int /*n*/ )
{
	return _T("max");
}

const TCHAR* C3DSceneExport::LongDesc()
{
#ifdef	_DEBUG
	return _T("FCEngine PLUGIN for 3dsmax 11d");
#else
	return _T("FCEngine PLUGIN for 3dsmax 11r");
#endif
}

const TCHAR* C3DSceneExport::ShortDesc()
{
#ifdef	_DEBUG
	return _T("FCEngine PLUGINd");
#else
	return _T("FCEngine PLUGINr");
#endif
}

const TCHAR* C3DSceneExport::AuthorName()
{
	return _T("FC Engine Team");
}

const TCHAR* C3DSceneExport::CopyrightMessage()
{
	return _T("COPYRIGHT(C) ALL RIGHT RESERVED");
}

const TCHAR* C3DSceneExport::OtherMessage1()
{
	return _T("好玩123");
}

const TCHAR* C3DSceneExport::OtherMessage2()
{
	return _T("好玩123");
}

unsigned int C3DSceneExport::Version()
{
	return 1;
}

void C3DSceneExport::ShowAbout( HWND /*hWnd*/ )
{

}

BOOL C3DSceneExport::SupportsOptions( int /*ext*/, DWORD /*options*/ )
{
	return FALSE;
}

int C3DSceneExport::DoExport( const TCHAR* pszName, ExpInterface* /*pExpInterface*/, Interface* /*pInterface*/, BOOL/* bSuppressPrompts = FALSE*/, DWORD/* options = 0*/)
{
    m_pDataExporter->SetExportFileName(pszName);
	m_pDataExporter->ExportScene();
	return TRUE;
}




