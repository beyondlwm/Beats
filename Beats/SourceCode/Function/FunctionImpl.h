#ifndef BEATS_FUNCTION_FUNCTIONIMPL_H__INCLUDE
#define BEATS_FUNCTION_FUNCTIONIMPL_H__INCLUDE

#include "FunctionTraits.h"

//////////////////////////
// Forward Declaration

template< int ParamCount, typename Signature >
struct FunctionWrapperImpl;

template< int ParamCount, typename Signature >
struct PointerFunctionImpl;

template< int ParamCount, typename Object, typename Signature >
struct MemberFunctionImpl;

template< int ParamCount, typename Object, typename Signature >
struct MemberFunctionConstImpl;



#define FUNCTION_PARAM_COUNT 0
#include "FunctionImpl.hpp"
#undef FUNCTION_PARAM_COUNT

#define FUNCTION_PARAM_COUNT 1
#include "FunctionImpl.hpp"
#undef FUNCTION_PARAM_COUNT

#define FUNCTION_PARAM_COUNT 2
#include "FunctionImpl.hpp"
#undef FUNCTION_PARAM_COUNT

#define FUNCTION_PARAM_COUNT 3
#include "FunctionImpl.hpp"
#undef FUNCTION_PARAM_COUNT

#define FUNCTION_PARAM_COUNT 4
#include "FunctionImpl.hpp"
#undef FUNCTION_PARAM_COUNT

#endif