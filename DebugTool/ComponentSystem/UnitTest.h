#ifndef BEATS_COMPONENTS_UNITTEST_H__INCLUDE
#define BEATS_COMPONENTS_UNITTEST_H__INCLUDE
#include "../../Utility/EnumStrGenerator/EnumStrGenerator.h"
#include "../../Utility/Serializer/Serializer.h"

#include "../../Components/Component/ComponentBase.h"
#include "../../Components/Component/ComponentManager.h"
#include "wxWidgetsProperty/wxWidgetsPropertyBase.h"
#include "wxWidgetsProperty/BoolPropertyDescription.h"
#include "wxWidgetsProperty/FloatPropertyDescription.h"
#include "wxWidgetsProperty/IntPropertyDescription.h"
#include "wxWidgetsProperty/ListPropertyDescriptionEx.h"
#include "wxWidgetsProperty/StringPropertyDescription.h"
#include "wxWidgetsProperty/UIntPropertyDescription.h"
#include "wxWidgetsProperty/PtrPropertyDescription.h"
#include "wxWidgetsProperty/EnumPropertyDescription.h"
#include "wxWidgetsProperty/DoublePropertyDescription.h"
#include "wxWidgetsProperty/MapPropertyDescription.h"

REGISTER_PROPERTY(bool, ePT_Bool);
REGISTER_PROPERTY(float, ePT_Float);
REGISTER_PROPERTY(double, ePT_Double);
REGISTER_PROPERTY(int, ePT_Int);
REGISTER_PROPERTY(size_t, ePT_UInt);
REGISTER_PROPERTY(TString, ePT_Str);
REGISTER_PROPERTY(CComponentBase*, ePT_Ptr);
REGISTER_PROPERTY_TEMPLATE1(std::vector, ePT_List);
REGISTER_PROPERTY_TEMPLATE2(std::map, ePT_Map);

REGISTER_PROPERTY_DESC(ePT_Bool, CBoolPropertyDescription)
REGISTER_PROPERTY_DESC(ePT_Float, CFloatPropertyDescription)
REGISTER_PROPERTY_DESC(ePT_Double, CDoublePropertyDescription)
REGISTER_PROPERTY_DESC(ePT_Int, CIntPropertyDescription)
REGISTER_PROPERTY_DESC(ePT_Str, CStringPropertyDescription)
REGISTER_PROPERTY_DESC(ePT_UInt, CUIntPropertyDescription)
REGISTER_PROPERTY_DESC(ePT_Ptr, CPtrPropertyDescription)
REGISTER_PROPERTY_DESC(ePT_List, CListPropertyDescriptionEx)
REGISTER_PROPERTY_DESC(ePT_Enum, CEnumPropertyDescription)
REGISTER_PROPERTY_DESC(ePT_Map, CMapPropertyDescription)

struct SLuancher
{
    SLuancher()
    {
        std::vector<TString> scanPath;
        scanPath.push_back(_T("E:/Work/Project/EraFable/SGame"));
        CEnumStrGenerator::GetInstance()->Init(scanPath);
    }
    ~SLuancher()
    {
        CEnumStrGenerator * pInstance= CEnumStrGenerator::GetInstance();
        if (pInstance !=NULL)
        {
            CEnumStrGenerator::Destroy();
        }
               
        CComponentManager::Destroy();
    }
}launchme;
enum ETestEnum
{
    eTE_A,
    eTE_B,
    eTE_C,
    eTE_D,
    eTE_Count,
    eTE_Force32Bit = 0xffffffff
};

enum ETestEnum2
{
    eTE2_A,
    eTE2_B,
    eTE2_C= 9,
    eTE2_Count,
    eTE2_Force32Bit = 0xffffffff

};

static const TCHAR* TestEnumStr[] = 
{
    _T("A"),
    _T("B"),
    _T("C"),
    _T("D"),
    _T("个数"),
    _T("强制"),
};

static const TCHAR* TestEnum2Str[] = 
{
    _T("第一个"),
    _T("第二个"),
    _T("第三个"),
    _T("个数"),
    _T("强制"),
};

class CTestComponent : public CComponentBase
{
public:
    CTestComponent();
    virtual ~CTestComponent();

    virtual void CTestComponent::ReflectData(CSerializer& serializer) override;

    DECLARE_REFLECT_GUID(CTestComponent, 0x7391ADF1, CComponentBase)

private:
    bool m_boolValue;
    float m_floatValue;
    double m_doubleValue;
    int m_intValue;
    size_t m_uintValue;
    TString m_strValue;
    std::vector<int> m_listValue;
    std::map<ETestEnum, CTestComponent*> m_mapValue;
    CTestComponent* m_ptrValue;
    ETestEnum m_enumValue;
    CTestComponent* m_pNextObj;
    CTestComponent* m_pNextObj1;
    CTestComponent* m_pNextObj2;
    std::vector<CTestComponent*> m_pObjList;
};

#endif