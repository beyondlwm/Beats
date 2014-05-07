#include "stdafx.h"
#include "ComponentEditorProxy.h"
#include "../../Utility/Serializer/Serializer.h"
#include "../../Utility/TinyXML/tinyxml.h"
#include "../../Utility/StringHelper/StringHelper.h"
#include "../../Utility/IdManager/IdManager.h"
#include "../Property/PropertyDescriptionBase.h"
#include "../DependencyDescriptionLine.h"
#include "../Component/ComponentProxyManager.h"
#include "../Component/ComponentManager.h"
#include "../Component/ComponentProject.h"
#include "../DependencyDescription.h"
#include "ComponentGraphic.h"
#include <string>

CComponentEditorProxy::CComponentEditorProxy(CComponentGraphic* pGraphics)
: m_uGuid(0)
, m_uParentGuid(0)
, m_pGraphics(pGraphics)
, m_pDependenciesDescription(NULL)
, m_pBeConnectedDependencyLines(NULL)
, m_pHostComponent(NULL)
{
    BEATS_ASSERT(false, _T("Do never call this!"));
    m_pGraphics->SetOwner(this);
    m_pDependenciesDescription = new std::vector<CDependencyDescription*>;
    m_pBeConnectedDependencyLines = new std::vector<CDependencyDescriptionLine*>;
    m_pSerializeOrder = new std::vector<char>;
    m_pProperties = new std::vector<CPropertyDescriptionBase*>;
}

CComponentEditorProxy::CComponentEditorProxy(CComponentGraphic* pGraphics, size_t uGuid, size_t uParentGuid, const TCHAR* pszClassName)
: m_uGuid(uGuid)
, m_uParentGuid(uParentGuid)
, m_szClassName(pszClassName)
, m_pGraphics(pGraphics)
, m_pDependenciesDescription(NULL)
, m_pBeConnectedDependencyLines(NULL)
, m_pHostComponent(NULL)
{
    m_pDependenciesDescription = new std::vector<CDependencyDescription*>;
    m_pBeConnectedDependencyLines = new std::vector<CDependencyDescriptionLine*>;
    m_pSerializeOrder = new std::vector<char>;
    m_pProperties = new std::vector<CPropertyDescriptionBase*>;
    m_pGraphics->SetOwner(this);
}

CComponentEditorProxy::~CComponentEditorProxy()
{
    ClearProperty();
    BEATS_ASSERT(m_pHostComponent == NULL || m_pHostComponent->GetId() == GetId());
    if (GetId() != 0xFFFFFFFF)
    {
        CComponentManager::GetInstance()->DeleteComponent(m_pHostComponent);
    }
    for (size_t i = 0; i < m_pDependenciesDescription->size(); ++i)
    {
        BEATS_SAFE_DELETE((*m_pDependenciesDescription)[i]);
    }
    BEATS_SAFE_DELETE(m_pDependenciesDescription);

    // In CDependencyDescriptionLine's destructor function, it will reduce the number in m_pBeConnectedDependencyLines.
    while (m_pBeConnectedDependencyLines->size() > 0)
    {
        CDependencyDescriptionLine* pLine = (*m_pBeConnectedDependencyLines)[0];
        CComponentEditorProxy* pProxyNeedUpdate = pLine->GetOwnerDependency()->GetOwner();
        // Delete the line before update the proxy, or the proxy doesn't know this dependency is deleted!
        BEATS_SAFE_DELETE(pLine);
        if (pProxyNeedUpdate != NULL)
        {
            pProxyNeedUpdate->UpdateHostComponent();
        }
    }
    BEATS_SAFE_DELETE(m_pBeConnectedDependencyLines);

    BEATS_SAFE_DELETE(m_pProperties);
    BEATS_SAFE_DELETE(m_pSerializeOrder);
    BEATS_SAFE_DELETE(m_pGraphics);
}

