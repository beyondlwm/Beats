#ifndef FCENGINEEDITOR_ENGINEPROPERTYGRID_H__INCLUDE
#define FCENGINEEDITOR_ENGINEPROPERTYGRID_H__INCLUDE

#include <wx/propgrid/propgrid.h>

class EnginePropertyGrid : public wxPropertyGrid
{
public:
    EnginePropertyGrid();
    virtual ~EnginePropertyGrid();
    void FreeEditorCtrl();
    void SetManager(wxPropertyGridManager* pManager);
    wxPropertyGridManager* GetManager() const;
private:
    wxPropertyGridManager* m_pManager;
};


#endif