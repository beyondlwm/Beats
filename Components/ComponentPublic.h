#ifndef BEATS_COMPONENTS_COMPONENTPUBLIC_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENTPUBLIC_H__INCLUDE

#include <string>
#include <algorithm>
#include "SharePtr/SharePtr.h"
#include "Property/PropertyPublic.h"
#include "Utility/StringHelper/StringHelper.h"
#include "Component/ComponentInstance.h"
#include "Component/ComponentProxy.h"
#include "Component/ComponentProxyManager.h"
#include "Component/ComponentInstanceManager.h"
#include "Property/PropertyDescriptionBase.h"
#include "Utility/Serializer/Serializer.h"

// To comment or un-comment this macro to decide serializer/deserialize.
//#define EXPORT_TO_EDITOR
#define EDITOR_MODE

#define ENGINE_COMPONENT_INFO_FILE _T("EngineLaunch.eds")
#define ENGINE_COMPONENT_INFO_PATCH _T("EnginePatch.XML")
#define GAME_COMPONENT_INFO_FILE _T("GameLaunch.eds")
#define GAME_COMPONENT_INFO_PATCH _T("GamePatch.XML")
#define ENGINE_ENUM_INFO_FILE _T("EngineScanData.bin")

#define COMPONENT_FILE_HEADERSTR _T("ComponentFile")
#define COMPONENT_FILE_EXTENSION _T(".bcf")
#define COMPONENT_FILE_EXTENSION_FILTER _T("Beats Component File(*.bcf)\0*.bcf\0\0")
#define COMPONENT_PROJECT_EXTENSION _T("Beats Component Project File(*.bcp)\0*.bcp\0\0")
#define BINARIZE_FILE_NAME _T("AIData.bin")
#define BINARIZE_FILE_EXTENSION _T(".bin")
#define BINARIZE_FILE_EXTENSION_FILTER _T("Beats binaray File(*.bin)\0*.bin\0\0")
#define PROPERTY_PARAM_SPLIT_STR _T(",")
#define PROPERTY_KEYWORD_SPLIT_STR _T(":")

#define COMPONENT_SYSTEM_VERSION 4

struct SSerilaizerExtraInfo
{
    SSerilaizerExtraInfo()
        : m_uPropertyCount(0)
        , m_uDependencyCount(0)
    {

    }
    uint32_t m_uPropertyCount;
    uint32_t m_uDependencyCount;
};

class CSerializer;

template<typename T>
inline EReflectPropertyType GetEnumType(T& value, CSerializer* pSerializer)
{
    EReflectPropertyType eRet = eRPT_Invalid;
    const char* pszTypeName = typeid(value).name();
    bool bIsEnum = memcmp(pszTypeName, "enum ", strlen("enum ")) == 0;
    if (bIsEnum)
    {
        eRet = eRPT_Enum;
        (*pSerializer) << (uint32_t)eRPT_Enum;
        (*pSerializer) << (int)(value) << &pszTypeName[strlen("enum ")];
    }
    BEATS_ASSERT(bIsEnum, _T("Unknown type!"));
    return eRet;
}

template<typename T>
inline void SerializeVariable(const T& value, CSerializer* pSerializer)
{
    *pSerializer << value;
}

template<typename T>
inline void SerializeVariable(const std::vector<T>& value, CSerializer* pSerializer)
{
    *pSerializer << value.size();
    for (auto iter = value.begin(); iter != value.end(); ++iter)
    {
        *pSerializer << (*iter);
    }
}

template<typename T1, typename T2>
inline void SerializeVariable(const std::map<T1, T2>& value, CSerializer* pSerializer)
{
    *pSerializer << value.size();
    for (auto iter = value.begin(); iter != value.end(); ++iter)
    {
        SerializeVariable(iter->first, pSerializer);
        SerializeVariable(iter->second, pSerializer);
    }
}

template<typename T>
inline void DeserializeVariable(T& value, CSerializer* pSerializer, CComponentInstance* /*pOwner*/)
{
    *pSerializer >> value;
}

struct SReflectSoundFilePath
{
    TString m_value;
};

struct SReflectFilePath
{
    TString m_value;
};

template<>
inline void DeserializeVariable(SReflectFilePath& value, CSerializer* pSerializer, CComponentInstance* /*pOwner*/)
{
    *pSerializer >> value.m_value;
}

