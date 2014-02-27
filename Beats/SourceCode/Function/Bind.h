#ifndef BEATS_FUNCTION_BIND_H__INCLUDE
#define BEATS_FUNCTION_BIND_H__INCLUDE

#include "Function.h"
#include "ParamMacro.h"



#define FUNCTION_PARAM_COUNT 0
#include "Bind.hpp"
#undef FUNCTION_PARAM_COUNT


#define FUNCTION_PARAM_COUNT 1
#include "Bind.hpp"
#undef FUNCTION_PARAM_COUNT


#define FUNCTION_PARAM_COUNT 2
#include "Bind.hpp"
#undef FUNCTION_PARAM_COUNT


#define FUNCTION_PARAM_COUNT 3
#include "Bind.hpp"
#undef FUNCTION_PARAM_COUNT


#define FUNCTION_PARAM_COUNT 4
#include "Bind.hpp"
#undef FUNCTION_PARAM_COUNT

#endif
