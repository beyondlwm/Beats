#ifndef BEATS_COMPONENTS_COMPONENTPUBLIC_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENTPUBLIC_H__INCLUDE

#include "Property/PropertyPublic.h"

// To comment or un-comment this macro to decide serializer/deseraize.
#define EXPORT_TO_EDITOR

#define EXPORT_STRUCTURE_DATA_FILENAME _T("EDS.bin")
#define EXPORT_STRUCTURE_DATA_PATCH_XMLFILENAME _T("EDSPatch.XML")
#define COMPONENT_FILE_HEADERSTR _T("ComponentFile")
#define COMPONENT_FILE_EXTENSION _T(".bcf")
#define COMPONENT_FILE_EXTENSION_FILTER _T("Beats Component File(*.bcf)\0*.bcf\0\0")
#define COMPONENT_PROJECT_EXTENSION _T("Beats Component Project File(*.bcp)\0*.bcp\0\0")
#define BINARIZE_FILE_EXTENSION _T(".bin")
#define BINARIZE_FILE_EXTENSION_FILTER _T("Beats binaray File(*.bin)\0*.bin\0\0")
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
inline EPropertyType GetEnumType(T& /*value*/, CSerializer* /*serializer*/, bool /*bIgnoreValue*/)
{
    BEATS_ASSERT(false, _T("Unknown type!"));
    return ePT_Invalid;
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
        value = dynamic_cast<T*>(CComponentManager::GetInstance()->CreateComponent(uGuid, false, true, 0xFFFFFFFF, false, pSerializer));
        if ((uStartPos + uDataSize) != pSerializer->GetReadPos())
        {
            TCHAR szInfo[256];
            _stprintf(szInfo, _T("Got an error when creating data for component"));
            MessageBox(NULL, szInfo, _T("Component Data Not Match!"), MB_OK | MB_ICONERROR);
        }
    }
}

template<typename T>
inline void DeserializeVarialble(std::vector<T>& value, CSerializer* pSerializer)
{
    EPropertyType childType;
    size_t childCount = 0;
    *pSerializer >> childType;
    *pSerializer >> childCount;
    value.resize(childCount);
    for (size_t i = 0; i < childCount; ++i)
    {
        DeserializeVarialble(value[i], pSerializer);
    }
}

template<typename T>
inline EPropertyType GetEnumType(T*& /*value*/, CSerializer* pSerializer, bool /*bIgnoreValue*/)
{
    size_t guid = T::REFLECT_GUID;
    T* pTestParam = (T*)(guid);
    EPropertyType eReturnType = ePT_Invalid;
    CComponentBase* pReflect = dynamic_cast<CComponentBase*>(pTestParam);
    if (pReflect != NULL)
    {
        eReturnType = GetEnumType(pReflect, pSerializer, true);
        if (pSerializer != NULL)
        {
            *pSerializer << T::REFLECT_GUID;
        }
        return eReturnType;
    }
    BEATS_ASSERT(false, _T("Unknown type!"));
    return ePT_Invalid;
}

#define REGISTER_PROPERTY(classType, enumType)\
template<>\
inline EPropertyType GetEnumType(classType& value, CSerializer* pSerializer, bool bIgnoreValue)\
{\
    if (pSerializer != NULL)\
    {\
        *pSerializer << ((size_t)enumType);\
        if(!bIgnoreValue)\
        {\
            BEATS_ASSERT(enumType != ePT_Ptr && enumType != ePT_List);\
            *pSerializer << value;\
        }\
    }\
    return enumType;\
}

#define REGISTER_PROPERTY_SHAREPTR(classType, enumType)\
    template<typename T>\
    inline EPropertyType GetEnumType(classType<T>& /*value*/, CSerializer* pSerializer, bool /*bIgnoreValue*/)\
{\
    if (pSerializer != NULL)\
    {\
        *pSerializer << (size_t)enumType;\
        T* tmp;\
        GetEnumType(tmp, pSerializer, true);\
    }\
    return enumType;\
}

#define REGISTER_PROPERTY_TEMPLATE1(classType, enumType)\
    template<typename T>\
    inline EPropertyType GetEnumType(classType<T>& /*value*/, CSerializer* pSerializer, bool /*bIgnoreValue*/)\
{\
    if (pSerializer != NULL)\
    {\
        *pSerializer << (size_t)enumType;\
        T tmp;\
        GetEnumType(tmp, pSerializer, true);\
    }\
    return enumType;\
}

