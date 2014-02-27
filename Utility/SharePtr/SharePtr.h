#ifndef BEATS_UTILITY_SHAREPTR_SHAREPTR_H__INCLUDE
#define BEATS_UTILITY_SHAREPTR_SHAREPTR_H__INCLUDE

template<typename ClassType>
class SharePtr
{
public:
    SharePtr<ClassType>()
        : m_pObject(NULL)
        , m_refCount(NULL)
    {
    }

    SharePtr<ClassType>(ClassType* pObject)
        : m_pObject(pObject)
        , m_refCount (NULL)
    {
        if (pObject != NULL)
        {
            m_refCount = new long(1);
        }
        BEATS_ASSERT(pObject != NULL, _T("Object can't be NULL for share pointer!"));
    }

    SharePtr<ClassType>(const SharePtr<ClassType>& value)
        : m_pObject(NULL)
        , m_refCount(NULL)
    {
        *this = value;
    }

    ~SharePtr<ClassType>()
    {
        Destroy();
    }

    SharePtr<ClassType>&  operator = (const SharePtr<ClassType>& value)
    {
        if (*this != value)
        {
            if (m_pObject != NULL && m_refCount != NULL && *m_refCount > 0)
            {
                Destroy();
            }
            m_pObject = value.m_pObject;
            m_refCount = value.m_refCount;
            ::InterlockedIncrement(m_refCount);
        }
        return *this;
    }

    ClassType* operator ->()
    {
        return m_pObject;
    }

    ClassType* operator ->() const
    {
        return m_pObject;
    }

    bool operator == (const SharePtr<ClassType>& rhs) const
    {
        return m_pObject == rhs.m_pObject;
    }

    bool operator != (const SharePtr<ClassType>& rhs) const
    {
        return m_pObject != rhs.m_pObject;
    }

    void Create(ClassType* pObject)
    {
        bool bCanCreate = m_pObject == NULL && m_refCount == NULL && pObject != NULL;
        BEATS_ASSERT(bCanCreate, _T("Can't create a share ptr twice!"));
        if (bCanCreate)
        {
           m_pObject = pObject;
           m_refCount = new long(1);
        }
    }

    ClassType* Get()
    {
        return m_pObject;
    }

    int RefCount()
    {
        return *m_refCount;
    }

private:
    void Destroy()
    {
        bool bCanDestroy = m_refCount != NULL && m_pObject != NULL && *m_refCount > 0;
        if (bCanDestroy)
        {
            BEATS_ASSERT(*m_refCount > 0, _T("Ref count is invalid for share pointer destroy"));
            ::InterlockedDecrement(m_refCount);
            if (*m_refCount == 0)
            {
                BEATS_SAFE_DELETE(m_refCount);
                BEATS_SAFE_DELETE(m_pObject);
            }
        }
    }

private:
    long* m_refCount;
    ClassType* m_pObject;
};


#endif