void CComponentEditorProxy::Deserialize( CSerializer& serializer )
{
    size_t nPropertyCount = 0;
    size_t nDependencyCount = 0;
    serializer >> nPropertyCount;
    serializer >> nDependencyCount;
    size_t uPropertyCounter = 0;
    size_t uDependencyCounter = 0;
    while (uPropertyCounter + uDependencyCounter < nPropertyCount + nDependencyCount)
    {
        bool bIsPropertyOrDependencyData;
        serializer >> bIsPropertyOrDependencyData;
        m_pSerializeOrder->push_back(bIsPropertyOrDependencyData);
        if (bIsPropertyOrDependencyData)
        {
            EReflectPropertyType ePropertyType;
            serializer >> ePropertyType;
            CPropertyDescriptionBase* pPropertyBase = CComponentProxyManager::GetInstance()->CreateProperty(ePropertyType, &serializer);
            pPropertyBase->Deserialize(serializer);
            this->AddProperty(pPropertyBase);
            ++uPropertyCounter;
            BEATS_ASSERT(uPropertyCounter <= uPropertyCounter);
        }
        else
        {
            bool bIsList = false;
            serializer >> bIsList;
            EDependencyType type;
            serializer >> type;
            size_t uGuid;
            serializer >> uGuid;
            CDependencyDescription* pNewDependency = new CDependencyDescription(type, uGuid, this, this->GetDependencies().size(), bIsList);
            TCHAR* pName = NULL;
            TCHAR** pNameHolder = &pName;
            serializer.Read(pNameHolder);
            pNewDependency->SetDisplayName(pName);
            serializer.Read(pNameHolder);
            pNewDependency->SetVariableName(pName);
            ++uDependencyCounter;
            BEATS_ASSERT(uDependencyCounter <= nDependencyCount);
        }
    }
}

CComponentBase* CComponentEditorProxy::Clone(bool bCloneValue, CSerializer* /*pSerializer*/)
{
    CComponentEditorProxy* pNewInstance = new CComponentEditorProxy(m_pGraphics->Clone(), m_uGuid, m_uParentGuid, m_szClassName.c_str());
    pNewInstance->SetDisplayName(m_displayName.c_str());
    pNewInstance->SetCatalogName(m_catalogName.c_str());
    BEATS_ASSERT(pNewInstance->GetGuid() == GetGuid(), _T("Can't assign between two different type (0x%x and 0x%x) of Reflect Base!"), pNewInstance->GetGuid(), GetGuid());
    pNewInstance->ClearProperty();
    for (size_t i = 0; i < (*m_pProperties).size(); ++i)
    {
        CPropertyDescriptionBase* pNewProperty = (*m_pProperties)[i]->Clone(bCloneValue);
        pNewInstance->AddProperty(pNewProperty);
    }
    for (size_t i = 0; i < m_pDependenciesDescription->size(); ++i)
    {
        CDependencyDescription* pDependency = (*m_pDependenciesDescription)[i];
        CDependencyDescription* pNewDependency = new CDependencyDescription(pDependency->GetType(), pDependency->GetDependencyGuid(), pNewInstance, pNewInstance->GetDependencies().size(), pDependency->IsListType());
        pNewDependency->SetDisplayName(pDependency->GetDisplayName());
        pNewDependency->SetVariableName(pDependency->GetVariableName());
    }
    pNewInstance->m_pSerializeOrder->assign(m_pSerializeOrder->begin(), m_pSerializeOrder->end());
    pNewInstance->GetGraphics()->CaculateSize();

    if (m_pHostComponent != NULL)
    {
        // please do manual manage because we don't know the id yet! we will register host component after we get the id!
        pNewInstance->m_pHostComponent = CComponentManager::GetInstance()->CreateComponentByRef(m_pHostComponent, bCloneValue, true);
    }
    pNewInstance->Initialize();

    return pNewInstance;
}

void CComponentEditorProxy::Serialize( CSerializer& serializer, EValueType eValueType)
{
    size_t startPos = serializer.GetWritePos();
    size_t totalSize = 0;
    serializer << totalSize;
    serializer << GetGuid();
    serializer << GetId();
    size_t uPropertyCounter = 0;
    size_t uDepedencyCoutner = 0;
    for (size_t i = 0; i < m_pSerializeOrder->size(); ++i)
    {
        if ((*m_pSerializeOrder)[i] > 0)
        {
            (*m_pProperties)[uPropertyCounter]->Serialize(serializer, eValueType);
            ++uPropertyCounter;
        }
        else
        {
            CDependencyDescription* pDependencyDesc = (*m_pDependenciesDescription)[uDepedencyCoutner];
            BEATS_ASSERT(pDependencyDesc != NULL);
            size_t uLineCount = pDependencyDesc->GetDependencyLineCount();
            serializer << uLineCount;
            for (size_t j = 0; j < uLineCount; ++j)
            {
                CComponentBase* pConnectedComponent = pDependencyDesc->GetDependencyLine(j)->GetConnectedComponent();
                BEATS_ASSERT(pConnectedComponent != NULL);
                serializer << pConnectedComponent->GetId();
                serializer << pConnectedComponent->GetGuid();
            }
            ++uDepedencyCoutner;
        }
    }
    totalSize = serializer.GetWritePos() - startPos;
    serializer.SetWritePos(startPos);
    serializer << totalSize;
    serializer.SetWritePos(totalSize + startPos);
}

