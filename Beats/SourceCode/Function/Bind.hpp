#if FUNCTION_PARAM_COUNT == 0
#define FUNCTION_COMMA
#else
#define FUNCTION_COMMA ,
#endif


/// Bine PointerFunction
template <typename R FUNCTION_COMMA GEN_PARAM( FUNCTION_PARAM_COUNT, typename Arg )>
inline PointerFunction<R ( GEN_PARAM( FUNCTION_PARAM_COUNT, Arg ))> Bind( R (*pFunc)( GEN_PARAM (FUNCTION_PARAM_COUNT, Arg ) ) )
{
    return PointerFunction<R ( GEN_PARAM( FUNCTION_PARAM_COUNT, Arg ))>(pFunc);
}

/// Bind MemberFunction
template <typename FirstObject, typename SecondObject, typename R FUNCTION_COMMA GEN_PARAM( FUNCTION_PARAM_COUNT, typename Arg ) >
inline MemberFunction< FirstObject, R ( GEN_PARAM( FUNCTION_PARAM_COUNT, Arg )) > Bind(FirstObject& obj, R (SecondObject::*pMemFunc)( GEN_PARAM( FUNCTION_PARAM_COUNT, Arg )) )
{
    return MemberFunction< FirstObject, R ( GEN_PARAM( FUNCTION_PARAM_COUNT, Arg )) >(obj, pMemFunc);
}

/// Bind const Member Function
template <typename FirstObject, typename SecondObject, typename R FUNCTION_COMMA GEN_PARAM( FUNCTION_PARAM_COUNT, typename Arg ) >
inline MemberFunctionConst< FirstObject, R ( GEN_PARAM( FUNCTION_PARAM_COUNT, Arg )) > Bind(FirstObject& obj, R (SecondObject::*pMemFunc)( GEN_PARAM( FUNCTION_PARAM_COUNT, Arg )) const )
{
    return MemberFunctionConst< FirstObject, R ( GEN_PARAM( FUNCTION_PARAM_COUNT, Arg )) >(obj, pMemFunc);
}



#undef FUNCTION_COMMA