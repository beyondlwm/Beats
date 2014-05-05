#ifndef BEATS_COMPONENTS_COMPONENTPUBLIC_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENTPUBLIC_H__INCLUDE

#include <string>
#include "Property/PropertyPublic.h"
#include "../Utility/StringHelper/StringHelper.h"
#include "Component/ComponentEditorProxy.h"

// To comment or un-comment this macro to decide serializer/deserialize.
//#define EXPORT_TO_EDITOR
#define EDITOR_MODE

#define EXPORT_STRUCTURE_DATA_FILENAME _T("EDS.bin")
#define EXPORT_STRUCTURE_DATA_PATCH_XMLFILENAME _T("EDSPatch.XML")
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
    size_t m_uPropertyCount;
    size_t m_uDependencyCount;
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
        TCHAR szNameBuffer[128];
        CStringHelper::GetInstance()->ConvertToTCHAR(&pszTypeName[strlen("enum ")], szNameBuffer, 128);
        (*pSerializer) << (size_t)eRPT_Enum;
        (*pSerializer) << (int)(value) << szNameBuffer;
    }
    BEATS_ASSERT(bIsEnum, _T("Unknown type!"));
    return eRet;
}

template<typename T>
inline void DeserializeVarialble(T& value, CSerializer* pSerializer)
{
    *pSerializer >> value;
}

template<typename T>
inline void DeserializeVarialble(T*& value, CSerializer* pSerializer)
{
    bool bHasInstance = false;
    *pSerializer >> bHasInstance;
    if(bHasInstance)
    {
        size_t uDataSize, uGuid, uId, uStartPos;
        uStartPos = pSerializer->GetReadPos();
        *pSerializer >> uDataSize;
        *pSerializer >> uGuid;
        *pSerializer >> uId;
#ifdef EDITOR_MODE
        if (value == NULL)
        {
            CComponentBase* pComponent = CComponentManager::GetInstance()->CreateComponent(uGuid, false, true, 0xFFFFFFFF, false, pSerializer);
            CComponentEditorProxy* pProxy = dynamic_cast<CComponentEditorProxy*>(pComponent);
            if (pProxy != NULL)
            {
                value = dynamic_cast<T*>(pProxy->GetHostComponent());
                pProxy->SetHostComponent(NULL);
                BEATS_SAFE_DELETE(pProxy);
                value->ReflectData(*pSerializer);
            }
            else
            {
                value = dynamic_cast<T*>(pComponent);
            }
        }
        else
        {
            value->ReflectData(*pSerializer);
        }
        BEATS_ASSERT(value != NULL,_T("Deserialize pointer failed! guid:0x%x"), uGuid);
        BEATS_ASSERT(uStartPos + uDataSize == pSerializer->GetReadPos(), 
            _T("Component Data Not Match!\nGot an error when Deserialize a pointer of component 0x%x %s instance id %d\nRequired size: %d, Actual size: %d"), uGuid, value->GetClassStr(), uId, uDataSize, pSerializer->GetReadPos() - uStartPos);
    }
    else
    {
        if (value != NULL)
        {
            BEATS_SAFE_DELETE(value);
        }

#else
        value = dynamic_cast<T*>(CComponentManager::GetInstance()->CreateComponent(uGuid, false, true, 0xFFFFFFFF, false, pSerializer));
        BEATS_ASSERT(uStartPos + uDataSize == pSerializer->GetReadPos(), 
            _T("Component Data Not Match!\nGot an error when Deserialize a pointer of component 0x%x %s instance id %d\nRequired size: %d, Actual size: %d"), uGuid, value->GetClassStr(), uId, uDataSize, pSerializer->GetReadPos() - uStartPos);
#endif // EDITOR_MODE
    }
}

#ifdef EDITOR_MODE
template<typename T>
inline void ResizeVector(std::vector<T>& value, size_t uCount)
{
    value.resize(uCount);
}

template<typename T>
inline void ResizeVector(std::vector<T*>& value, size_t uCount)
{
    if (value.size() > uCount)
    {
        for (size_t i = 0; i < value.size() - uCount; ++i)
        {
            BEATS_ASSERT(dynamic_cast<CComponentBase*>(value.back()) != NULL);
            BEATS_SAFE_DELETE(value.back());
            value.pop_back();
        }
    }
    else
    {
        value.resize(uCount);
    }
}

template<typename T1, typename T2>
inline void ClearMap(std::map<T1, T2>& value)
{
    value.clear();
}

