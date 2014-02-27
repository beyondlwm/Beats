#if FUNCTION_PARAM_COUNT == 0
#define FUNCTION_COMMA
#else
#define FUNCTION_COMMA ,
#endif


// Generate the class name
#define BASE_FUNCTION_NAME GEN_NAME(BaseFunction, FUNCTION_PARAM_COUNT)
#define POINTER_FUNCTION_NAME GEN_NAME(PointerFunction, FUNCTION_PARAM_COUNT)
#define MEMBER_FUNCTION_NAME GEN_NAME(MemberFunction, FUNCTION_PARAM_COUNT)
#define MEMBER_FUNCTION_CONST_NAME GEN_NAME(MemberFunctionConst, FUNCTION_PARAM_COUNT)
#define MEMBER_FUNCTION_SHARE_PTR_NAME GEN_NAME(MemberFunctionSharePtr, FUNCTION_PARAM_COUNT)
#define MEMBER_FUNCTION_CONST_SHARE_PTR_NAME GEN_NAME(MemberFunctionConstSharePtrEx, FUNCTION_PARAM_COUNT)
#define FUNCTION_WRAPPER_NAME GEN_NAME(FunctionWrapper, FUNCTION_PARAM_COUNT)


////////////////////////////////////////////


/// BaseFunction
template < typename R FUNCTION_COMMA GEN_PARAM( FUNCTION_PARAM_COUNT, typename Arg ) >
class BASE_FUNCTION_NAME
{
public:
    typedef R ReturnType;

public:
    virtual ~BASE_FUNCTION_NAME() {};

    virtual R Call( GEN_PARAM(FUNCTION_PARAM_COUNT, Arg) ) = 0;

    virtual BASE_FUNCTION_NAME* Clone() = 0;

    virtual bool IsEqual(BASE_FUNCTION_NAME* pBase) const = 0;
};


/// PointerFunction
template < typename R FUNCTION_COMMA GEN_PARAM( FUNCTION_PARAM_COUNT, typename Arg ) >
class POINTER_FUNCTION_NAME : public BASE_FUNCTION_NAME< R FUNCTION_COMMA GEN_PARAM( FUNCTION_PARAM_COUNT, Arg ) >
{
public:
    typedef ReturnType (*FunctionType)( GEN_PARAM( FUNCTION_PARAM_COUNT, Arg ) );

public:
    explicit POINTER_FUNCTION_NAME(FunctionType func)
        : m_func(func)
    {
    }

public:
    virtual ReturnType Call( GEN_PARAM_EX( FUNCTION_PARAM_COUNT, Arg, arg ) )
    {
        return m_func( GEN_PARAM( FUNCTION_PARAM_COUNT, arg ) );
    }

    ReturnType operator()( GEN_PARAM_EX( FUNCTION_PARAM_COUNT, Arg, arg ) )
    {
        return Call( GEN_PARAM( FUNCTION_PARAM_COUNT, arg ) );
    }

    virtual BASE_FUNCTION_NAME* Clone()
    {
        return new POINTER_FUNCTION_NAME(m_func);
    }

    virtual bool IsEqual(BASE_FUNCTION_NAME* pBase) const
    {
        const POINTER_FUNCTION_NAME* pOther = static_cast<const POINTER_FUNCTION_NAME*>(pBase);
        return m_func == pOther->m_func;
    }

private:
    FunctionType m_func;
};


/// MemberFunction
template < typename R, typename Object FUNCTION_COMMA GEN_PARAM( FUNCTION_PARAM_COUNT, typename Arg ) >
class MEMBER_FUNCTION_NAME : public BASE_FUNCTION_NAME< R FUNCTION_COMMA GEN_PARAM( FUNCTION_PARAM_COUNT, Arg ) >
{
public:
    typedef ReturnType (Object::*FunctionType)( GEN_PARAM( FUNCTION_PARAM_COUNT, Arg ) );

public:
    explicit MEMBER_FUNCTION_NAME(Object& obj, FunctionType func)
        : m_obj(&obj) 
        , m_memFunc(func)
    {
    }

public:
    virtual ReturnType Call( GEN_PARAM_EX( FUNCTION_PARAM_COUNT, Arg, arg ) )
    {
        return (m_obj->*m_memFunc)( GEN_PARAM( FUNCTION_PARAM_COUNT, arg ) );
    }

    ReturnType operator()( GEN_PARAM_EX( FUNCTION_PARAM_COUNT, Arg, arg ) )
    {
        return Call( GEN_PARAM( FUNCTION_PARAM_COUNT, arg ) );
    }