template<typename T>
inline void DeserializeVariable(T*& value, CSerializer* pSerializer, CComponentInstance* pOwner)
{
    bool bHasInstance = false;
    *pSerializer >> bHasInstance;
    bool bCloneAble = false;
    *pSerializer >> bCloneAble;
    if (bHasInstance)
    {
        uint32_t uStartPos = pSerializer->GetReadPos();
#ifdef EDITOR_MODE
        // If we are calling CComponentProxy::UpdateHostComponent, it means all ptr value are ready, it is CPtrPropertyDescription::GetInstanceComponent()->GetHostComponent.
        // Which the value will be pack in CPtrPropertyDescription::Serialize.
        CComponentProxy* pCurrUpdateProxy = CComponentProxyManager::GetInstance()->GetCurrUpdateProxy();
        CPropertyDescriptionBase* pCurrReflectProperty = CComponentProxyManager::GetInstance()->GetCurrReflectProperty();
        if (pCurrUpdateProxy != nullptr || pCurrReflectProperty != nullptr)
        {
            uint32_t uHostComponentAddress = 0;
            (*pSerializer) >> uHostComponentAddress;
            value = (T*)(uHostComponentAddress);
        }
        else
        {
#endif
            uint32_t uDataSize, uGuid, uId;
            *pSerializer >> uDataSize;
            *pSerializer >> uGuid;
            *pSerializer >> uId;
            BEATS_ASSERT(value == NULL, _T("A pointer should be initialize to NULL before it is deserialized!"));
            if (value == NULL)
            {
                // If this ptr property is not allowed to clone and we are in clone phase, ignore it.
                if (!bCloneAble && CComponentInstanceManager::GetInstance()->IsInClonePhase())
                {
                    pSerializer->SetReadPos(uStartPos + uDataSize);
                }
                else
                {
#ifdef EDITOR_MODE
                    BEATS_ASSERT(CComponentInstanceManager::GetInstance()->IsInClonePhase());
#endif
                    value = dynamic_cast<T*>(CComponentInstanceManager::GetInstance()->CreateComponent(uGuid, false, true, 0xFFFFFFFF, false, pSerializer, false));
                    BEATS_ASSERT(pOwner != nullptr);
                    pOwner->RegisterReflectComponent(value);
                    if (CComponentInstanceManager::GetInstance()->GetFileSerializer() == pSerializer)
                    {
                        value->SetDataPos(uStartPos);
                        value->SetDataSize(uDataSize);
                    }
                    BEATS_ASSERT(uStartPos + uDataSize == pSerializer->GetReadPos(),
                        _T("Component Data Not Match!\nGot an error when Deserialize a pointer of component 0x%x %s instance id %d\nRequired size: %d, Actual size: %d"), uGuid, value->GetClassStr(), uId, uDataSize, pSerializer->GetReadPos() - uStartPos);
                }
            }
#ifdef EDITOR_MODE
        }
#endif
    }
#ifdef EDITOR_MODE
    else
    {
        value = NULL;
    }
#endif
}

template<typename T>
inline void DeserializeVariable(std::vector<T>& value, CSerializer* pSerializer, CComponentInstance* pOwner)
{
#ifdef EDITOR_MODE
    EReflectOperationType operateType;
    CPropertyDescriptionBase* pCurrReflectProperty = CComponentProxyManager::GetInstance()->GetCurrReflectProperty(&operateType);
    if (pCurrReflectProperty != nullptr)
    {
        if (operateType == EReflectOperationType::ChangeValue)
        {
            uint32_t uElementIndex = 0;
            *pSerializer >> uElementIndex;
            BEATS_ASSERT(uElementIndex < value.size());
            DeserializeVariable(value[uElementIndex], pSerializer, pOwner);
        }
        else if (operateType == EReflectOperationType::AddChild)
        {
            if (pSerializer->GetReadPos() == (uint32_t)pSerializer->GetUserData())
            {
                uint32_t uData = 0;
                *pSerializer >> uData;
                auto iter = value.begin();
                std::advance(iter, uData);
                T tmpData;
                value.insert(iter, tmpData);
                CComponentProxyManager::GetInstance()->SetCurrReflectProperty(nullptr, EReflectOperationType::ChangeValue);
                DeserializeVariable(value[uData], pSerializer, pOwner);
                CComponentProxyManager::GetInstance()->SetCurrReflectProperty(pCurrReflectProperty, EReflectOperationType::AddChild);
            }
            else
            {
                uint32_t uData = 0;
                *pSerializer >> uData;
                BEATS_ASSERT(uData < value.size());
                DeserializeVariable(value[uData], pSerializer, pOwner);
            }
        }
        else if (operateType == EReflectOperationType::RemoveChild)
        {
            if (pSerializer->GetReadPos() == (uint32_t)pSerializer->GetUserData())
            {
                bool bRemoveAll = false;
                *pSerializer >> bRemoveAll;
                if (!bRemoveAll)
                {
                    uint32_t uRemovePos = 0;
                    *pSerializer >> uRemovePos;
                    auto iter = value.begin();
                    std::advance(iter, uRemovePos);
                    value.erase(iter);
                }
                else
                {
                    value.clear();
                }
            }
            else
            {
                uint32_t uElementIndex = 0;
                *pSerializer >> uElementIndex;
                BEATS_ASSERT(uElementIndex < value.size());
                DeserializeVariable(value[uElementIndex], pSerializer, pOwner);
            }
        }
    }
    else
    {
#endif
        uint32_t childCount = 0;
        *pSerializer >> childCount;
#ifdef EDITOR_MODE
        value.clear();
#else
        BEATS_ASSERT(value.size() == 0, _T("vector should be empty when deserialize!"));
#endif
        value.resize(childCount);
        for (uint32_t i = 0; i < childCount; ++i)
        {
            BEATS_ASSERT(typeid(T) != typeid(bool), _T("std::vector<bool> is not supported! it's a very specific version of stl, it's not a container!"));
            DeserializeVariable(value[i], pSerializer, pOwner);
        }
#ifdef EDITOR_MODE
    }
#endif
}

