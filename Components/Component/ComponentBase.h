#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTBASE_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTBASE_H__INCLUDE

#include "../expdef.h"
#include "DependencyDescription.h"
#include "ComponentInstanceManager.h"

#define DECLARE_REFLECT_GUID(className, guid, parentClassName)\
    DECLARE_REFLECT_GUID_ABSTRACT(className, guid, parentClassName)\
    public:\
    virtual CComponentBase* Clone(bool /*bCloneFromTemplate*/, CSerializer* pSerializer, uint32_t id, bool bCallInitFunc = true)\
    {\
        bool bOriginClonePhase = CComponentInstanceManager::GetInstance()->IsInClonePhase(); \
        CComponentInstanceManager::GetInstance()->SetClonePhaseFlag(true); \
        BEATS_ASSERT(typeid(className) == typeid(*this), _T("Define wrong class type: define %s"), _T(#className)); \
        CComponentBase* pNewInstance = new className; \
        pNewInstance->SetId(id); \
        if (pSerializer != NULL)\
        {\
            pNewInstance->ReflectData(*pSerializer); \
            if (bCallInitFunc)\
            {\
                pNewInstance->Initialize(); \
            }\
        }\
        CComponentInstanceManager::GetInstance()->SetClonePhaseFlag(bOriginClonePhase); \
        return pNewInstance; \
    }\
    private:

#define DECLARE_REFLECT_GUID_ABSTRACT(className, guid, parentClassName)\
        public:\
        static const uint32_t REFLECT_GUID = guid;\
        static const uint32_t PARENT_REFLECT_GUID = parentClassName::REFLECT_GUID;\
        virtual uint32_t GetGuid() const {return REFLECT_GUID;}\
        virtual uint32_t GetParentGuid() const {return PARENT_REFLECT_GUID;}\
        virtual const TCHAR* GetClassStr() const {return _T(#className);}\
        private:\
        typedef parentClassName super;


class CSerializer;
class CDependencyDescription;
class COMPONENTS_DLL_DECL CComponentBase
{
public:
    CComponentBase();
    virtual ~CComponentBase();
    DECLARE_REFLECT_GUID(CComponentBase, 0xFBA14097, CComponentBase)
    
public:
    uint32_t GetId() const;
    void SetId(uint32_t id);

    virtual bool Load(); // This method do some load stuff, which can be done in other thread.
    virtual bool Unload();
    bool IsLoaded() const;
    void SetLoadFlag(bool bFlag);

    virtual void Initialize();// This method can only be called in main thread, including render operation.
    virtual void Uninitialize();
    bool IsInitialized() const;
    void SetInitializeFlag(bool bFlag);

    // Game mode only!
    virtual void ReflectData(CSerializer& serializer);

    // Editor mode only!
    // The return value means if user has sync the variable manually, or system will do this assign.
    virtual bool OnPropertyChange(void* pVariableAddr, CSerializer* pNewValueToBeSet);
    virtual bool OnDependencyChange(void* pComponentAddr, CComponentBase* pComponent);
    virtual bool OnDependencyListChange(void* pComponentAddr, EDependencyChangeAction action, CComponentBase* pComponent);
    virtual void OnSave();

private:
    bool m_bLoaded;
    bool m_bInitialize;
    uint32_t m_id;
};
#endif