    virtual BASE_FUNCTION_NAME* Clone()
    {
        return new MEMBER_FUNCTION_NAME(*m_obj, m_memFunc);
    }

    virtual bool IsEqual(BASE_FUNCTION_NAME* pBase) const
    {
        const MEMBER_FUNCTION_NAME* pOther = static_cast<const MEMBER_FUNCTION_NAME*>(pBase);
        return (m_obj == pOther->m_obj) && (m_memFunc == pOther->m_memFunc);
    }

private:
    Object*  m_obj;
    FunctionType    m_memFunc;
};


/// MemberFunctionConst
template < typename R, typename Object FUNCTION_COMMA GEN_PARAM( FUNCTION_PARAM_COUNT, typename Arg ) >
class MEMBER_FUNCTION_CONST_NAME : public BASE_FUNCTION_NAME< R FUNCTION_COMMA GEN_PARAM( FUNCTION_PARAM_COUNT, Arg ) >
{
public:
    typedef ReturnType (Object::*FunctionType)( GEN_PARAM( FUNCTION_PARAM_COUNT, Arg ) ) const;

public:
    explicit MEMBER_FUNCTION_CONST_NAME(Object& obj, FunctionType func)
        : m_obj(&obj) 
        , m_memFunc(func)
    {
    }

public:
    virtual ReturnType Call( GEN_PARAM_EX( FUNCTION_PARAM_COUNT, Arg, arg ) )
    {
        return (m_obj->*m_memFunc)( GEN_PARAM( FUNCTION_PARAM_COUNT, arg ) );
    }

    ReturnType operator()( GEN_PARAM_EX( FUNCTION_PARAM_COUNT, Arg, arg ) )
    {
        return Call( GEN_PARAM( FUNCTION_PARAM_COUNT, arg ) );
    }

    virtual BASE_FUNCTION_NAME* Clone()
    {
        return new MEMBER_FUNCTION_CONST_NAME(*m_obj, m_memFunc);
    }

    virtual bool IsEqual(BASE_FUNCTION_NAME* pBase) const
    {
        const MEMBER_FUNCTION_CONST_NAME* pOther = static_cast<const MEMBER_FUNCTION_CONST_NAME*>(pBase);
        return (m_obj == pOther->m_obj) && (m_memFunc == pOther->m_memFunc);
    }

private:
    Object*  m_obj;
    FunctionType    m_memFunc;
};


/// MemberFunctionEx
template < typename R, typename SharePtrObject, typename Object FUNCTION_COMMA GEN_PARAM( FUNCTION_PARAM_COUNT, typename Arg ) >
class MEMBER_FUNCTION_SHARE_PTR_NAME : public BASE_FUNCTION_NAME< R FUNCTION_COMMA GEN_PARAM( FUNCTION_PARAM_COUNT, Arg ) >
{
public:
    typedef ReturnType (Object::*FunctionType)( GEN_PARAM( FUNCTION_PARAM_COUNT, Arg ) );

public:
    explicit MEMBER_FUNCTION_SHARE_PTR_NAME(SharePtrObject& obj, FunctionType func)
        : m_obj(obj) 
        , m_memFunc(func)
    {
    }

public:
    virtual ReturnType Call( GEN_PARAM_EX( FUNCTION_PARAM_COUNT, Arg, arg ) )
    {
        return (m_obj.Get()->*m_memFunc)( GEN_PARAM( FUNCTION_PARAM_COUNT, arg ) );
    }

    ReturnType operator()( GEN_PARAM_EX( FUNCTION_PARAM_COUNT, Arg, arg ) )
    {
        return Call( GEN_PARAM( FUNCTION_PARAM_COUNT, arg ) );
    }

    virtual BASE_FUNCTION_NAME* Clone()
    {
        return new MEMBER_FUNCTION_SHARE_PTR_NAME(m_obj, m_memFunc);
    }

    virtual bool IsEqual(BASE_FUNCTION_NAME* pBase) const
    {
        const MEMBER_FUNCTION_SHARE_PTR_NAME* pOther = static_cast<const MEMBER_FUNCTION_SHARE_PTR_NAME*>(pBase);
        return (m_obj.Get() == pOther->m_obj.Get()) && (m_memFunc == pOther->m_memFunc);
    }

private:
    SharePtrObject  m_obj;
    FunctionType    m_memFunc;
};