#define REGISTER_PROPERTY_TEMPLATE2(classType, enumType)\
    template<typename T1, typename T2>\
    inline EPropertyType GetEnumType(classType<T1, T2>& /*value*/, CSerializer* pSerializer, bool /*bIgnoreValue*/)\
{\
    if (pSerializer != NULL)\
{\
    *pSerializer << (size_t)enumType;\
    T1 tmp1;\
    GetEnumType(tmp1, pSerializer, true);\
    T2 tmp2;\
    GetEnumType(tmp2, pSerializer, true);\
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
            CComponentManager::GetInstance()->RegisterPropertyCreator(enumType, CreateProperty_##propertyDescriptionType);\
        }\
    }enumType##_creator_launcher;

#define REGISTER_PROPERTY_ENUM(propertyClass)\
CPropertyDescriptionBase* GetEnumPropertyDesc(int defaultValue)\
{\
    return new propertyClass(defaultValue);\
}

#ifdef EXPORT_TO_EDITOR
#define DECLARE_PROPERTY(serializer, property, editable, color, displayName, catalog, tip, parameter)\
{\
    serializer << (bool) true;\
    GetEnumType(property, &serializer, false);\
    size_t nPropertyDataSizeHolder = serializer.GetWritePos();\
    serializer << nPropertyDataSizeHolder;\
    serializer << (bool)editable << color << (displayName ? displayName : _T(#property)) << (catalog ? catalog : _T("")) << (tip ? tip : _T("")) << (_T(#property)) << (parameter ? parameter : _T(""));\
    size_t propertyDataSize = serializer.GetWritePos() - nPropertyDataSizeHolder;\
    serializer.SetWritePos(nPropertyDataSizeHolder);\
    serializer << propertyDataSize;\
    serializer.SetWritePos(nPropertyDataSizeHolder + propertyDataSize);\
    ++(((SSerilaizerExtraInfo*)(serializer.GetUserData()))->m_uPropertyCount);\
}
#define DECLARE_DEPENDENCY(serializer, ptrProperty, propertyType, displayName, dependencyType)\
{\
    serializer << (bool) false << (bool)false << dependencyType << propertyType::REFLECT_GUID << displayName << _T(#ptrProperty);\
    ++(((SSerilaizerExtraInfo*)(serializer.GetUserData()))->m_uDependencyCount);\
}
#define DECLARE_DEPENDENCY_LIST(serializer, ptrProperty, propertyType, displayName, dependencyType)\
{\
    serializer << (bool) false << (bool)true << dependencyType << propertyType::REFLECT_GUID << displayName << _T(#ptrProperty);\
    ++(((SSerilaizerExtraInfo*)(serializer.GetUserData()))->m_uDependencyCount);\
}
#define DECLARE_PROPERTY_ENUM(serializer, enumVariable, count, selfDefineStrArray, enumType, editable, color, displayName, catalog, tip, parameter)\
{\
    serializer << (bool) true << ePT_Enum << (int)enumVariable << _T(#enumType) << count;\
    for(size_t i = 0; i < count; ++i)\
    {\
        BEATS_ASSERT(selfDefineStrArray != NULL, _T("序列化枚举失败，设定了枚举数目却没有指定枚举对应的字符串数组。位于组件%s 的枚举%s"), GetClassStr(), _T(#enumType));\
        serializer << ((const TCHAR**)selfDefineStrArray)[i];\
    }\
    size_t nPropertyDataSizeHolder = serializer.GetWritePos();\
    serializer << nPropertyDataSizeHolder;\
    serializer << (bool)editable << color << (displayName ? displayName : _T(#enumVariable)) << (catalog ? catalog : _T("")) << (tip ? tip : _T("")) << (_T(#enumVariable)) << (parameter ? parameter : _T(""));\
    size_t propertyDataSize = serializer.GetWritePos() - nPropertyDataSizeHolder;\
    serializer.SetWritePos(nPropertyDataSizeHolder);\
    serializer << propertyDataSize;\
    serializer.SetWritePos(nPropertyDataSizeHolder + propertyDataSize);\
    ++(((SSerilaizerExtraInfo*)(serializer.GetUserData()))->m_uPropertyCount);\
}


#else
#define DECLARE_PROPERTY(serializer, property, editable, color, displayName, catalog, tip, parameter) DeserializeVarialble(property, &serializer);
    
#define DECLARE_DEPENDENCY(serializer, ptrProperty, propertyType, displayName, dependencyType)\
        {\
            size_t uLineCount = 0;\
            serializer >> uLineCount;\
            BEATS_ASSERT(uLineCount <= 1);\
            if (uLineCount == 1)\
            {\
                size_t uInstanceId, uGuid;\
                serializer >> uInstanceId >> uGuid;\
                CComponentManager::GetInstance()->AddDependencyResolver(NULL, 0 , uGuid, uInstanceId, &ptrProperty, false);\
            }\
}

#define DECLARE_DEPENDENCY_LIST(serializer, ptrProperty, propertyType, displayName, dependencyType)\
        {\
            size_t uLineCount = 0;\
            serializer >> uLineCount;\
            for (size_t i = 0; i < uLineCount; ++i)\
            {\
                size_t uInstanceId, uGuid;\
                serializer >> uInstanceId >> uGuid;\
                CComponentManager::GetInstance()->AddDependencyResolver(NULL, i , uGuid, uInstanceId, &ptrProperty, true);\
            }\
        }

#define DECLARE_PROPERTY_ENUM(serializer, enumVariable, count, selfDefineStrArray, enumType, editable, color, displayName, catalog, tip, parameter)\
    serializer >> enumVariable;
#endif

#ifdef EXPORT_TO_EDITOR

#define START_REGISTER_COMPONET\
    struct SRegisterLauncher\
    {\
        ~SRegisterLauncher()\
        {\
            CComponentManager::Destroy();\
        }\
        SRegisterLauncher()\
        {\
            CComponentBase* pBase = NULL;\
            SSerilaizerExtraInfo extraInfo;\
            CSerializer serializer;\
            serializer.SetUserData(&extraInfo);\
            serializer << COMPONENT_FILE_HEADERSTR;\
            serializer << COMPONENT_SYSTEM_VERSION;\
            size_t nCurWritePos = serializer.GetWritePos();\
            size_t nComponentCounter = 0;\
            serializer << nCurWritePos;

#define END_REGISTER_COMPONET\
        size_t nNewCurWritePos = serializer.GetWritePos();\
        serializer.SetWritePos(nCurWritePos);\
        serializer << nComponentCounter;\
        serializer.SetWritePos(nNewCurWritePos);\
        CComponentManager::GetInstance()->SerializeTemplateData(serializer);\
        }\
    };\
    SRegisterLauncher registerLauncher;\
    void(*pComponentLauncherFunc)() = NULL;

#define REGISTER_COMOPNENT(component, displayName, catalogName)\
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
    pBase = new component(serializer);\
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
        serializer << (size_t)12;\
        serializer << component::REFLECT_GUID;\
        serializer << component::PARENT_REFLECT_GUID;\
    }
#else

#define START_REGISTER_COMPONET\
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

#define END_REGISTER_COMPONET\
            TCHAR szFilePath[MAX_PATH];\
            GetModuleFileName(NULL, szFilePath, MAX_PATH);\
            PathRemoveFileSpec(szFilePath);\
            _tcscat(szFilePath, _T("\\AIData.bin"));\
            bool bFileExists = PathFileExists(szFilePath);\
            BEATS_ASSERT(bFileExists, _T("The data file doesn't exists in path : %s"), szFilePath);\
            if(bFileExists)\
            {\
                CSerializer serializer(szFilePath);\
                CComponentManager::GetInstance()->Import(serializer);\
            }\
        }\
    };\
    SRegisterLauncher registerLauncher;\
    void(*pComponentLauncherFunc)() = &SRegisterLauncher::Launch;

#define REGISTER_COMOPNENT(component, displayName, catalogName)\
    CComponentManager::GetInstance()->RegisterTemplate(new component);

#define REGISTER_ABSTRACT_COMPONENT(component)

#endif
    extern void(*pComponentLauncherFunc)();
#endif