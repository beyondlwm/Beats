#include "stdafx.h"
#include "PropertyDescriptionBase.h"
#include "../../Utility/Serializer/Serializer.h"
#include "../../Utility/SharePtr/SharePtr.h"
#include "../Component/ComponentEditorProxy.h"

CPropertyDescriptionBase::CPropertyDescriptionBase(EPropertyType type)
: m_pOwner(NULL)
, m_pParent(NULL)
, m_pChildren(new std::vector<CPropertyDescriptionBase*>())
, m_pBasicInfo(new SharePtr<SBasicPropertyInfo>(new SBasicPropertyInfo))
, m_type(type)
{
    for (size_t i = 0; i < eVT_Count; ++i)
    {
        m_valueArray[i] = NULL;
    }
}

CPropertyDescriptionBase::CPropertyDescriptionBase(const CPropertyDescriptionBase& rRef)
: m_pOwner(NULL)
, m_pParent(NULL)
, m_pChildren(new std::vector<CPropertyDescriptionBase*>())
, m_pBasicInfo(new SharePtr<SBasicPropertyInfo>(*rRef.m_pBasicInfo))
, m_type(rRef.GetType())
{
}

CPropertyDescriptionBase::~CPropertyDescriptionBase()
{
    BEATS_SAFE_DELETE(m_pBasicInfo);
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        BEATS_SAFE_DELETE((*m_pChildren)[i]);
    }
    BEATS_SAFE_DELETE(m_pChildren);
}

EPropertyType CPropertyDescriptionBase::GetType() const
{
    return m_type;
}

void CPropertyDescriptionBase::SetType(EPropertyType type)
{
    m_type = type;
}

CComponentEditorProxy* CPropertyDescriptionBase::GetOwner() const
{
    return m_pOwner;
}

void CPropertyDescriptionBase::SetOwner(CComponentEditorProxy* pOwner)
{
    m_pOwner = pOwner;
}

CPropertyDescriptionBase* CPropertyDescriptionBase::GetParent() const
{
    return m_pParent;
}

void CPropertyDescriptionBase::SetParent(CPropertyDescriptionBase* pParent)
{
    BEATS_ASSERT(m_pParent == NULL);
    m_pParent = pParent;
}

void CPropertyDescriptionBase::SetBasicInfo(const SBasicPropertyInfo& info)
{
    *m_pBasicInfo->Get() = info;
}

SBasicPropertyInfo& CPropertyDescriptionBase::GetBasicInfo() const
{
    return *m_pBasicInfo->Get();
}

bool CPropertyDescriptionBase::Deserialize( CSerializer& serializer )
{
    size_t startPos = serializer.GetReadPos();
    size_t dataLenghth = 0;
    serializer >> dataLenghth;
    DeserializeBasicInfo(serializer);
    TCHAR* pParameter = NULL;
    TCHAR** pParameterHolder = &pParameter;
    serializer.Read(pParameterHolder);
    this->AnalyseUIParameter(pParameter, true);
    size_t totalSize = serializer.GetReadPos() - startPos;
    BEATS_ASSERT(dataLenghth == totalSize, _T("Read data of property: %s in component: %d Failed, Data Length :%d while require %d"), (*m_pBasicInfo)->m_variableName.c_str(), m_pOwner->GetGuid(), dataLenghth, totalSize);
    serializer.SetReadPos(startPos + dataLenghth);
    return true;
}

bool CPropertyDescriptionBase::DeserializeBasicInfo(CSerializer& serializer)
{
    TCHAR* buff = NULL;
    TCHAR** pbuff = &buff;
    serializer >> (*m_pBasicInfo)->m_bEditable;
    serializer >> (*m_pBasicInfo)->m_color;
    serializer.Read(pbuff);
    (*m_pBasicInfo)->m_displayName.assign(*pbuff);
    serializer.Read(pbuff); 
    (*m_pBasicInfo)->m_catalog.assign(*pbuff);
    serializer.Read(pbuff);
    (*m_pBasicInfo)->m_tip.assign(*pbuff);
    serializer.Read(pbuff);
    (*m_pBasicInfo)->m_variableName.assign(*pbuff);

    return true;
}

void CPropertyDescriptionBase::AddChild(CPropertyDescriptionBase* pProperty)
{
#ifdef _DEBUG
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        BEATS_ASSERT((*m_pChildren)[i] != pProperty);
    }
#endif
    pProperty->m_pParent = this;
    m_pChildren->push_back(pProperty);
}

bool CPropertyDescriptionBase::DeleteChild(CPropertyDescriptionBase* pProperty, bool bKeepChildOrder/* = false*/)
{
    bool bRet = false;
    for (size_t i = 0; i < m_pChildren->size(); ++i)
    {
        if((*m_pChildren)[i] == pProperty)
        {
            bRet = true;
            if (bKeepChildOrder)
            {
                std::vector<CPropertyDescriptionBase*>::iterator iter = m_pChildren->begin();
                advance(iter, i);
                m_pChildren->erase(iter);
            }
            else
            {
                (*m_pChildren)[i] = m_pChildren->back();
                m_pChildren->pop_back();
            }
            break;
        }
    }
    BEATS_SAFE_DELETE(pProperty);
    return bRet;
}

CPropertyDescriptionBase* CPropertyDescriptionBase::GetChild(size_t i) const
{
    BEATS_ASSERT(i < m_pChildren->size());
    return (*m_pChildren)[i];
}

size_t CPropertyDescriptionBase::GetChildrenCount() const
{
    return m_pChildren->size();
}

std::vector<CPropertyDescriptionBase*>& CPropertyDescriptionBase::GetChildren()
{
    return *m_pChildren;
}

CPropertyDescriptionBase* CPropertyDescriptionBase::Clone(bool bCloneValue)
{
    CPropertyDescriptionBase* pNewProperty = this->CreateNewInstance();
    pNewProperty->SetValue(m_valueArray[eVT_DefaultValue], eVT_DefaultValue);
    pNewProperty->SetValue(bCloneValue ? m_valueArray[eVT_SavedValue] : m_valueArray[eVT_DefaultValue], eVT_SavedValue);
    pNewProperty->SetValue(bCloneValue ? m_valueArray[eVT_CurrentValue] : m_valueArray[eVT_DefaultValue], eVT_CurrentValue);
    return pNewProperty;
}

void* CPropertyDescriptionBase::GetValue( EValueType type ) const
{
    return m_valueArray[type];
}


void CPropertyDescriptionBase::Save()
{
    SetValue(m_valueArray[eVT_CurrentValue], eVT_SavedValue);
}

void CPropertyDescriptionBase::Initialize()
{

}