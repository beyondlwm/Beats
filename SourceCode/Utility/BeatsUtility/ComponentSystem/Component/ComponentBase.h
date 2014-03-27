#ifndef BEATS_COMPONENTS_COMPONENT_COMPONENTBASE_H__INCLUDE
#define BEATS_COMPONENTS_COMPONENT_COMPONENTBASE_H__INCLUDE

#include "Utility/BeatsUtility/ComponentSystem/ComponentPublic.h"

extern void ForceReferenceSymbolForComponent();

#define DECLARE_REFLECT_GUID(className, guid, parentClassName)\
    DECLARE_REFLECT_GUID_ABSTRACT(className, guid, parentClassName)\
    public:\
    virtual CComponentBase* Clone(bool /*bCloneFromTemplate*/, CSerializer* pSerializer){CComponentBase* pNewInstance = pSerializer == NULL ? new className : new className(*pSerializer); return pNewInstance;}\
    private:

#define DECLARE_REFLECT_GUID_ABSTRACT(className, guid, parentClassName)\
        public:\
        static const size_t REFLECT_GUID = guid;\
        static const size_t PARENT_REFLECT_GUID = parentClassName::REFLECT_GUID;\
        virtual size_t GetGuid() const {return REFLECT_GUID;}\
        virtual size_t GetParentGuid() const {return PARENT_REFLECT_GUID;}\
        virtual const TCHAR* GetClassStr() const {ForceReferenceSymbolForComponent();return _T(#className);}\
        private:

class CSerializer;

class CComponentBase
{
public:
    CComponentBase();
    CComponentBase(CSerializer& serializer);
    virtual ~CComponentBase();
    DECLARE_REFLECT_GUID(CComponentBase, 0xFBA14097, CComponentBase)
    
public:
    size_t GetId() const;
    void SetId(size_t id);

    virtual void Initialize();

private:
    size_t m_id;
};
#endif