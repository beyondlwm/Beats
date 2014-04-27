#ifndef FCENGINEEDITOR_CONSTANTCURVEPROPERTY_H__INCLUDE
#define FCENGINEEDITOR_CONSTANTCURVEPROPERTY_H__INCLUDE


#include "SingleTypeChoicePropertyBase.h"

class ConstantCurveProperty : public SingleTypeChoicePropertyBase
{
public:
    enum
    {
        Constant_Type = 0,
        Curve_Type,
        ConstantRandom_Type,
        CurveRandom_Type
    };

    ConstantCurveProperty(const wxString& label,
        const wxString& name = wxPG_LABEL,
        double value = 0.0);
    virtual ~ConstantCurveProperty();

    virtual wxSize OnMeasureImage( int item = 0 ) const;
    virtual void OnCustomPaint(wxDC& dc, const wxRect& rect, wxPGPaintData& paintdata);

    void    SetValueImageSizeX(int iSizeX);
    void    SetCurveBitmap(wxBitmap& bitmap);
    wxBitmap* GetCurveBitmap() const;
    void    SetValueType(int iType);
    int     GetValueType() const;
    void    SetConstant1(double Value1 = 0.0);
    void    SetConstant2(double Value2 = 0.0);
    double  GetConstant1() const;
    double  GetConstant2() const;
    void    CreatConstantProperty();
    void    ReleaseConstantProperty();
    wxPGProperty*   GetConstantProperty1() const;
    wxPGProperty*   GetConstantProperty2() const;
    void    HideProperty(bool bHide1 = true, bool bHide2 = true);
    void    AppendConstantProperty();
    virtual bool StringToValue( wxVariant& variant, const wxString& text, int argFlags = 0 ) const;

private:
    bool        m_bChildAppended;
    double      m_ConstantValue_1;
    double      m_ConstantValue_2;
    wxPGProperty* m_pConstantProperty1;
    wxPGProperty* m_pConstantProperty2;
    wxBitmap*   m_pCurveBitmap; 
    wxImage*    m_pImage; 
};

#endif