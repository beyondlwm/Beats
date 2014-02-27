#ifndef BEATS_FUNCTION_FUNCTIONTRAITS_H__INCLUDE
#define BEATS_FUNCTION_FUNCTIONTRAITS_H__INCLUDE

#include "ParamMacro.h"

//////////////////////////
// Forward Declaration
template <typename Function>
struct FunctionTraitsImpl;


template <typename Signature>
struct FunctionTraits : public FunctionTraitsImpl< Signature >
{
};


#define FUNCTION_PARAM_COUNT 0
#include "FunctionTraits.hpp"
#undef FUNCTION_PARAM_COUNT


#define FUNCTION_PARAM_COUNT 1
#include "FunctionTraits.hpp"
#undef FUNCTION_PARAM_COUNT


#define FUNCTION_PARAM_COUNT 2
#include "FunctionTraits.hpp"
#undef FUNCTION_PARAM_COUNT


#define FUNCTION_PARAM_COUNT 3
#include "FunctionTraits.hpp"
#undef FUNCTION_PARAM_COUNT


#define FUNCTION_PARAM_COUNT 4
#include "FunctionTraits.hpp"
#undef FUNCTION_PARAM_COUNT

#endif