template<typename T1, typename T2>
inline void ClearMap(std::map<T1, T2*>& value)
{
    for (std::map<T1, T2*>::iterator iter = value.begin(); iter != value.end(); ++iter)
    {
        BEATS_SAFE_DELETE(iter->second);
    }
    value.clear();
}

#endif

template<typename T>
inline void DeserializeVarialble(std::vector<T>& value, CSerializer* pSerializer)
{
    size_t childCount = 0;
    *pSerializer >> childCount;
#ifdef EDITOR_MODE
    ResizeVector(value, childCount);
#else
    value.resize(childCount);
#endif
    for (size_t i = 0; i < childCount; ++i)
    {
        DeserializeVarialble(value[i], pSerializer);
    }
}

template<typename T1, typename T2>
inline void DeserializeVarialble(std::map<T1, T2>& value, CSerializer* pSerializer)
{
    EReflectPropertyType keyType;
    EReflectPropertyType valueType;
    size_t childCount = 0;
    *pSerializer >> keyType;
    *pSerializer >> valueType;
    *pSerializer >> childCount;
#ifdef EDITOR_MODE
    ClearMap(value);
#else
    BEATS_ASSERT(value.size() == 0, _T("map should be empty when deserialize!"));
#endif
    for (size_t i = 0; i < childCount; ++i)
    {
        T1 key;
        InitValue(key);
        DeserializeVarialble(key, pSerializer);
        T2 myValue;
        InitValue(myValue);
        DeserializeVarialble(myValue, pSerializer);
        BEATS_ASSERT(value.find(key) == value.end(), _T("A map can't have two same key value!"));
        value[key] = myValue;
    }
}

template<typename T>
inline EReflectPropertyType GetEnumType(T*& /*value*/, CSerializer* pSerializer)
{
    size_t guid = T::REFLECT_GUID;
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
        *pSerializer << ((size_t)enumType);\
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
        *pSerializer << (size_t)enumType;\
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
        *pSerializer << (size_t)enumType;\
        T tmp;\
        InitValue(tmp);\
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
    *pSerializer << (size_t)enumType;\
    T1 tmp1;\
    InitValue(tmp1);\
    GetEnumType(tmp1, pSerializer);\
    T2 tmp2;\
    InitValue(tmp2);\
    GetEnumType(tmp2, pSerializer);\
}\
    return enumType;\
}

template<typename T>
inline void InitValue(const T& param)
{
    //TODO: I don't know how to init a template value, so this is the HACK way.
    if (sizeof(T) <= 4)
    {
        ZeroMemory((void*)&param, sizeof(T));
    }
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
            CComponentManager::GetInstance()->RegisterPropertyCreator(enumType, CreateProperty_##propertyDescriptionType);\
        }\
    }enumType##_creator_launcher;

#define REGISTER_PROPERTY_ENUM(propertyClass)\
CPropertyDescriptionBase* GetEnumPropertyDesc(int defaultValue)\
{\
    return new propertyClass(defaultValue);\
}

#ifdef EXPORT_TO_EDITOR

