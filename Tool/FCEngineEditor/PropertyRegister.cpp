#include "stdafx.h"
#include "BoolPropertyDescription.h"
#include "IntPropertyDescription.h"
#include "FloatPropertyDescription.h"
#include "DoublePropertyDescription.h"
#include "EnumPropertyDescription.h"
#include "ListPropertyDescription.h"
#include "PtrPropertyDescription.h"
#include "UIntPropertyDescription.h"
#include "StringPropertyDescription.h"
#include "ColorPropertyDescription.h"
#include "Vec2fPropertyDescription.h"
#include "Vec3fPropertyDescription.h"
#include "Vec4fPropertyDescription.h"
#include "MapPropertyDescription.h"

#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentManager.h"

    REGISTER_PROPERTY_DESC(ePT_Bool, CBoolPropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_Float, CFloatPropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_Double, CDoublePropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_Int, CIntPropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_Str, CStringPropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_UInt, CUIntPropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_Ptr, CPtrPropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_List, CListPropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_Enum, CEnumPropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_Color, CColorPropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_Vec2F, CVec2PropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_Vec3F, CVec3PropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_Vec4F, CVec4PropertyDescription)
    REGISTER_PROPERTY_DESC(ePT_Map, CMapPropertyDescription)

