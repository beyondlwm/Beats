#include "stdafx.h"
#include "BoolPropertyDescription.h"
#include "IntPropertyDescription.h"
#include "FloatPropertyDescription.h"
#include "DoublePropertyDescription.h"
#include "EnumPropertyDescription.h"
#include "ListPropertyDescriptionEx.h"
#include "PtrPropertyDescription.h"
#include "UIntPropertyDescription.h"
#include "StringPropertyDescription.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentManager.h"

REGISTER_PROPERTY_DESC(ePT_Bool, CBoolPropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_Float, CFloatPropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_Double, CDoublePropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_Int, CIntPropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_Str, CStringPropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_UInt, CUIntPropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_Ptr, CPtrPropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_List, CListPropertyDescriptionEx)
    REGISTER_PROPERTY_DESC(ePT_Enum, CEnumPropertyDescription)

