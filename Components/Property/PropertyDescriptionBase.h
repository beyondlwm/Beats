#ifndef BEATS_COMPONENTS_PROPERTY_PROPERTYDESCRIPTIONBASE_H__INCLUDE
#define BEATS_COMPONENTS_PROPERTY_PROPERTYDESCRIPTIONBASE_H__INCLUDE

#include "PropertyPublic.h"
#include "../expdef.h"

class CComponentEditorProxy;
class CSerializer;
class TiXmlElement;
template<typename T>
class SharePtr;
enum EValueType
{
    eVT_DefaultValue, //Value comes from code serialization.
    eVT_SavedValue,      //Value comes from current saved data.
    eVT_CurrentValue, //Value comes from editor view.

    eVT_Count,
    eVT_Force32Bit = 0xFFFFFFFF
};

class COMPONENTS_DLL_DECL CPropertyDescriptionBase
{
public:
    CPropertyDescriptionBase(EPropertyType type);
    CPropertyDescriptionBase(const CPropertyDescriptionBase& rRef);

    virtual ~CPropertyDescriptionBase();

    EPropertyType GetType() const;
    void SetType(EPropertyType type);

    CComponentEditorProxy* GetOwner() const;
    void SetOwner(CComponentEditorProxy* pOwner);

    CPropertyDescriptionBase* GetParent() const;
    void SetParent(CPropertyDescriptionBase* pParent);

    void SetBasicInfo(const SBasicPropertyInfo& info);
    SBasicPropertyInfo& GetBasicInfo() const;

    bool Deserialize(CSerializer& serializer);

    virtual CPropertyDescriptionBase* AddChild(CPropertyDescriptionBase* pProperty);
    virtual bool DeleteChild(CPropertyDescriptionBase* pProperty, bool bKeepChildOrder = false);
    virtual void DeleteAllChild();
    CPropertyDescriptionBase* GetChild(size_t i) const;
    std::vector<CPropertyDescriptionBase*>& GetChildren();
    size_t GetChildrenCount() const;
    void Save();
    template<typename T>
    void InitializeValue(const T& value)
    {
        for (size_t i = 0; i < eVT_Count; ++i)
        {
            if (m_valueArray[i] == NULL)
            {
                m_valueArray[i] = new T(value);
            }
            else
            {
                (*(T*)m_valueArray[i]) = value;
            }
        }
    }
    template<typename T>
    void DestoryValue()
    {
        for (size_t i = 0; i < eVT_Count; ++i)
        {
            T* pValue = (T*)m_valueArray[i];
            BEATS_SAFE_DELETE(pValue);
        }
    }

    void* GetValue(EValueType type) const;

    virtual void Initialize();
    virtual bool IsContainerProperty();

    virtual CPropertyDescriptionBase* Clone(bool bCloneValue);
    virtual CPropertyDescriptionBase* CreateNewInstance() = 0;

    virtual void GetValueAsChar(EValueType type, char* pOut) = 0;
    virtual bool GetValueByTChar(const TCHAR* pIn, void* pOutValue) = 0;
    virtual void SetValue(void* pDefaultValue, EValueType type) = 0;

    // It's very important to know what this function mean.
    // Compare current data(property value in your property grid) to Default Value(which is determined in code of macro DECLARE_PROPERTY) and
    // XML data(the data you saved last time).
    // Notice: Default Value stores in wxPGProperty's defaultValue; current value stores in wxPGProperty's value; XML value stores in m_refVariableAddr.
    virtual bool IsDataSame(bool bWithDefaultOrXML) = 0;

    // This function will be called when Serialize from macro to data.
    virtual void AnalyseUIParameter(const TCHAR* parameter) = 0;

    virtual void SaveToXML(TiXmlElement* pParentNode) = 0;
    virtual void LoadFromXML(TiXmlElement* pNode) = 0;
    virtual void Serialize(CSerializer& serializer) = 0;

protected:
    bool DeserializeBasicInfo(CSerializer& serializer);

protected:
    EPropertyType m_type;
    CComponentEditorProxy* m_pOwner;
    SharePtr<SBasicPropertyInfo>* m_pBasicInfo;
    void* m_valueArray[eVT_Count];
    CPropertyDescriptionBase* m_pParent;
    std::vector<CPropertyDescriptionBase*>* m_pChildren;
};

#endif