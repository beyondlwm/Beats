#ifndef BEATS_COMPONENTS_COMPONENTLAUNCHER_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENTLAUNCHER_H__INCLUDE


#ifdef EXPORT_TO_EDITOR

#define START_REGISTER_COMPONENT(FuncName)\
    void FuncName()\
    {\
        SSerilaizerExtraInfo extraInfo;\
        CSerializer serializer;\
        serializer.SetUserData(&extraInfo);\
        serializer << COMPONENT_FILE_HEADERSTR;\
        serializer << COMPONENT_SYSTEM_VERSION;\
        uint32_t nCurWritePos = serializer.GetWritePos();\
        uint32_t nComponentCounter = 0;\
        serializer << nCurWritePos;

#define REGISTER_COMPONENT(component, displayName, catalogName)\
    ++nComponentCounter;\
    {\
        serializer << (bool) false;\
        uint32_t nDataSizePosHolder = serializer.GetWritePos();\
        serializer << nDataSizePosHolder;\
        serializer << component::REFLECT_GUID;\
        serializer << component::PARENT_REFLECT_GUID;\
        serializer << _T(#component);\
        serializer << displayName;\
        serializer << catalogName;\
        uint32_t nCountHolder = serializer.GetWritePos();\
        serializer << nCountHolder;\
        serializer << nCountHolder;\
        component* pComponent = new component();\
        pComponent->ReflectData(serializer);\
        uint32_t curWritePos = serializer.GetWritePos();\
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
        uint32_t nDataSizePosHolder = serializer.GetWritePos();\
        serializer << nDataSizePosHolder;\
        serializer << component::REFLECT_GUID;\
        serializer << component::PARENT_REFLECT_GUID;\
        serializer << _T(#component);\
        uint32_t curWritePos = serializer.GetWritePos();\
        serializer.SetWritePos(nDataSizePosHolder);\
        serializer << (curWritePos - nDataSizePosHolder);\
        serializer.SetWritePos(curWritePos);\
    }

#define END_REGISTER_COMPONENT\
        uint32_t nNewCurWritePos = serializer.GetWritePos();\
        serializer.SetWritePos(nCurWritePos);\
        serializer << nComponentCounter;\
        serializer.SetWritePos(nNewCurWritePos);\
        const TCHAR* pszFileName = _T(__FUNCTION__)_T(".bin");\
        serializer.Deserialize(pszFileName);\
        TCHAR szInfo[MAX_PATH];\
        _stprintf(szInfo, _T("导出%s成功，程序即将退出"), pszFileName);\
        MessageBox(NULL, szInfo, _T("导出成功"), MB_OK);\
        _exit(0);\
    }

#else

    #define START_REGISTER_COMPONENT(FuncName)\
    void FuncName()\
    {

    #define END_REGISTER_COMPONENT }
    #define REGISTER_COMPONENT(component, displayName, catalogName)\
        CComponentInstanceManager::GetInstance()->RegisterTemplate(new component);

    #define REGISTER_ABSTRACT_COMPONENT(component)

#endif

#endif