size_t CComponentEditorProxy::GetGuid() const
{
    return m_uGuid;
}

size_t CComponentEditorProxy::GetParentGuid()
{
    return m_uParentGuid;
}

const TCHAR* CComponentEditorProxy::GetClassStr() const
{
    return m_szClassName.c_str();
}

const TString& CComponentEditorProxy::GetDisplayName() const
{
    return m_displayName;
}

void CComponentEditorProxy::SetDisplayName(const TCHAR* pDisplayName)
{
    m_displayName.assign(pDisplayName);
}

const TString& CComponentEditorProxy::GetCatalogName() const
{
    return m_catalogName;
}

void CComponentEditorProxy::SetCatalogName(const TCHAR* pCatalogName)
{
    if (pCatalogName == NULL)
    {
        pCatalogName = _T("");
    }
    m_catalogName.assign(pCatalogName);
}

CComponentGraphic* CComponentEditorProxy::GetGraphics()
{
    return m_pGraphics;
}

void CComponentEditorProxy::SetHostComponent(CComponentBase* pComponent)
{
    m_pHostComponent = pComponent;
}

CComponentBase* CComponentEditorProxy::GetHostComponent() const
{
    return m_pHostComponent;
}

void CComponentEditorProxy::UpdateHostComponent()
{
    if (m_pHostComponent)
    {
        CSerializer serializer;
        Serialize(serializer, eVT_CurrentValue);
        size_t uTotalSize = 0;
        size_t uGuid = 0;
        size_t uId = 0;
        serializer >> uTotalSize;
        serializer >> uGuid;
        serializer >> uId;
        m_pHostComponent->ReflectData(serializer);
        CComponentManager::GetInstance()->ResolveDependency();
    }
}

void CComponentEditorProxy::SaveToXML( TiXmlElement* pNode, bool bSaveOnlyNoneNativePart/* = false*/)
{
    TiXmlElement* pInstanceElement = new TiXmlElement("Instance");
    pInstanceElement->SetAttribute("Id", (int)GetId());
    int posX = 0;
    int posY = 0;
    if (m_pGraphics)
    {
        m_pGraphics->GetPosition(&posX, &posY);
    }
    pInstanceElement->SetAttribute("PosX", posX);
    pInstanceElement->SetAttribute("PosY", posY);

    for (size_t i = 0; i < m_pProperties->size(); ++i)
    {
        if (!bSaveOnlyNoneNativePart || !(*m_pProperties)[i]->IsDataSame(true))
        {
            (*m_pProperties)[i]->SaveToXML(pInstanceElement);
        }
    }

    for (size_t i = 0; i < m_pDependenciesDescription->size(); ++i)
    {
        (*m_pDependenciesDescription)[i]->SaveToXML(pInstanceElement);
    }
    pNode->LinkEndChild(pInstanceElement);
}

