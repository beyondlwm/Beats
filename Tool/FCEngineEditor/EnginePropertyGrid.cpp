#include "stdafx.h"
#include "EnginePropertyGrid.h"
EnginePropertyGrid::EnginePropertyGrid()
{
}

EnginePropertyGrid::~EnginePropertyGrid()
{
} 

void EnginePropertyGrid::FreeEditorCtrl()
{
    wxPropertyGrid::FreeEditors();
}

void EnginePropertyGrid::SetManager( wxPropertyGridManager* pManager )
{
    m_pManager = pManager;
}

wxPropertyGridManager* EnginePropertyGrid::GetManager() const
{
    return m_pManager;
}