template<typename T1, typename T2>
inline void DeserializeVariable(std::map<T1, T2>& value, CSerializer* pSerializer, CComponentInstance* pOwner)
{
#ifdef EDITOR_MODE
    EReflectOperationType operateType;
    CPropertyDescriptionBase* pCurrReflectProperty = CComponentProxyManager::GetInstance()->GetCurrReflectProperty(&operateType);
    if (pCurrReflectProperty != nullptr)
    {
        if (operateType == EReflectOperationType::ChangeValue)
        {
            T1 key;
            DeserializeVariable(key, pSerializer, pOwner);
            BEATS_ASSERT(value.find(key) != value.end());
            DeserializeVariable(value[key], pSerializer, pOwner);
        }
        else if (operateType == EReflectOperationType::AddChild)
        {
            T1 key;
            DeserializeVariable(key, pSerializer, pOwner);
            if (pSerializer->GetReadPos() == (uint32_t)pSerializer->GetUserData())
            {
                CComponentProxyManager::GetInstance()->SetCurrReflectProperty(nullptr, EReflectOperationType::ChangeValue);
                T2 mapValue;
                DeserializeVariable(mapValue, pSerializer, pOwner);
                CComponentProxyManager::GetInstance()->SetCurrReflectProperty(pCurrReflectProperty, EReflectOperationType::AddChild);
                BEATS_ASSERT(value.find(key) == value.end());
                value[key] = mapValue;
            }
            else
            {
                BEATS_ASSERT(value.find(key) != value.end());
                DeserializeVariable(value[key], pSerializer, pOwner);
            }
        }
        else if (operateType == EReflectOperationType::RemoveChild)
        {
            if (pSerializer->GetReadPos() == (uint32_t)pSerializer->GetUserData())
            {
                bool bRemoveAll = false;
                *pSerializer >> bRemoveAll;
                if (!bRemoveAll)
                {
                    T1 key;
                    DeserializeVariable(key, pSerializer, pOwner);
                    BEATS_ASSERT(value.find(key) != value.end());
                    value.erase(key);
                }
                else
                {
                    value.clear();
                }
            }
            else
            {
                T1 key;
                DeserializeVariable(key, pSerializer, pOwner);
                BEATS_ASSERT(value.find(key) != value.end());
                DeserializeVariable(value[key], pSerializer, pOwner);
            }
        }
    }
    else
    {
#endif
        EReflectPropertyType keyType;
        EReflectPropertyType valueType;
        uint32_t childCount = 0;
        *pSerializer >> keyType;
        *pSerializer >> valueType;
        *pSerializer >> childCount;
#ifdef EDITOR_MODE
        value.clear();
#else
        BEATS_ASSERT(value.size() == 0, _T("map should be empty when deserialize!"));
#endif
        for (uint32_t i = 0; i < childCount; ++i)
        {
            T1 key = T1();
            DeserializeVariable(key, pSerializer, pOwner);
            T2 myValue = T2();
            DeserializeVariable(myValue, pSerializer, pOwner);
            BEATS_ASSERT(value.find(key) == value.end() || CComponentProxyManager::GetInstance()->IsLoadingFile(), _T("A map can't have two same key value!"));
            value[key] = myValue;
        }

#ifdef EDITOR_MODE
    }
#endif
}

template<>
inline EReflectPropertyType GetEnumType(SReflectFilePath& value, CSerializer* pSerializer)
{
    *pSerializer << eRPT_File;
    *pSerializer << value.m_value;
    return eRPT_File;
}

template<typename T>
inline EReflectPropertyType GetEnumType(T*& /*value*/, CSerializer* pSerializer)
{
    uint32_t guid = T::REFLECT_GUID;
    T* pTestParam = (T*)(guid);
    EReflectPropertyType eReturnType = eRPT_Invalid;
    CComponentBase* pReflect = dynamic_cast<CComponentBase*>(pTestParam);
    if (pReflect != NULL)
    {
        eReturnType = eRPT_Ptr;
        *pSerializer << (int)eRPT_Ptr;
        *pSerializer << T::REFLECT_GUID;
    }
    BEATS_ASSERT(eReturnType != eRPT_Invalid, _T("Unknown type!"));
    return eReturnType;
}

