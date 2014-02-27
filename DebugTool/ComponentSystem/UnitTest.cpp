#include "stdafx.h"
#include "UnitTest.h"
#include "../../Components/ComponentPublic.h"
#include "../../Components/DependencyDescription.h"
//START_REGISTER_COMPONET
//REGISTER_COMOPNENT(CTestComponent, _T("TestComponents"), _T("Test\\Components"))
//END_REGISTER_COMPONET
CTestComponent::CTestComponent(CSerializer& serializer)
: m_boolValue(false)
, m_floatValue(0)
, m_doubleValue(0)
, m_intValue(0)
, m_uintValue(0)
, m_ptrValue(NULL)
, m_enumValue(eTE_A)
{
    DECLARE_PROPERTY(serializer, m_boolValue, true, 0xff0000ff, _T("是否自动"), NULL, _T("I'm a test bool tip"), _T("DefaultValue:true"));
    DECLARE_PROPERTY(serializer, m_floatValue, true, 0xffffffff, _T("生命值"), _T(""), _T(""), _T("DefaultValue: 7.5, SpinStep: 0.7"));
    DECLARE_PROPERTY(serializer, m_doubleValue, true, 0xffffffff, NULL, NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_intValue, true, 0xffffffff, _T("个数"), NULL, NULL, _T("DefaultValue: 99, SpinStep: 7"));
    DECLARE_PROPERTY(serializer, m_uintValue, true, 0xffffffff, NULL, NULL, NULL, _T("SpinStep: 7"));
    DECLARE_PROPERTY(serializer, m_strValue, true, 0xffffffff, NULL, NULL, NULL, _T("DefaultValue: hastalavista"));
    DECLARE_PROPERTY(serializer, m_ptrValue, true, 0xffffffff, _T("其他对象"), NULL, NULL, NULL);
    DECLARE_PROPERTY(serializer, m_listValue, true, 0xffffffff, _T("ListValue"), NULL, NULL, NULL);
    //DECLARE_PROPERTY_ENUM(ETestEnum, m_enumValue, true, 0xffffffff, _T("EnumValue"), NULL, NULL, eTE_Count, NULL, NULL);
    DECLARE_DEPENDENCY(serializer, m_pNextObj, CTestComponent, _T("目标对象"), eDT_Weak);
    DECLARE_DEPENDENCY(serializer, m_pNextObj1, CTestComponent, _T("目标对象2"), eDT_Weak);
    DECLARE_DEPENDENCY(serializer, m_pNextObj2, CTestComponent, _T("目标对象3"), eDT_Weak);
}

CTestComponent::~CTestComponent()
{

}
