#include "stdafx.h"
#include "Utility/BeatsUtility/ComponentSystem/ComponentPublic.h"
#include "Utility/BeatsUtility/ComponentSystem/Component/ComponentManager.h"
#include "Utility/BeatsUtility/Serializer.h"
#include "Render/Model.h"
#include "Render/Skeleton.h"

void ForceReferenceSymbolForComponent()
{
    // When you are writing in a lib project, this cpp may not be referenced by the exe project.
    // so we add this empty function and make it extern in componentpublic.h, every cpp in the exe project,
    // which contain the componentpublic.h file will reference this symbol.
}

START_REGISTER_COMPONET
REGISTER_COMOPNENT(CModel, _T("Ä£ÐÍ"), _T("Render\\CModel"))
REGISTER_COMOPNENT(CSkeleton, _T("¹Ç÷À"), _T("Render\\CSkeleton"))
END_REGISTER_COMPONET

