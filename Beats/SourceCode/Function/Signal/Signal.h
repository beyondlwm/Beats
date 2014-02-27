#ifndef BEATS_FUNCTION_SIGNAL_SIGNAL_H__INCLUDE
#define BEATS_FUNCTION_SIGNAL_SIGNAL_H__INCLUDE

#include "../../Function/ParamMacro.h"
#include "../../Function/Function.h"


//////////////////////////
// Forward Declaration
template< int ParamCount, typename Signature >
struct SignalImpl;

template< typename Signature >
class Signal : public SignalImpl< FunctionTraits< Signature >::ParamCount, Signature >::Type
{
};


#define SIGNAL_PARAM_COUNT 0
#include "Signal.hpp"
#undef SIGNAL_PARAM_COUNT

#define SIGNAL_PARAM_COUNT 1
#include "Signal.hpp"
#undef SIGNAL_PARAM_COUNT

#define SIGNAL_PARAM_COUNT 2
#include "Signal.hpp"
#undef SIGNAL_PARAM_COUNT

#define SIGNAL_PARAM_COUNT 3
#include "Signal.hpp"
#undef SIGNAL_PARAM_COUNT

#define SIGNAL_PARAM_COUNT 4
#include "Signal.hpp"
#undef SIGNAL_PARAM_COUNT

#endif