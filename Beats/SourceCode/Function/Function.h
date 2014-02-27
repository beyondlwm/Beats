#ifndef BEATS_FUNCTION_FUNCTION_H__INCLUDE
#define BEATS_FUNCTION_FUNCTION_H__INCLUDE

#include "FunctionImpl.h"
#include "FunctionTraits.h"


/// PointerFunction
template <typename Signature>
class PointerFunction : public PointerFunctionImpl< FunctionTraits<Signature>::ParamCount, Signature >::Type
{
public:

    typedef typename PointerFunctionImpl< FunctionTraits< Signature >::ParamCount, Signature >::Type BaseType;
    typedef typename BaseType::FunctionType FuncType;

    explicit PointerFunction( FuncType func )
        : BaseType(func)
    {}
};


/// MemberFunction
template <typename Object, typename Signature>
class MemberFunction : public MemberFunctionImpl< FunctionTraits<Signature>::ParamCount, Object, Signature >::Type
{
public:

    typedef typename MemberFunctionImpl< FunctionTraits< Signature >::ParamCount, Object, Signature >::Type BaseType;
    typedef typename BaseType::FunctionType FuncType;

    explicit MemberFunction( Object& obj, FuncType func )
        : BaseType(obj, func)
    {}
};


/// MemberFunctionConst
template <typename Object, typename Signature>
class MemberFunctionConst : public MemberFunctionConstImpl< FunctionTraits<Signature>::ParamCount, Object, Signature >::Type
{
public:

    typedef typename MemberFunctionConstImpl< FunctionTraits< Signature >::ParamCount, Object, Signature >::Type BaseType;
    typedef typename BaseType::FunctionType FuncType;

    explicit MemberFunctionConst( Object& obj, FuncType func )
        : BaseType(obj, func)
    {}
};



/// Function
template <typename Signature>
class Function : public FunctionWrapperImpl< FunctionTraits<Signature>::ParamCount, Signature >::Type
{
public:

    typedef typename FunctionWrapperImpl< FunctionTraits< Signature >::ParamCount, Signature >::Type BaseType;

    //default constructor for being a std::map value.
    Function()       
    {
    }

    template <typename T>
    explicit Function( const T& func )       
        : BaseType(func)
    {
    }

    template< typename Object, typename F >
    explicit Function( Object& obj, const F& func )
        : BaseType(obj, func)
    {
    }

    
    Function(PointerFunction<Signature>& ptrFunc)
        : BaseType(ptrFunc.Clone())
    {

    }

    template< typename Object >
    Function(MemberFunction<Object, Signature>& memFunc)
        : BaseType(memFunc.Clone())
    {

    }

    template< typename Object >
    Function(MemberFunctionConst<Object, Signature>& memFunc)
        : BaseType(memFunc.Clone())
    {

    }


    Function(const Function& rhs)
        : BaseType(rhs)
    {

    }

    Function& operator = (const Function& rhs)
    {
        if (this != &rhs)
        {
            BaseType::operator=(rhs);
        }
        return *this;
    }
};

/// Return true if two functions are the same function
template< typename Signature>
inline bool operator==( const Function< Signature >& lhs, const Function< Signature >& rhs )
{
    return lhs.IsEqual(rhs);
}

template< typename Signature>
inline bool operator!=( const Function< Signature >& lhs, const Function< Signature >& rhs )
{
    return !lhs.IsEqual(rhs);
}

template< typename Signature>
inline bool operator < ( const Function< Signature >& /*lhs*/, const Function< Signature >& /*rhs*/ )
{
    return false;
}

#endif