#define REGISTER_PROPERTY(classType, enumType)\
template<>\
inline EReflectPropertyType GetEnumType(classType& value, CSerializer* pSerializer)\
{\
    if (pSerializer != NULL)\
    {\
        *pSerializer << ((uint32_t)enumType);\
        *pSerializer << value;\
    }\
    return enumType;\
}

#define REGISTER_PROPERTY_SHAREPTR(classType, enumType)\
    template<typename T>\
    inline EReflectPropertyType GetEnumType(classType<T>& /*value*/, CSerializer* pSerializer)\
{\
    if (pSerializer != NULL)\
    {\
        T* tmp = NULL;\
        GetEnumType(tmp, pSerializer);\
    }\
    return enumType;\
}

#define REGISTER_PROPERTY_TEMPLATE1(classType, enumType)\
    template<typename T>\
    inline EReflectPropertyType GetEnumType(classType<T>& /*value*/, CSerializer* pSerializer)\
    {\
        if (pSerializer != NULL)\
        {\
            *pSerializer << (uint32_t)enumType;\
            T tmp = T();\
            GetEnumType(tmp, pSerializer);\
        }\
        return enumType;\
    }

#define REGISTER_PROPERTY_TEMPLATE2(classType, enumType)\
    template<typename T1, typename T2>\
    inline EReflectPropertyType GetEnumType(classType<T1, T2>& /*value*/, CSerializer* pSerializer)\
    {\
        if (pSerializer != NULL)\
        {\
            *pSerializer << (uint32_t)enumType;\
            T1 tmp1 = T1();\
            GetEnumType(tmp1, pSerializer);\
            T2 tmp2 = T2();\
            GetEnumType(tmp2, pSerializer);\
        }\
        return enumType;\
    }