inline const TCHAR* GenEnumParamStr(const TCHAR* enumStringArray[], const TCHAR* pszParam = NULL)
{
    static TString strEnumParam;
    strEnumParam.clear();
    strEnumParam.append(UIParameterAttrStr[eUIPAT_EnumStringArray]).append(PROPERTY_KEYWORD_SPLIT_STR);
    size_t uCount = sizeof(enumStringArray);
    BEATS_ASSERT(uCount > 0, _T("Enum string array is empty!"));
    for (size_t i = 0; i < uCount; ++i)
    {
        strEnumParam.append(enumStringArray[i]);
        if (i < uCount - 1)
        {
            strEnumParam.append(_T("@"));
        }
    }
    if(pszParam != NULL)
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

#define DECLARE_PROPERTY(serializer, property, editable, color, displayName, catalog, tip, parameter)\
{\
    BEATS_ASSERT(serializer.GetUserData() != NULL, _T("Serializer doesn't contain an user data!\nPlease make sure you enalbe EDITOR_MODE when launch editor!"));\
    serializer << (bool) true;\
    EReflectPropertyType propertyType = GetEnumType(property, &serializer);\
    size_t nPropertyDataSizeHolder = serializer.GetWritePos();\
    serializer << nPropertyDataSizeHolder;\
    const TCHAR* pszParam = parameter;\
    if (propertyType == eRPT_Enum && pszParam != NULL)\
    {\
        size_t uEnumStringArrayNameLen = _tcslen(UIParameterAttrStr[eUIPAT_EnumStringArray]);\
        if (_tcslen(pszParam) > uEnumStringArrayNameLen && memcmp(pszParam, UIParameterAttrStr[eUIPAT_EnumStringArray], uEnumStringArrayNameLen) == 0)\
        {\
            const char* pszTypeName = typeid(property).name();\
            TCHAR szNameBuffer[128];\
            CStringHelper::GetInstance()->ConvertToTCHAR(&pszTypeName[strlen("enum ")], szNameBuffer, 128);\
            TString strEnumName = szNameBuffer;\
            if (CheckIfEnumHasExported(strEnumName))\
            {\
                pszParam = NULL;/*It's not necessary to export the same enum string array value, so ignore it!*/\
            }\
        }\
    }\
    serializer << (bool)editable << color << (displayName ? displayName : _T(#property)) << (catalog ? catalog : _T("")) << (tip ? tip : _T("")) << (_T(#property)) << (pszParam ? pszParam : _T(""));\
    size_t propertyDataSize = serializer.GetWritePos() - nPropertyDataSizeHolder;\
    serializer.SetWritePos(nPropertyDataSizeHolder);\
    serializer << propertyDataSize;\
    serializer.SetWritePos(nPropertyDataSizeHolder + propertyDataSize);\
    ++(((SSerilaizerExtraInfo*)(serializer.GetUserData()))->m_uPropertyCount);\
}
#define DECLARE_DEPENDENCY(serializer, ptrProperty, displayName, dependencyType)\
{\
    BEATS_ASSERT(serializer.GetUserData() != NULL, _T("Serializer doesn't contain an user data!\nPlease make sure you enalbe EDITOR_MODE when launch editor!"));\
    serializer << (bool) false << (bool)false << dependencyType << ptrProperty->REFLECT_GUID << displayName << _T(#ptrProperty);\
    ++(((SSerilaizerExtraInfo*)(serializer.GetUserData()))->m_uDependencyCount);\
}
#define DECLARE_DEPENDENCY_LIST(serializer, ptrProperty, displayName, dependencyType)\
{\
    BEATS_ASSERT(serializer.GetUserData() != NULL, _T("Serializer doesn't contain an user data!\nPlease make sure you enalbe EDITOR_MODE when launch editor!"));\
    ptrProperty.resize(1);\
    serializer << (bool) false << (bool)true << dependencyType << ptrProperty[0]->REFLECT_GUID << displayName << _T(#ptrProperty);\
    ptrProperty.resize(0);\
    ++(((SSerilaizerExtraInfo*)(serializer.GetUserData()))->m_uDependencyCount);\
}

#else
#define DECLARE_PROPERTY(serializer, property, editable, color, displayName, catalog, tip, parameter) DeserializeVarialble(property, &serializer);

#define DECLARE_DEPENDENCY(serializer, ptrProperty, displayName, dependencyType)\
        {\
            size_t uLineCount = 0;\
            serializer >> uLineCount;\
            BEATS_ASSERT(uLineCount <= 1, _T("Data error:\nWe want a dependency data, but got %d line count!"), uLineCount);\
            ptrProperty = NULL;\
            if (uLineCount == 1)\
            {\
                size_t uInstanceId, uGuid;\
                serializer >> uInstanceId >> uGuid;\
                CComponentManager::GetInstance()->AddDependencyResolver(NULL, 0 , uGuid, uInstanceId, &ptrProperty, false);\
            }\
        }

#define DECLARE_DEPENDENCY_LIST(serializer, ptrProperty, displayName, dependencyType)\
        {\
            size_t uLineCount = 0;\
            serializer >> uLineCount;\
            ptrProperty.clear();\
            for (size_t i = 0; i < uLineCount; ++i)\
            {\
                size_t uInstanceId, uGuid;\
                serializer >> uInstanceId >> uGuid;\
                CComponentManager::GetInstance()->AddDependencyResolver(NULL, i , uGuid, uInstanceId, &ptrProperty, true);\
            }\
        }

#endif

#ifdef EXPORT_TO_EDITOR

#define START_REGISTER_COMPONENT\
    struct SRegisterLauncher\
    {\
        ~SRegisterLauncher()\
        {\
            CComponentManager::Destroy();\
        }\
        SRegisterLauncher()\
        {\
            SSerilaizerExtraInfo extraInfo;\
            CSerializer serializer;\
            serializer.SetUserData(&extraInfo);\
            serializer << COMPONENT_FILE_HEADERSTR;\
            serializer << COMPONENT_SYSTEM_VERSION;\
            size_t nCurWritePos = serializer.GetWritePos();\
            size_t nComponentCounter = 0;\
            serializer << nCurWritePos;

#define END_REGISTER_COMPONENT\
        size_t nNewCurWritePos = serializer.GetWritePos();\
        serializer.SetWritePos(nCurWritePos);\
        serializer << nComponentCounter;\
        serializer.SetWritePos(nNewCurWritePos);\
        CComponentManager::GetInstance()->SerializeTemplateData(serializer);\
        }\
    };\
    SRegisterLauncher registerLauncher;\
    void(*pComponentLauncherFunc)() = NULL;

#define REGISTER_COMPONENT(component, displayName, catalogName)\
    ++nComponentCounter;\
    {\
    serializer << (bool) false;\
    size_t nDataSizePosHolder = serializer.GetWritePos();\
    serializer << nDataSizePosHolder;\
    serializer << component::REFLECT_GUID;\
    serializer << component::PARENT_REFLECT_GUID;\
    serializer << _T(#component);\
    serializer << displayName;\
    serializer << catalogName;\
    size_t nCountHolder = serializer.GetWritePos();\
    serializer << nCountHolder;\
    serializer << nCountHolder;\
    component* pComponent = new component();\
    pComponent->ReflectData(serializer);\
    CComponentManager::GetInstance()->RegisterTemplate(pComponent);\
    size_t curWritePos = serializer.GetWritePos();\
    serializer.SetWritePos(nCountHolder);\
    SSerilaizerExtraInfo* pExtraInfo = (SSerilaizerExtraInfo*)(serializer.GetUserData());\
    serializer << pExtraInfo->m_uPropertyCount;\
    pExtraInfo->m_uPropertyCount = 0;\
    serializer << pExtraInfo->m_uDependencyCount;\
    pExtraInfo->m_uDependencyCount = 0;\
    serializer.SetWritePos(nDataSizePosHolder);\
    serializer << (curWritePos - nDataSizePosHolder);\
    serializer.SetWritePos(curWritePos);\
    }

#define REGISTER_ABSTRACT_COMPONENT(component)\
    ++nComponentCounter;\
    {\
        serializer << (bool)true;\
        size_t nDataSizePosHolder = serializer.GetWritePos();\
        serializer << nDataSizePosHolder;\
        serializer << component::REFLECT_GUID;\
        serializer << component::PARENT_REFLECT_GUID;\
        serializer << _T(#component);\
        size_t curWritePos = serializer.GetWritePos();\
        serializer.SetWritePos(nDataSizePosHolder);\
        serializer << (curWritePos - nDataSizePosHolder);\
        serializer.SetWritePos(curWritePos);\
    }
#else

#define START_REGISTER_COMPONENT\
    struct SRegisterLauncher\
    {\
    ~SRegisterLauncher()\
    {\
    CComponentManager::Destroy();\
    }\
    SRegisterLauncher()\
    {\
    Launch();\
    }\
    static void Launch()\
    {

#ifdef EDITOR_MODE

#define END_REGISTER_COMPONENT\
            }\
            };\
            void(*pComponentLauncherFunc)() = &SRegisterLauncher::Launch;

#else

#define END_REGISTER_COMPONENT\
    TCHAR szFilePath[MAX_PATH];\
    GetModuleFileName(NULL, szFilePath, MAX_PATH);\
    PathRemoveFileSpec(szFilePath);\
    _tcscat(szFilePath, _T("\\"));\
    _tcscat(szFilePath, BINARIZE_FILE_NAME);\
    bool bFileExists = PathFileExists(szFilePath) == TRUE;\
    BEATS_ASSERT(bFileExists, _T("The data file doesn't exists in path : %s"), szFilePath);\
    if(bFileExists)\
{\
    CSerializer serializer(szFilePath);\
    CComponentManager::GetInstance()->Import(serializer);\
                }\
                }\
                };\
                void(*pComponentLauncherFunc)() = &SRegisterLauncher::Launch;

#endif

#define REGISTER_COMPONENT(component, displayName, catalogName)\
    CComponentManager::GetInstance()->RegisterTemplate(new component);

#define REGISTER_ABSTRACT_COMPONENT(component)

#endif
    extern void(*pComponentLauncherFunc)();
#endif