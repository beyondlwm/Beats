#ifndef BEATS_COMPONENTS_PROPERTY_PROPERTYPUBLIC_H__INCLUDE
#define BEATS_COMPONENTS_PROPERTY_PROPERTYPUBLIC_H__INCLUDE

enum EReflectPropertyType
{
    eRPT_Invalid,
    eRPT_Bool,
    eRPT_Float,
    eRPT_Double,
    eRPT_Int,
    eRPT_UInt,
    eRPT_Str,
    eRPT_GUID,
    eRPT_List,
    eRPT_Map,
    eRPT_Ptr,
    eRPT_Color,
    eRPT_File,
    eRPT_Enum,

    eRPT_Count,
    eRPT_ForceTo32Bit = 0xFFFFFFFF

};

const static TCHAR* szPropertyTypeStr[] =
{
    _T("ePT_Invalid"),
    _T("ePT_Bool"),
    _T("ePT_Float"),
    _T("ePT_Double"),
    _T("ePT_Int"),
    _T("ePT_UInt"),
    _T("ePT_Str"),
    _T("ePT_GUID"),
    _T("ePT_List"),
    _T("ePT_Map"),
    _T("ePT_Ptr"),
    _T("ePT_Color"),
    _T("ePT_File"),
    _T("ePT_Enum"),

    _T("ePT_Count"),
    _T("ePT_ForceTo32Bit")
};

struct SBasicPropertyInfo
{
    SBasicPropertyInfo()
        : m_bEditable(true)
        , m_color(0xFFFFFFFF)
    {
    }
    ~SBasicPropertyInfo()
    {

    }
    SBasicPropertyInfo& operator = (const SBasicPropertyInfo& rRef)
    {
        m_bEditable = rRef.m_bEditable;
        m_color = rRef.m_color;
        m_displayName = rRef.m_displayName;
        m_catalog = rRef.m_catalog;
        m_tip = rRef.m_tip;
        m_variableName = rRef.m_variableName;
        return *this;
    }
    bool m_bEditable;
    size_t m_color;
    TString m_displayName;
    TString m_catalog;
    TString m_tip;
    TString m_variableName; // This member is a key of matching data structure with existing data.
};

enum EUIParameterAttrType
{
    eUIPAT_DefaultValue,
    eUIPAT_MinValue,
    eUIPAT_MaxValue,
    eUIPAT_SpinStep,
    eUIPAT_MaxCount,
    eUIPAT_FixCount,
    eUIPAT_VisibleWhen,
    eUIPAT_EnumStringArray,
    eUIPAT_Count,
    eUIPAT_Force32Bit = 0xffffffff
};

static const TCHAR* UIParameterAttrStr[] = 
{
    _T("DefaultValue"),
    _T("MinValue"),
    _T("MaxValue"),
    _T("SpinStep"),
    _T("MaxCount"),
    _T("FixCount"),
    _T("VisibleWhen"),
    _T("EnumUIString"),
};

#endif