#define REGISTER_PROPERTY_DESC(enumType, propertyDescriptionType)\
    CPropertyDescriptionBase* CreateProperty_##propertyDescriptionType(CSerializer* serilaizer)\
    {\
        return new propertyDescriptionType(serilaizer);\
    }\
    struct SRegister_Creator_##enumType\
    {\
        SRegister_Creator_##enumType()\
        {\
            CComponentProxyManager::GetInstance()->RegisterPropertyCreator(enumType, CreateProperty_##propertyDescriptionType);\
        }\
    }enumType##_creator_launcher;

#ifdef EXPORT_TO_EDITOR

inline const TCHAR* GenEnumParamStr(const std::vector<TString>& enumStringArray, const TCHAR* pszParam = NULL)
{
    static TString strEnumParam;
    strEnumParam.clear();
    strEnumParam.append(UIParameterAttrStr[eUIPAT_EnumStringArray]).append(PROPERTY_KEYWORD_SPLIT_STR);
    uint32_t uCount = enumStringArray.size();
    BEATS_ASSERT(uCount > 0, _T("Enum string array is empty!"));
    for (uint32_t i = 0; i < uCount; ++i)
    {
        strEnumParam.append(enumStringArray[i]);
        if (i < uCount - 1)
        {
            strEnumParam.append(_T("@"));
        }
    }
    if (pszParam != NULL)
    {
        strEnumParam.append(PROPERTY_PARAM_SPLIT_STR).append(pszParam);
    }
    return strEnumParam.c_str();
}

inline const TCHAR* GenEnumParamStr(const TCHAR* enumStringArray[], const TCHAR* pszParam = NULL)
{
    static TString strEnumParam;
    strEnumParam.clear();
    strEnumParam.append(UIParameterAttrStr[eUIPAT_EnumStringArray]).append(PROPERTY_KEYWORD_SPLIT_STR);
    uint32_t uCount = sizeof(enumStringArray) / sizeof(TCHAR*);
    BEATS_ASSERT(uCount > 0, _T("Enum string array is empty!"));
    for (uint32_t i = 0; i < uCount; ++i)
    {
        strEnumParam.append(enumStringArray[i]);
        if (i < uCount - 1)
        {
            strEnumParam.append(_T("@"));
        }
    }
    if (pszParam != NULL)
    {
        strEnumParam.append(PROPERTY_PARAM_SPLIT_STR).append(pszParam);
    }
    return strEnumParam.c_str();
}

inline bool CheckIfEnumHasExported(const TString& strEnumName)
{
    static std::set<TString> exportRecord;

    bool bRet = exportRecord.find(strEnumName) == exportRecord.end();
    if (bRet)
    {
        exportRecord.insert(strEnumName);
    }
    return !bRet;
}

#define GEN_ENUM_PARAM(stringarray, propertyParam) GenEnumParamStr(stringarray, propertyParam)

template<typename T>
void DeclareProperty(CSerializer& serializer, T& property, const TCHAR* pszPropertyStr, bool bEditable, int color, const TCHAR* pszDisplayName, const TCHAR* pszCatalog, const TCHAR* pszTip, const TCHAR* pszParameter)
{
    BEATS_ASSERT(serializer.GetUserData() != NULL, _T("Serializer doesn't contain an user data!nPlease make sure you enalbe EDITOR_MODE and Disable EXPORT_TO_EDITOR when launch editor!"));
    serializer << (bool)true;
    EReflectPropertyType propertyType = GetEnumType(property, &serializer);
    uint32_t nPropertyDataSizeHolder = serializer.GetWritePos();
    serializer << nPropertyDataSizeHolder;
    const TCHAR* pszParam = pszParameter;
    if (propertyType == eRPT_Enum && pszParam != NULL)
    {
        uint32_t uEnumStringArrayNameLen = _tcslen(UIParameterAttrStr[eUIPAT_EnumStringArray]);
        if (_tcslen(pszParam) > uEnumStringArrayNameLen && memcmp(pszParam, UIParameterAttrStr[eUIPAT_EnumStringArray], uEnumStringArrayNameLen) == 0)
        {
            const char* pszTypeName = typeid(property).name();
            TString strEnumName = &pszTypeName[strlen("enum ")];
            if (CheckIfEnumHasExported(strEnumName))
            {
                pszParam = NULL;/*It's not necessary to export the same enum string array value, so ignore it!*/
            }
        }
    }
    serializer << (bool)bEditable << color << (pszDisplayName ? TO_UTF8(pszDisplayName) : pszPropertyStr) << (pszCatalog ? TO_UTF8(pszCatalog) : _T("")) << (pszTip ? TO_UTF8(pszTip) : _T("")) << pszPropertyStr << (pszParam ? pszParam : _T(""));
    uint32_t propertyDataSize = serializer.GetWritePos() - nPropertyDataSizeHolder;
    serializer.SetWritePos(nPropertyDataSizeHolder);
    serializer << propertyDataSize;
    serializer.SetWritePos(nPropertyDataSizeHolder + propertyDataSize);
    ++(((SSerilaizerExtraInfo*)(serializer.GetUserData()))->m_uPropertyCount);
}

#define DECLARE_PROPERTY(serializer, property, editable, color, displayName, catalog, tip, parameter)\
    DeclareProperty(serializer, property, _T(#property), editable, color, displayName, catalog, tip, parameter)

#define DECLARE_DEPENDENCY(serializer, ptrProperty, displayName, dependencyType)\
{\
    BEATS_ASSERT(serializer.GetUserData() != NULL, _T("Serializer doesn't contain an user data!\nPlease make sure you enalbe EDITOR_MODE and Disable EXPORT_TO_EDITOR when launch editor!"));\
    serializer << (bool)false << (bool)false << dependencyType << ptrProperty->REFLECT_GUID << TO_UTF8(displayName) << TO_UTF8(_T(#ptrProperty)); \
    ++(((SSerilaizerExtraInfo*)(serializer.GetUserData()))->m_uDependencyCount);\
}

#define DECLARE_DEPENDENCY_LIST(serializer, ptrProperty, displayName, dependencyType)\
{\
    BEATS_ASSERT(serializer.GetUserData() != NULL, _T("Serializer doesn't contain an user data!\nPlease make sure you enalbe EDITOR_MODE and Disable EXPORT_TO_EDITOR when launch editor!"));\
    ptrProperty.resize(1);\
    serializer << (bool)false << (bool)true << dependencyType << ptrProperty[0]->REFLECT_GUID << TO_UTF8(displayName) << TO_UTF8(_T(#ptrProperty)); \
    ptrProperty.resize(0);\
    ++(((SSerilaizerExtraInfo*)(serializer.GetUserData()))->m_uDependencyCount);\
}

#else

#ifndef EDITOR_MODE
    #define DECLARE_PROPERTY(serializer, property, editable, color, displayName, catalog, tip, parameter) DeserializeVariable(property, &serializer, this);

    #define DECLARE_DEPENDENCY(serializer, ptrProperty, displayName, dependencyType)\
    {\
        uint32_t uLineCount = 0;\
        serializer >> uLineCount;\
        BEATS_ASSERT(uLineCount <= 1, _T("Data error:\nWe want a dependency data, but got %d line count!"), uLineCount);\
        ptrProperty = NULL;\
        if (uLineCount == 1)\
        {\
            uint32_t uInstanceId, uGuid;\
            serializer >> uInstanceId >> uGuid;\
            CComponentInstanceManager::GetInstance()->AddDependencyResolver(NULL, 0 , uGuid, uInstanceId, &ptrProperty, false);\
        }\
    }

    #define DECLARE_DEPENDENCY_LIST(serializer, ptrProperty, displayName, dependencyType)\
    {\
        uint32_t uLineCount = 0;\
        serializer >> uLineCount;\
        ptrProperty.clear();\
        for (uint32_t i = 0; i < uLineCount; ++i)\
        {\
            uint32_t uInstanceId, uGuid;\
            serializer >> uInstanceId >> uGuid;\
            CComponentInstanceManager::GetInstance()->AddDependencyResolver(NULL, i , uGuid, uInstanceId, &ptrProperty, true);\
        }\
    }

#else

template<typename T>
bool DeclareProperty(CSerializer& serializer, CComponentInstance* pComponent, T& property, const TCHAR* pszPropertyStr)
{
    bool bStopHandle = false;
    bool bInClonePhase = CComponentInstanceManager::GetInstance()->IsInClonePhase();
    bool bInLoadingPhase = CComponentProxyManager::GetInstance()->IsLoadingFile();
    bool bNeedHandleSync = bInClonePhase || bInLoadingPhase;
    if (!bNeedHandleSync)
    {
        CPropertyDescriptionBase* pDescriptionBase = CComponentProxyManager::GetInstance()->GetCurrReflectProperty();
        BEATS_ASSERT(CComponentProxyManager::GetInstance()->GetCurrReflectDependency() != nullptr || pDescriptionBase != nullptr);
        if (pDescriptionBase != nullptr)
        {
            BEATS_ASSERT(CComponentProxyManager::GetInstance()->GetCurrReflectDependency() == nullptr);
            if (pDescriptionBase->GetBasicInfo()->m_variableName.compare(pszPropertyStr) == 0)
            {
                bNeedHandleSync = !pComponent->OnPropertyChange(&property, &serializer);
                bStopHandle = true;
            }
        }
    }
    if (bNeedHandleSync)
    {
        DeserializeVariable(property, &serializer, pComponent);
    }
    return bStopHandle;
}
#define DECLARE_PROPERTY(serializer, property, editable, color, displayName, catalog, tip, parameter) \
    {\
        if (serializer.GetWritePos() == serializer.GetReadPos() || DeclareProperty(serializer, this, property, _T(#property))) return;\
    }

template<typename T>
bool DeclareDependency(CSerializer& serializer, CComponentBase* pComponent, T& ptrProperty, const TCHAR* pszPropertyStr)
{
    bool bStopHandle = false;
    bool bInClonePhase = CComponentInstanceManager::GetInstance()->IsInClonePhase();
    // In clone phase, we don't need to resolve anything(avoid resolve the existing component), just copy all property description.
    if (bInClonePhase)
    {
        uint32_t uLineCount, uInstanceId, uGuid;
        serializer >> uLineCount;
        if (uLineCount > 0)
        {
            BEATS_ASSERT(uLineCount == 1, _T("Data error:We want a dependency data, but got %d line count!"), uLineCount);
            serializer >> uInstanceId >> uGuid;
        }
    }
    else
    {
        if (CComponentProxyManager::GetInstance()->GetCurrReflectProperty() == NULL)
        {
            CDependencyDescription* pDependency = CComponentProxyManager::GetInstance()->GetCurrReflectDependency();
            if (pDependency == NULL || _tcscmp(pDependency->GetVariableName(), pszPropertyStr) == 0)
            {
                uint32_t uLineCount = 0;
                serializer >> uLineCount;
                BEATS_ASSERT(uLineCount <= 1, _T("Data error:We want a dependency data, but got %d line count!"), uLineCount);
                if (uLineCount == 1)
                {
                    uint32_t uInstanceId, uGuid;
                    serializer >> uInstanceId >> uGuid;
                    bool bNeedSnyc = true;
                    if (pDependency != NULL && !CComponentProxyManager::GetInstance()->IsLoadingFile())
                    {
                        EDependencyChangeAction action;
                        CComponentProxy* pOperateProxy = NULL;
                        pDependency->GetCurrActionParam(action, pOperateProxy);
                        if (pOperateProxy->IsInitialized())
                        {
                            BEATS_ASSERT(CComponentInstanceManager::GetInstance()->GetComponentInstance(uInstanceId, uGuid) == pOperateProxy->GetHostComponent());
                            bNeedSnyc = !pComponent->OnDependencyChange(&ptrProperty, pOperateProxy->GetHostComponent());
                        }
                    }
                    if (bNeedSnyc)
                    {
                        ptrProperty = NULL;
                        uint32_t uDepGuid = ptrProperty->REFLECT_GUID;
                        bool bIsParent = CComponentProxyManager::GetInstance()->IsParent(uDepGuid, uGuid);
                        BEATS_ASSERT(bIsParent,
                            _T("GUID mismatched. GUID of dependency:0x%x, GUID in datafile:0x%x"),
                            uDepGuid, uGuid);
                        if (bIsParent)
                        {
                            CComponentInstanceManager::GetInstance()->AddDependencyResolver(NULL, 0, uGuid, uInstanceId, &ptrProperty, false);
                        }
                    }
                }
                else
                {
                    if (ptrProperty != NULL && !CComponentProxyManager::GetInstance()->IsLoadingFile())
                    {
                        pComponent->OnDependencyChange(&ptrProperty, NULL);
                    }
                    ptrProperty = NULL;
                }
                if (pDependency != NULL)
                {
                    bStopHandle = true;
                }
            }
        }
    }
    return bStopHandle;
}
#define DECLARE_DEPENDENCY(serializer, ptrProperty, displayName, dependencyType)\
    {\
        if (serializer.GetWritePos() == serializer.GetReadPos() || DeclareDependency(serializer, this, ptrProperty, _T(#ptrProperty))) return;\
    }

template<typename T>
bool DeclareDependencyList(CSerializer& serializer, CComponentBase* pComponent, T& ptrProperty, const TCHAR* pszPropertyStr)
{
    bool bStopHandle = false;
    bool bInClonePhase = CComponentInstanceManager::GetInstance()->IsInClonePhase();
    // In clone phase, we don't need to resolve anything(avoid resolve the existing component), just copy all property description.
    if (bInClonePhase)
    {
        uint32_t uLineCount, uInstanceId, uGuid;
        serializer >> uLineCount;
        for (uint32_t i = 0; i < uLineCount; ++i)
            serializer >> uInstanceId >> uGuid;
    }
    else
    {
        CPropertyDescriptionBase* pDescriptionBase = CComponentProxyManager::GetInstance()->GetCurrReflectProperty();
        CDependencyDescription* pDependencyList = CComponentProxyManager::GetInstance()->GetCurrReflectDependency();
        if (pDescriptionBase == NULL)
        {
            if (pDependencyList == NULL || _tcscmp(pDependencyList->GetVariableName(), pszPropertyStr) == 0)
            {
                uint32_t uLineCount = 0;
                serializer >> uLineCount;
                bool bNeedSnyc = true;
                if (pDependencyList != NULL && !CComponentProxyManager::GetInstance()->IsLoadingFile())
                {
                    EDependencyChangeAction action;
                    CComponentProxy* pOperateProxy = NULL;
                    pDependencyList->GetCurrActionParam(action, pOperateProxy);
                    if ((pOperateProxy->IsInitialized() || action == eDCA_Delete) && action != eDCA_Ordered)
                    {
                        bNeedSnyc = !pComponent->OnDependencyListChange(&ptrProperty, action, pOperateProxy->GetHostComponent());
                    }
                }
                if (bNeedSnyc)
                {
                    // Clear all nodes which is a dependency component.
                    for (auto iter = ptrProperty.begin(); iter != ptrProperty.end();)
                    {
                        if ((*iter)->GetId() != 0xFFFFFFFF)
                        {
                            iter = ptrProperty.erase(iter);
                        }
                        else
                        {
                            ++iter;
                        }
                    }
                    uint32_t uDepGuid = 0xFFFFFFFF;
                    if (ptrProperty.size() == 0)
                    {
                        ptrProperty.resize(1);
                        uDepGuid = ptrProperty[0]->REFLECT_GUID;
                        ptrProperty.clear();
                    }
                    else
                    {
                        uDepGuid = ptrProperty[0]->REFLECT_GUID;
                    }
                    BEATS_ASSERT(uDepGuid != 0xFFFFFFFF);
                    for (uint32_t i = 0; i < uLineCount; ++i)
                    {
                        uint32_t uInstanceId, uGuid;
                        serializer >> uInstanceId >> uGuid;
                        bool bIsParent = CComponentProxyManager::GetInstance()->IsParent(uDepGuid, uGuid);
                        BEATS_ASSERT(bIsParent,
                            _T("GUID mismatched. GUID of dependency:0x%x, GUID in datafile:0x%x"),
                            uDepGuid, uGuid);
                        if (bIsParent)
                        {
                            CComponentInstanceManager::GetInstance()->AddDependencyResolver(NULL, i, uGuid, uInstanceId, &ptrProperty, true);
                        }
                    }
                }
                if (pDependencyList != NULL)
                {
                    bStopHandle = true;
                }
            }
        }
    }
    return bStopHandle;
}

#define DECLARE_DEPENDENCY_LIST(serializer, ptrProperty, displayName, dependencyType)\
{\
    if (DeclareDependencyList(serializer, this, ptrProperty, _T(#ptrProperty))) return; \
}
#endif
#endif

#ifdef EDITOR_MODE
#define UPDATE_PROXY_PROPERTY(propertyDesc, value)\
            {\
                CSerializer serializer; \
                SerializeVariable(value, &serializer); \
                propertyDesc->Deserialize(serializer); \
                propertyDesc->Save();\
                CEngineCenter::GetInstance()->m_editorPropertyGridSyncList.insert(propertyDesc); \
            }

#define UPDATE_PROXY_PROPERTY_BY_NAME(component, value, propertyNameStr)\
        {\
        CComponentProxy* pProxy = component->GetProxyComponent(); \
        if (pProxy)\
        {\
            CPropertyDescriptionBase* pPropertyDesc = pProxy->GetPropertyDescription(propertyNameStr); \
            BEATS_ASSERT(pPropertyDesc != NULL, _T("Can not find property %s in component %s GUID:0x%x"), propertyNameStr, component->GetClassStr(), component->GetGuid()); \
            BEATS_ASSERT((pPropertyDesc->GetType() >= eRPT_Bool && pPropertyDesc->GetType() <= eRPT_Str) || pPropertyDesc->GetType() == eRPT_Color || pPropertyDesc->GetType() == eRPT_Enum || (pPropertyDesc->GetType() >= eRPT_Vec2F && pPropertyDesc->GetType() <= eRPT_Vec4F) || pPropertyDesc->GetType() == eRPT_Map || pPropertyDesc->GetType() == eRPT_List); \
            UPDATE_PROXY_PROPERTY(pPropertyDesc, value)\
        }\
    }

#define HIDE_PROPERTY(property)\
    {\
    CPropertyDescriptionBase* pDescriptionBase = CComponentProxyManager::GetInstance()->GetCurrReflectDescription(); \
    CDependencyDescription* pDependencyList = CComponentProxyManager::GetInstance()->GetCurrReflectDependency(); \
    if (pDescriptionBase == NULL && pDependencyList == NULL)\
            {\
        CComponentProxy* pProxy = GetProxyComponent(); \
        if (pProxy)\
                        {\
            CPropertyDescriptionBase* pProperty = pProxy->GetPropertyDescription(_T(#property)); \
            BEATS_ASSERT(pProperty != NULL, _T("Can not find property %s in component %s GUID:0x%x"), _T(#property), this->GetClassStr(), this->GetGuid()); \
            pProperty->SetHide(true); \
                        }\
            }\
    }

#define REINTERPRET_PTR(property, derivedGuid)\
    {\
    CPropertyDescriptionBase* pDescriptionBase = CComponentProxyManager::GetInstance()->GetCurrReflectDescription(); \
    CDependencyDescription* pDependencyList = CComponentProxyManager::GetInstance()->GetCurrReflectDependency(); \
    if (pDescriptionBase == NULL && pDependencyList == NULL)\
    {\
        CComponentProxy* pProxy = GetProxyComponent(); \
        if (pProxy)\
        {\
            CPropertyDescriptionBase* pProperty = pProxy->GetPropertyDescription(_T(#property)); \
            BEATS_ASSERT(pProperty != NULL, _T("Can not find property %s in component %s GUID:0x%x"), _T(#property), this->GetClassStr(), this->GetGuid()); \
            BEATS_ASSERT(pProperty->GetType() == eRPT_Ptr || (pProperty->GetType() == eRPT_List && down_cast<CListPropertyDescription*>(pProperty)->GetTemplateProperty()->GetType() == eRPT_Ptr)); \
            CPtrPropertyDescription* pPtrProperty = NULL;\
            if (pProperty->GetType() == eRPT_Ptr)\
            {\
                pPtrProperty = down_cast<CPtrPropertyDescription*>(pProperty); \
            }\
            else\
            {\
                pPtrProperty = down_cast<CPtrPropertyDescription*>(down_cast<CListPropertyDescription*>(pProperty)->GetTemplateProperty());\
            }\
            uint32_t uOriginPtrGuid = pPtrProperty->GetPtrGuid(); \
            if (uOriginPtrGuid != derivedGuid)\
            {\
                std::vector<uint32_t> derivedGuidList; \
                CComponentProxyManager::GetInstance()->QueryDerivedClass(derivedGuid, derivedGuidList, true); \
                if (std::find(derivedGuidList.begin(), derivedGuidList.end(), uOriginPtrGuid) == derivedGuidList.end())\
                {\
                    derivedGuidList.clear(); \
                    CComponentProxyManager::GetInstance()->QueryDerivedClass(uOriginPtrGuid, derivedGuidList, true); \
                    BEATS_ASSERT(std::find(derivedGuidList.begin(), derivedGuidList.end(), derivedGuid) != derivedGuidList.end(), _T("Guid 0x%x is not a derived class of Guid 0x%x in REINTERPRET_PTR"), derivedGuid, uOriginPtrGuid); \
                    pPtrProperty->SetPtrGuid(derivedGuid); \
                }\
            }\
    }
#else
#define UPDATE_PROXY_PROPERTY_BY_NAME(component, property, propertyNameStr)
#define HIDE_PROPERTY(property)
#define REINTERPRET_PTR(property, derivedGuid)
#endif


#define GET_VAR_NAME(var) (var, _T(#var))

#endif
