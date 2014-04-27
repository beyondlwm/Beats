#ifndef FCENGINEEDITOR_CONSTANTCURVEEDITOR_H__INCLUDE
#define FCENGINEEDITOR_CONSTANTCURVEEDITOR_H__INCLUDE

#include <wx/propgrid/propgrid.h>

class ConstantCurveEditor : public wxPGTextCtrlAndButtonEditor
{
public:
    ConstantCurveEditor();
    virtual ~ConstantCurveEditor();
    bool GetPropertyCtrlValueFromControl( wxVariant& variant, wxPGProperty* property, wxWindow* ctrl );
    bool GetValueFromControl( wxVariant& variant, wxPGProperty* property, wxWindow* ctrl ) const;
    virtual void DrawValue (wxDC &dc, const wxRect &rect, wxPGProperty *property, const wxString &text) const;
    virtual wxPGWindowList CreateControls( wxPropertyGrid* propGrid, wxPGProperty* property, const wxPoint& pos, const wxSize& sz ) const;
private:

};



#endif