/// MemberFunctionConstEx
template < typename R, typename SharePtrObject, typename Object FUNCTION_COMMA GEN_PARAM( FUNCTION_PARAM_COUNT, typename Arg ) >
class MEMBER_FUNCTION_CONST_SHARE_PTR_NAME : public BASE_FUNCTION_NAME< R FUNCTION_COMMA GEN_PARAM( FUNCTION_PARAM_COUNT, Arg ) >
{
public:
    typedef ReturnType (Object::*FunctionType)( GEN_PARAM( FUNCTION_PARAM_COUNT, Arg ) ) const;

public:
    explicit MEMBER_FUNCTION_CONST_SHARE_PTR_NAME(SharePtrObject& obj, FunctionType func)
        : m_obj(obj) 
        , m_memFunc(func)
    {
    }

public:
    virtual ReturnType Call( GEN_PARAM_EX( FUNCTION_PARAM_COUNT, Arg, arg ) )
    {
        return (m_obj.Get()->*m_memFunc)( GEN_PARAM( FUNCTION_PARAM_COUNT, arg ) );
    }

    ReturnType operator()( GEN_PARAM_EX( FUNCTION_PARAM_COUNT, Arg, arg ) )
    {
        return Call( GEN_PARAM( FUNCTION_PARAM_COUNT, arg ) );
    }

    virtual BASE_FUNCTION_NAME* Clone()
    {
        return new MEMBER_FUNCTION_CONST_SHARE_PTR_NAME(m_obj, m_memFunc);
    }

    virtual bool IsEqual(BASE_FUNCTION_NAME* pBase) const
    {
        const MEMBER_FUNCTION_CONST_SHARE_PTR_NAME* pOther = static_cast<const MEMBER_FUNCTION_CONST_SHARE_PTR_NAME*>(pBase);
        return (m_obj.Get() == pOther->m_obj.Get()) && (m_memFunc == pOther->m_memFunc);
    }

private:
    SharePtrObject     m_obj;
    FunctionType    m_memFunc;
};


/// FunctionWrapper 
template < typename Signature >
class FUNCTION_WRAPPER_NAME;

template < typename R FUNCTION_COMMA GEN_PARAM( FUNCTION_PARAM_COUNT, typename Arg ) >
class FUNCTION_WRAPPER_NAME < R ( GEN_PARAM (FUNCTION_PARAM_COUNT, Arg ) ) >
{
public:
    typedef BASE_FUNCTION_NAME< R FUNCTION_COMMA GEN_PARAM( FUNCTION_PARAM_COUNT, Arg ) > BaseType;

    // Constructor
    FUNCTION_WRAPPER_NAME(R (*pFunc)( GEN_PARAM (FUNCTION_PARAM_COUNT, Arg ) ))
        : m_pFuncBase( new POINTER_FUNCTION_NAME< R FUNCTION_COMMA GEN_PARAM (FUNCTION_PARAM_COUNT, Arg ) >(pFunc) )
    {}

    template <typename T, typename P>
    FUNCTION_WRAPPER_NAME(T& obj, R (P::*pMemFunc)( GEN_PARAM (FUNCTION_PARAM_COUNT, Arg ) ))
        : m_pFuncBase( new MEMBER_FUNCTION_NAME< R, T FUNCTION_COMMA GEN_PARAM (FUNCTION_PARAM_COUNT, Arg ) >(obj, pMemFunc))
    {}

    template <typename T, typename P>
    FUNCTION_WRAPPER_NAME(T& obj, R (P::*pMemFunc)( GEN_PARAM (FUNCTION_PARAM_COUNT, Arg ) ) const )
        : m_pFuncBase( new MEMBER_FUNCTION_CONST_NAME< R, T FUNCTION_COMMA GEN_PARAM (FUNCTION_PARAM_COUNT, Arg ) >(obj, pMemFunc))
    {}

    template <typename T, template <typename> class SharePtr>
    FUNCTION_WRAPPER_NAME(SharePtr<T>& obj, R (T::*pMemFunc)( GEN_PARAM (FUNCTION_PARAM_COUNT, Arg ) ))
        : m_pFuncBase( new MEMBER_FUNCTION_SHARE_PTR_NAME< R, SharePtr<T>, T FUNCTION_COMMA GEN_PARAM (FUNCTION_PARAM_COUNT, Arg ) >(obj, pMemFunc))
    {}

    template <typename T, template <typename> class SharePtr>
    FUNCTION_WRAPPER_NAME(SharePtr<T>& obj, R (T::*pMemFunc)( GEN_PARAM (FUNCTION_PARAM_COUNT, Arg ) ) const )
        : m_pFuncBase( new MEMBER_FUNCTION_CONST_SHARE_PTR_NAME< R, SharePtr<T>, T FUNCTION_COMMA GEN_PARAM (FUNCTION_PARAM_COUNT, Arg ) >(obj, pMemFunc))
    {}

    FUNCTION_WRAPPER_NAME(BaseType* pBase)
        : m_pFuncBase(pBase)
    {}