void CComponentEditorProxy::LoadFromXML( TiXmlElement* pNode )
{
    const char* pFilePath = pNode->GetDocument()->Value();
    TCHAR szFilePath[MAX_PATH];
    CStringHelper::GetInstance()->ConvertToTCHAR(pFilePath, szFilePath, MAX_PATH);

    int x = 0;
    int y = 0;
    if (pNode->Attribute("PosX", &x) && pNode->Attribute("PosY", &y))
    {
        m_pGraphics->SetPosition(x, y);
    }
    std::map<TString, CPropertyDescriptionBase*> unInitializedproperties;
    for (size_t k = 0; k < m_pProperties->size(); ++k)
    {
        BEATS_ASSERT(unInitializedproperties.find((*m_pProperties)[k]->GetBasicInfo()->m_variableName) == unInitializedproperties.end());
        unInitializedproperties[(*m_pProperties)[k]->GetBasicInfo()->m_variableName] = (*m_pProperties)[k];
    }
    TiXmlElement* pVarElement = pNode->FirstChildElement("VariableNode");
    std::vector<TiXmlElement*> unUsedXMLVariableNode;
    CComponentProject* pProject = CComponentProxyManager::GetInstance()->GetProject();
    while (pVarElement != NULL)
    {
        EReflectPropertyType propertyType;
        pVarElement->Attribute("Type", (int*)&propertyType);
        const char* szVariableName = pVarElement->Attribute("Variable");
        BEATS_ASSERT(szVariableName != NULL);
        TCHAR szTCHARVariableName[MAX_PATH];
        CStringHelper::GetInstance()->ConvertToTCHAR(szVariableName, szTCHARVariableName, MAX_PATH);
        bool bNeedMaintain = true;
        std::map<TString, CPropertyDescriptionBase*>::iterator iter = unInitializedproperties.find(szTCHARVariableName);
        if (iter == unInitializedproperties.end())
        {
            TString strReplacePropertyName;
            bNeedMaintain = !pProject->GetReplacePropertyName(this->GetGuid(), szTCHARVariableName, strReplacePropertyName);
            if (!bNeedMaintain && 
                strReplacePropertyName.length() > 0)
            {
                iter = unInitializedproperties.find(strReplacePropertyName);
                BEATS_ASSERT(iter != unInitializedproperties.end());
            }
        }

        if (iter != unInitializedproperties.end() && iter->second->GetType() == propertyType)
        {
            iter->second->LoadFromXML(pVarElement);
            unInitializedproperties.erase(iter);
        }
        else if(bNeedMaintain)
        {
            unUsedXMLVariableNode.push_back(pVarElement);
        }
        pVarElement = pVarElement->NextSiblingElement("VariableNode");
    }

    // Run maintain logic.
    for (size_t i = 0; i < unUsedXMLVariableNode.size(); ++i)
    {
        EReflectPropertyType propertyType;
        unUsedXMLVariableNode[i]->Attribute("Type", (int*)&propertyType);
        const char* szVariableName = unUsedXMLVariableNode[i]->Attribute("Variable");

        TCHAR szTCHARVariableName[MAX_PATH];
        CStringHelper::GetInstance()->ConvertToTCHAR(szVariableName, szTCHARVariableName, MAX_PATH);

        std::vector<CPropertyDescriptionBase*> matchTypeProperties;
        for (std::map<TString, CPropertyDescriptionBase*>::iterator iter = unInitializedproperties.begin(); iter != unInitializedproperties.end(); ++iter)
        {
            if (iter->second->GetType() == propertyType)
            {
                matchTypeProperties.push_back(iter->second);
            }
        }
        if (matchTypeProperties.size() > 0)
        {
            TCHAR szInform[1024];
            _stprintf(szInform, _T("数据:%s (位于文件\n%s\n组件 %s GUID:0x%x)在当前版本中已经失效, 请联系开发人员维护该数据!\n遗弃数据请选\"是\"\n重定位数据请选\"否\"\n"),
                szTCHARVariableName,
                szFilePath,
                this->GetClassStr(),
                this->GetGuid());
            int iRet = MessageBox(NULL, szInform, _T("数据维护"), MB_YESNO);
            if (iRet == IDYES)
            {
                pProject->RegisterPropertyMaintainInfo(this->GetGuid(), szTCHARVariableName, _T(""));
            }
            else
            {
                for (size_t j = 0; j < matchTypeProperties.size(); )
                {
                    const TString& strVariableName = matchTypeProperties[j]->GetBasicInfo()->m_variableName;
                    _stprintf(szInform, _T("是否将 %s 定位到 %s?"), szTCHARVariableName, strVariableName.c_str());
                    TCHAR szTitle[MAX_PATH];
                    _stprintf(szTitle, _T("重定位数据 %d/%d"), j + 1, matchTypeProperties.size());
                    int iRet = MessageBox(NULL, szInform, szTitle, MB_YESNO);
                    if (iRet == IDYES)
                    {
                        matchTypeProperties[j]->LoadFromXML(unUsedXMLVariableNode[i]);
                        unInitializedproperties.erase(strVariableName);
                        pProject->RegisterPropertyMaintainInfo(this->GetGuid(), szTCHARVariableName, strVariableName);
                        break;
                    }
                    else
                    {
                        if (++j == matchTypeProperties.size())
                        {
                            j = 0;
                        }
                    }
                }
            }
        }
    }
    for (size_t i = 0; i < m_pDependenciesDescription->size(); ++i)
    {
        (*m_pDependenciesDescription)[i]->LoadFromXML(pNode);
    }
}

