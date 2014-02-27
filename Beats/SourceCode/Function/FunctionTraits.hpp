


#if FUNCTION_PARAM_COUNT == 0
#define FUNCTION_COMMA
#else
#define FUNCTION_COMMA ,
#endif


// Function type [ int (), int (int) ]
template < typename Return FUNCTION_COMMA GEN_PARAM( FUNCTION_PARAM_COUNT, typename Arg) >
struct FunctionTraitsImpl< Return ( GEN_PARAM( FUNCTION_PARAM_COUNT, Arg ) ) >
{
    enum {ParamCount = FUNCTION_PARAM_COUNT};
    typedef Return ReturnType;
    GEN_PARAM_EX2( FUNCTION_PARAM_COUNT, typedef Arg, ArgType );    
};



#undef FUNCTION_COMMA