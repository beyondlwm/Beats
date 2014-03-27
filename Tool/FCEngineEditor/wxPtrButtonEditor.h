#ifndef BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_PTREDITOR_WXPTRBUTTONEDITOR_H__INCLUDE
#define BEATS_COMPONENTS_PROPERTY_WXWIDGETSIMPLEMENT_PTREDITOR_WXPTRBUTTONEDITOR_H__INCLUDE

#include <wx/propgrid/propgrid.h>

class wxPtrButtonEditor : public wxPGTextCtrlEditor
{
    DECLARE_DYNAMIC_CLASS(wxPtrButtonEditor)
    typedef wxPGTextCtrlEditor super;
public:
    wxPtrButtonEditor();
    virtual ~wxPtrButtonEditor();

    virtual wxPGWindowList CreateControls( wxPropertyGrid* propGrid,
        wxPGProperty* property,
        const wxPoint& pos,
        const wxSize& sz ) const;
    virtual bool OnEvent( wxPropertyGrid* propGrid,
        wxPGProperty* property,
        wxWindow* ctrl,
        wxEvent& event ) const;
};

#endif