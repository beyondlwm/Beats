#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTBASE_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTBASE_H__INCLUDE

#include "../expdef.h"
#define DECLARE_REFLECT_GUID(className, guid, parentClassName)\
    DECLARE_REFLECT_GUID_ABSTRACT(className, guid, parentClassName)\
    public:\
    virtual CComponentBase* Clone(bool /*bCloneFromTemplate*/, CSerializer* pSerializer, size_t id, bool bCallInitFunc = true){CComponentBase* pNewInstance = new className; pNewInstance->SetId(id); if (pSerializer != NULL){pNewInstance->ReflectData(*pSerializer);if(bCallInitFunc){pNewInstance->Initialize();}} return pNewInstance;}\
    private:

#define DECLARE_REFLECT_GUID_ABSTRACT(className, guid, parentClassName)\
        public:\
        static const size_t REFLECT_GUID = guid;\
        static const size_t PARENT_REFLECT_GUID = parentClassName::REFLECT_GUID;\
        virtual size_t GetGuid() const {return REFLECT_GUID;}\
        virtual size_t GetParentGuid() const {return PARENT_REFLECT_GUID;}\
        virtual const TCHAR* GetClassStr() const {return _T(#className);}\
        private:

class CSerializer;

class COMPONENTS_DLL_DECL CComponentBase
{
public:
    CComponentBase();
    virtual ~CComponentBase();
    DECLARE_REFLECT_GUID(CComponentBase, 0xFBA14097, CComponentBase)
    
public:
    size_t GetId() const;
    void SetId(size_t id);

    virtual void Initialize();
    virtual void Uninitialize();
    virtual void ReflectData(CSerializer& serializer);
    // Editor mode only!
    // The return value means if user has sync the variable manually, or system will do this assign.
    // If it is a property, the pNewValueToBeSet means the value data.
    // if it is a dependency or dependency list, it is a pointer to CDependencyDescription.
    virtual bool OnPropertyChange(void* pVariableAddr, void* pNewValueToBeSet);

private:
    size_t m_id;
};
#endif