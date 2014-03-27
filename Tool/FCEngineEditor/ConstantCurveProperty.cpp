
#include "stdafx.h"
#include "ConstantCurveProperty.h"
#define X_UNNULL_SIZE wxSize(1,0);

ConstantCurveProperty::ConstantCurveProperty(const wxString& label, const wxString& name, double value)
    : SingleTypeChoicePropertyBase(label, name, value), m_ConstantValue_1(0.0), m_ConstantValue_2(0.0)
    , m_pConstantProperty1(NULL)
    , m_pConstantProperty2(NULL)
    , m_bChildAppended(false)
    , m_pCurveBitmap(NULL)
{
    CreatConstantProperty();
}

ConstantCurveProperty::~ConstantCurveProperty()
{
    BEATS_SAFE_DELETE (m_pCurveBitmap);
}

wxSize ConstantCurveProperty::OnMeasureImage( int /*item*/ ) const
{   
    return X_UNNULL_SIZE;
}

void ConstantCurveProperty::SetCurveBitmap(wxBitmap& bitmap)
{
    BEATS_SAFE_DELETE (m_pCurveBitmap);
    
    m_pCurveBitmap = new wxBitmap(bitmap);
}

wxBitmap* ConstantCurveProperty::GetCurveBitmap() const
{
    return m_pCurveBitmap;
}

void ConstantCurveProperty::SetValueType(int iType)
{
    m_iType = iType;
}

int ConstantCurveProperty::GetValueType() const
{
    return m_iType;
}

void ConstantCurveProperty::SetConstant1(double Value1)
{
    m_ConstantValue_1 = Value1;
}

void ConstantCurveProperty::SetConstant2(double Value2)
{
    m_ConstantValue_2 = Value2;
}

double ConstantCurveProperty::GetConstant1() const
{
    return m_ConstantValue_1;
}

double ConstantCurveProperty::GetConstant2() const
{
    return m_ConstantValue_2;
}

wxPGProperty* ConstantCurveProperty::GetConstantProperty1() const
{
    return m_pConstantProperty1;
}
wxPGProperty* ConstantCurveProperty::GetConstantProperty2() const
{
    return m_pConstantProperty2;
}

void ConstantCurveProperty::CreatConstantProperty()
{
    m_pConstantProperty1 = new wxFloatProperty( wxT("Constant1"), wxPG_LABEL, m_ConstantValue_1);
    m_pConstantProperty2 = new wxFloatProperty( wxT("Constant2"), wxPG_LABEL, m_ConstantValue_2);
    AppendConstantProperty();
    HideProperty();
}

void ConstantCurveProperty::ReleaseConstantProperty()
{
    GetGrid()->DeleteProperty(m_pConstantProperty1);
    GetGrid()->DeleteProperty(m_pConstantProperty2);
}

void ConstantCurveProperty::HideProperty(bool bHide1,bool bHide2)
{
    m_pConstantProperty1->Hide(bHide1);
    m_pConstantProperty2->Hide(bHide2);
}

void ConstantCurveProperty::AppendConstantProperty()
{
    if (!m_bChildAppended)
    {
        AppendChild(m_pConstantProperty1);
        AppendChild(m_pConstantProperty2);
        m_bChildAppended = true;
    }
}

bool ConstantCurveProperty::StringToValue( wxVariant& variant, const wxString& text, int argFlags ) const
{
    return wxFloatProperty::StringToValue( variant, text, argFlags);
}

void ConstantCurveProperty::OnCustomPaint(wxDC& dc, const wxRect& rect, wxPGPaintData& /*paintdata*/)
{
    if(GetValueType() == ConstantCurveProperty::Curve_Type)
    {
        wxBitmap* pBmp = GetCurveBitmap();
        if (pBmp != NULL)
        {   
            wxBitmap* bmpNew = NULL;
            if (pBmp->GetWidth() != rect.x)
            {
                wxSize maxSz = GetGrid()->GetImageSize();
                wxSize imSz(pBmp->GetWidth(),pBmp->GetHeight());
                wxImage img = pBmp->ConvertToImage();
                img.Rescale(m_iImageSizeX, maxSz.y, wxIMAGE_QUALITY_HIGH);
                bmpNew = new wxBitmap(img, DEFAULT_DEPTH);
            }
            wxCHECK_RET( pBmp && pBmp->IsOk(), wxT("invalid bitmap") );
            wxCHECK_RET( rect.x >= 0, wxT("unexpected measure call") );
            dc.DrawBitmap(*bmpNew, rect.x - DEFAULT_IMAGE_OFFSET_INCREMENT, rect.y);
            BEATS_SAFE_DELETE(bmpNew);
        }
    }
    else if(GetValueType() == ConstantCurveProperty::ConstantRandom_Type)
    {
        
    }
}

void ConstantCurveProperty::SetValueImageSizeX(int iSizeX)
{
    m_iImageSizeX = iSizeX;
}