    FUNCTION_WRAPPER_NAME()
        : m_pFuncBase(NULL)
    {
    }

    FUNCTION_WRAPPER_NAME(const FUNCTION_WRAPPER_NAME& rhs)
    {
        m_pFuncBase = rhs.m_pFuncBase->Clone();
    }


    FUNCTION_WRAPPER_NAME& operator=(const FUNCTION_WRAPPER_NAME& rhs)
    {
        if (this != &rhs)
        {
            m_pFuncBase = rhs.m_pFuncBase->Clone();
        }
        return *this;
    }


    // Destructor
    virtual ~FUNCTION_WRAPPER_NAME()
    {
        BEATS_SAFE_DELETE(m_pFuncBase);
    }

public:

    R operator()( GEN_PARAM_EX( FUNCTION_PARAM_COUNT, Arg, arg ) )
    {
        return m_pFuncBase->Call( GEN_PARAM( FUNCTION_PARAM_COUNT, arg ) );
    }

    bool IsEqual(const FUNCTION_WRAPPER_NAME& other) const
    {
        return m_pFuncBase->IsEqual(other.m_pFuncBase);
    }

private:
    BaseType* m_pFuncBase;
};




/////////////////////////////////////////////////////////////////////////////////////

template< typename Signature >
struct PointerFunctionImpl< FUNCTION_PARAM_COUNT, Signature >
{
    typedef POINTER_FUNCTION_NAME 
        <
        typename FunctionTraits< Signature >::ReturnType FUNCTION_COMMA
        GEN_PARAM( FUNCTION_PARAM_COUNT, typename FunctionTraits< Signature >::ArgType )
        > Type;
};


template< typename Object, typename Signature >                                                                      
struct MemberFunctionImpl< FUNCTION_PARAM_COUNT, Object, Signature >                                                     
{                                                                                                   
    typedef MEMBER_FUNCTION_NAME
        < 
        typename FunctionTraits< Signature >::ReturnType,
        Object FUNCTION_COMMA                                
        GEN_PARAM( FUNCTION_PARAM_COUNT, typename FunctionTraits< Signature >::ArgType )
        > Type;                                                                                     
};

template< typename Object, template <typename> class SharedPtr, typename Signature >                                                                      
struct MemberFunctionImpl< FUNCTION_PARAM_COUNT, SharedPtr<Object>, Signature >                                                     
{                                                                                                   
    typedef MEMBER_FUNCTION_SHARE_PTR_NAME
        < 
        typename FunctionTraits< Signature >::ReturnType,
        SharedPtr<Object>,
        Object FUNCTION_COMMA                                
        GEN_PARAM( FUNCTION_PARAM_COUNT, typename FunctionTraits< Signature >::ArgType )
        > Type;                                                                                     
};

template< typename Object, typename Signature >                                                                      
struct MemberFunctionConstImpl< FUNCTION_PARAM_COUNT, Object, Signature >                                                     
{                                                                                                   
    typedef MEMBER_FUNCTION_CONST_NAME
        < 
        typename FunctionTraits< Signature >::ReturnType,
        Object FUNCTION_COMMA                                
        GEN_PARAM( FUNCTION_PARAM_COUNT, typename FunctionTraits< Signature >::ArgType )
        > Type;                                                                                     
};


template< typename Object, template <typename> class SharedPtr, typename Signature >                                                                      
struct MemberFunctionConstImpl< FUNCTION_PARAM_COUNT, SharedPtr<Object>, Signature >                                                     
{                                                                                                   
    typedef MEMBER_FUNCTION_CONST_SHARE_PTR_NAME
        < 
        typename FunctionTraits< Signature >::ReturnType,
        SharedPtr<Object>,
        Object FUNCTION_COMMA                                
        GEN_PARAM( FUNCTION_PARAM_COUNT, typename FunctionTraits< Signature >::ArgType )
        > Type;                                                                                     
};



template< typename Signature >                                                                      
struct FunctionWrapperImpl< FUNCTION_PARAM_COUNT, Signature >                                                     
{                                                                                                   
    typedef FUNCTION_WRAPPER_NAME
        < 
        Signature
        > Type;                                                                                     
};

#undef FUNCTION_BASE_NAME
#undef POINTER_FUNCTION_NAME
#undef MEMBER_FUNCTION_NAME
#undef MEMBER_FUNCTION_CONST_NAME
#undef MEMBER_FUNCTION_SHARE_PTR_NAME
#undef MEMBER_FUNCTION_CONST_SHARE_PTR_NAME
#undef FUNCTION_WRAPPER_NAME
#undef FUNCTION_COMMA