CDependencyDescription* CComponentEditorProxy::GetDependency(size_t index)
{
    BEATS_ASSERT(index < m_pDependenciesDescription->size());
    BEATS_ASSERT((*m_pDependenciesDescription)[index]->GetIndex() == index);
    return (*m_pDependenciesDescription)[index];
}

const std::vector<CDependencyDescription*>& CComponentEditorProxy::GetDependencies()
{
    return *m_pDependenciesDescription;
}

void CComponentEditorProxy::AddDependencyDescription(CDependencyDescription* pDependencyDesc )
{
    bool bExisting = false;
    if (pDependencyDesc != NULL)
    {
        for (size_t i = 0; i < m_pDependenciesDescription->size(); ++i)
        {
            if ((*m_pDependenciesDescription)[i] == NULL)
            {
                continue;
            }
            bExisting = _tcscmp((*m_pDependenciesDescription)[i]->GetVariableName(), pDependencyDesc->GetVariableName()) == 0;
            if (bExisting)
            {
                BEATS_ASSERT(false, _T("Component %s id %d got a reduplicated dependency declare of variable %s"), GetClassStr(), GetId(), pDependencyDesc->GetVariableName());
                break;
            }
        }
    }

    if (!bExisting)
    {
        if (pDependencyDesc != NULL)
        {
            BEATS_ASSERT(pDependencyDesc->GetOwner() == this);
        }
        m_pDependenciesDescription->push_back(pDependencyDesc);
    }
}

void CComponentEditorProxy::AddBeConnectedDependencyDescriptionLine( CDependencyDescriptionLine* pDependencyDescLine )
{
    BEATS_ASSERT(pDependencyDescLine != NULL);
    m_pBeConnectedDependencyLines->push_back(pDependencyDescLine);
}

void CComponentEditorProxy::RemoveBeConnectedDependencyDescriptionLine( CDependencyDescriptionLine* pDependencyDescLine )
{
    bool bRet = false;
    for (size_t i = 0; i < m_pBeConnectedDependencyLines->size(); ++i)
    {
        if (pDependencyDescLine == (*m_pBeConnectedDependencyLines)[i])
        {
            std::vector<CDependencyDescriptionLine*>::iterator iter = m_pBeConnectedDependencyLines->begin();
            advance(iter, i);
            m_pBeConnectedDependencyLines->erase(iter);
            bRet = true;
            break;
        }
    }
    BEATS_ASSERT(bRet);
}

const std::vector<CDependencyDescriptionLine*>& CComponentEditorProxy::GetBeConnectedDependencyLines()
{
    return *m_pBeConnectedDependencyLines;
}

void CComponentEditorProxy::AddProperty(CPropertyDescriptionBase* pProperty)
{
    if (pProperty != NULL)
    {
        BEATS_ASSERT(pProperty->GetOwner() == NULL);
        pProperty->SetOwner(this);
    }
    m_pProperties->push_back(pProperty);
}

void CComponentEditorProxy::ClearProperty()
{
    for (size_t i = 0; i < m_pProperties->size(); ++i)
    {
        BEATS_SAFE_DELETE((*m_pProperties)[i]);
    }
    m_pProperties->clear();
}

const std::vector<CPropertyDescriptionBase*>* CComponentEditorProxy::GetPropertyPool() const
{
    return m_pProperties;
}

void CComponentEditorProxy::Save()
{
    for (size_t i = 0; i < (*m_pProperties).size(); ++i)
    {
        (*m_pProperties)[i]->Save();
    }
}

void CComponentEditorProxy::SetId(size_t id)
{
    super::SetId(id);
    BEATS_ASSERT(m_pHostComponent->GetId() == 0xFFFFFFFF, _T("The host component has got an id already!"));
    if (m_pHostComponent != NULL)
    {
        m_pHostComponent->SetId(id);
        CComponentManager::GetInstance()->RegisterInstance(m_pHostComponent);
        CComponentManager::GetInstance()->GetIdManager()->ReserveId(id);
    }
}

void CComponentEditorProxy::Initialize()
{
    super::Initialize();
    for (size_t i = 0; i < (*m_pProperties).size(); ++i)
    {
        (*m_pProperties)[i]->Initialize();
    }
    if (m_pHostComponent != NULL)
    {
        m_pHostComponent->Initialize();
    }
}
