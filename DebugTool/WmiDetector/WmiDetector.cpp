#include "stdafx.h"
#include <comdef.h>
#include <comutil.h>
#include "WmiDetector.h"
#include "StringHelper\StringHelper.h"
#pragma comment(lib,"WbemUuid.lib")  

CWmiDetector* CWmiDetector::m_pInstance = NULL;

CWmiDetector::CWmiDetector()
: m_pEnumClsObj(NULL)
, m_pWbemClsObj(NULL)  
, m_pWbemSvc(NULL)
, m_pWbemLoc(NULL)
, m_comInitByOtherModule(false)
{
    init();
}

CWmiDetector::~CWmiDetector()
{
    Uninit();
}

void CWmiDetector::init()
{
    HRESULT hr;
    //1. Init COM 
    hr=::CoInitialize(0);
    m_comInitByOtherModule = !SUCCEEDED(hr);
    if (SUCCEEDED(hr) || RPC_E_CHANGED_MODE == hr)  
    {
        //Set the security of process£¬you have to do this after CoInitializeEx, or OS will treat it as virus  
        hr=CoInitializeSecurity(NULL,  
            -1,  
            NULL,                     
            NULL,  
            RPC_C_AUTHN_LEVEL_PKT,  
            RPC_C_IMP_LEVEL_IMPERSONATE,  
            NULL,  
            EOAC_NONE,  
            NULL);  
        BEATS_ASSERT(SUCCEEDED(hr), _T("Init WMI failed!"));
        //2. Create a WMI connection
        // Create a CLSID_WbemLocator object. 
        hr=CoCreateInstance(CLSID_WbemLocator,  
            0,  
            CLSCTX_INPROC_SERVER,  
            IID_IWbemLocator,  
            (LPVOID*)&m_pWbemLoc);  
        BEATS_ASSERT(SUCCEEDED(hr), _T("Init WMI failed!"));

        //connect m_pWbemLoc to "root\cimv2", assign the m_pWbemSvc
        hr=m_pWbemLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"),  
            NULL,  
            NULL,  
            0,  
            NULL,  
            0,  
            0,  
            &m_pWbemSvc);  
        BEATS_ASSERT(SUCCEEDED(hr), _T("Init WMI failed!"));

        //3. Set WMI connection security  
        hr=CoSetProxyBlanket(m_pWbemSvc,  
            RPC_C_AUTHN_WINNT,  
            RPC_C_AUTHZ_NONE,  
            NULL,  
            RPC_C_AUTHN_LEVEL_CALL,  
            RPC_C_IMP_LEVEL_IMPERSONATE,  
            NULL,  
            EOAC_NONE);
        BEATS_ASSERT(SUCCEEDED(hr), _T("Init WMI failed!"));
    }  
}

void CWmiDetector::Uninit()
{
    HRESULT hr;
    if (NULL != m_pWbemSvc)  
    {  
        hr=m_pWbemSvc->Release();
        BEATS_ASSERT(SUCCEEDED(hr), _T("Release WMI failed!"));
    }  
    if (NULL != m_pWbemLoc)  
    {  
        hr=m_pWbemLoc->Release();
        BEATS_ASSERT(SUCCEEDED(hr), _T("Release WMI failed!"));
    }  
    if (NULL != m_pEnumClsObj)  
    {  
        hr=m_pEnumClsObj->Release();
        BEATS_ASSERT(SUCCEEDED(hr), _T("Release WMI failed!"));
    }  

    if (!m_comInitByOtherModule)
    {
        ::CoUninitialize(); 
    }
}

bool CWmiDetector::GetItemInfo( const TString& ClassName, const TString& ClassMember, std::vector<TString>& chRetValue)
{
    _bstr_t query("SELECT * FROM ");  
    VARIANT vtProp;  
    ULONG uReturn;  
    HRESULT hr;  
    bool result = false;  

    if (NULL != m_pWbemSvc)  
    {
        //query by the className, store to m_pEnumClsObj
        query+=_bstr_t(ClassName.c_str());  
        hr=m_pWbemSvc->ExecQuery(_bstr_t("WQL"),query,WBEM_FLAG_FORWARD_ONLY|WBEM_FLAG_RETURN_IMMEDIATELY,  
            0,&m_pEnumClsObj);  
        if (SUCCEEDED(hr))  
        {
            VariantInit(&vtProp);  
            uReturn=0;  

            hr=m_pEnumClsObj->Next(WBEM_INFINITE,1,&m_pWbemClsObj,&uReturn);
            while(SUCCEEDED(hr)&&uReturn>0)   
            { 
                hr=m_pWbemClsObj->Get(_bstr_t(ClassMember.c_str()),0,&vtProp,0,0);  
                if (SUCCEEDED(hr))  
                {  
                    TString resultValue;
                    VariantToString(&vtProp,resultValue);
                    chRetValue.push_back(resultValue);
                    VariantClear(&vtProp);
                    result = true;  
                }
                hr=m_pEnumClsObj->Next(WBEM_INFINITE,1,&m_pWbemClsObj,&uReturn);
            }  
        }  
    }  
    if(NULL != m_pEnumClsObj)   
    {  
        hr=m_pEnumClsObj->Release();  
        m_pEnumClsObj = NULL;  
    }  
    if(NULL != m_pWbemClsObj)   
    {  
        hr=m_pWbemClsObj->Release();  
        m_pWbemClsObj = NULL;  
    }
    return result;  
}

bool CWmiDetector::GetItemInfo( const TString& className, const TString& itemName, TString& result)
{
    bool ret = false;
    std::vector<TString> resultVec;
    GetItemInfo(className, itemName, resultVec);
    if (resultVec.size() > 0)
    {
        result.assign(resultVec.front().c_str());
        ret = true;
    }
    return ret;
}


void CWmiDetector::VariantToString(const LPVARIANT pVar,TString &chRetValue) const  
{  
    _bstr_t HUGEP* pBstr;
    BYTE HUGEP* pBuf;
    LONG low,high,i;
    HRESULT hr;
    bool isU32Value = false;
    int u32Value = 0;

    switch(pVar->vt)
    {  
    case VT_BSTR:
        {
            TCHAR szBuffer[10240];
            WideCharToMultiByte(CP_UTF8, 0, (const wchar_t*)pVar->pbstrVal, -1, szBuffer, (int)10240, NULL, NULL);
            chRetValue.assign(szBuffer);
            //delete space suffix
            chRetValue = chRetValue.substr(0, chRetValue.find_last_not_of(' ') + 1);
        }  
        break;  
    case VT_BOOL:  
        {  
            chRetValue.assign(VARIANT_TRUE==pVar->boolVal ? _T("ÊÇ") : _T("·ñ"));  
        }  
        break;  
    case VT_I4:  
        {  
            isU32Value = true;
            u32Value = pVar->llVal;
        }  
        break;  
    case VT_UI1:  
        {  
            isU32Value = true;
            u32Value = pVar->bVal;
        }  
        break;  
    case VT_UI4:  
        {  
            isU32Value = true;
            u32Value = pVar->ulVal;
        }  
        break;

    case VT_BSTR|VT_ARRAY:
        {  
            hr = SafeArrayAccessData(pVar->parray,(void HUGEP**)&pBstr);
            hr = SafeArrayUnaccessData(pVar->parray);
            TCHAR szBuffer[10240];
            WideCharToMultiByte(CP_UTF8, 0, (const wchar_t*)pBstr->GetBSTR(), -1, szBuffer, (int)10240, NULL, NULL);
            chRetValue.assign(szBuffer);
        }
        break;

    case VT_I4|VT_ARRAY:
        {  
            SafeArrayGetLBound(pVar->parray,1,&low);
            SafeArrayGetUBound(pVar->parray,1,&high);

            hr=SafeArrayAccessData(pVar->parray,(void HUGEP**)&pBuf);
            hr=SafeArrayUnaccessData(pVar->parray);
            TString strTmp;
            high=min(high,MAX_PATH*2-1);
            for(i=low;i<=high;++i)
            {  
                TCHAR cache[32];
                _stprintf_s(cache, _T("%02X"), pBuf[i]);
                chRetValue.append(cache);
            }
        }
        break;
    default:
        break;
    }
    if (isU32Value)
    {
        TCHAR cache[32];
        _stprintf_s(cache, _T("%d"), u32Value);
        chRetValue.assign(cache);
    }
}

void CWmiDetector::GetSMBiosData( LPBYTE pData, uint32_t& length )
{
    HRESULT hr=m_pWbemLoc->ConnectServer(_bstr_t(L"ROOT\\WMI"),  
        NULL,  
        NULL,  
        0,  
        NULL,  
        0,  
        0,  
        &m_pWbemSvc);  
    BEATS_ASSERT(SUCCEEDED(hr), _T("Init WMI failed!"));

    VARIANT vtProp;  
    ULONG uReturn;  

    if (NULL != m_pWbemSvc)  
    {
        hr=m_pWbemSvc->ExecQuery(_bstr_t("WQL"),_bstr_t("SELECT * FROM MSSMBios_RawSMBiosTables"),WBEM_FLAG_FORWARD_ONLY|WBEM_FLAG_RETURN_IMMEDIATELY,  
            0,&m_pEnumClsObj);  
        if (SUCCEEDED(hr))  
        {
            VariantInit(&vtProp);  
            uReturn=0;  

            hr=m_pEnumClsObj->Next(WBEM_INFINITE,1,&m_pWbemClsObj,&uReturn);
            if(SUCCEEDED(hr)&&uReturn>0)   
            { 
                hr=m_pWbemClsObj->Get(_bstr_t("SMBiosData"),0,&vtProp,0,0);  
                if (SUCCEEDED(hr))  
                {
                    LONG low,high;
                    SafeArrayGetLBound(vtProp.parray, 1, &low);   
                    SafeArrayGetUBound(vtProp.parray, 1, &high);  
                    length = high - low + 1;
                    BYTE HUGEP* pBuf;
                    hr=SafeArrayAccessData(vtProp.parray,(void HUGEP**)&pBuf);
                    hr=SafeArrayUnaccessData(vtProp.parray);
                    memcpy(pData, pBuf, length);
                }  
            }  
        }  
    }

    if(NULL != m_pEnumClsObj)   
    {  
        hr=m_pEnumClsObj->Release();  
        m_pEnumClsObj = NULL;  
    }  
    if(NULL != m_pWbemClsObj)   
    {  
        hr=m_pWbemClsObj->Release();  
        m_pWbemClsObj = NULL;  
    }  

    hr=m_pWbemLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"),  
        NULL,  
        NULL,  
        0,  
        NULL,  
        0,  
        0,  
        &m_pWbemSvc);  
    BEATS_ASSERT(SUCCEEDED(hr), _T("Init WMI failed!"));
}

void CWmiDetector::GetHDDSMARTData(LPBYTE pData, uint32_t& length)
{
    HRESULT hr=m_pWbemLoc->ConnectServer(_bstr_t(L"ROOT\\WMI"),  
        NULL,  
        NULL,  
        0,  
        NULL,  
        0,  
        0,  
        &m_pWbemSvc);  
    BEATS_ASSERT(SUCCEEDED(hr), _T("Init WMI failed!"));

    VARIANT vtProp;  
    ULONG uReturn;  

    if (NULL != m_pWbemSvc)  
    {
        hr=m_pWbemSvc->ExecQuery(_bstr_t("WQL"),_bstr_t("SELECT * FROM MSStorageDriver_ATAPISmartData"),WBEM_FLAG_FORWARD_ONLY|WBEM_FLAG_RETURN_IMMEDIATELY,  
            0,&m_pEnumClsObj);  
        if (SUCCEEDED(hr))  
        {
            VariantInit(&vtProp);  
            uReturn=0;  

            hr=m_pEnumClsObj->Next(WBEM_INFINITE,1,&m_pWbemClsObj,&uReturn);
            if(SUCCEEDED(hr)&&uReturn>0)   
            { 
                hr=m_pWbemClsObj->Get(_bstr_t("VendorSpecific"),0,&vtProp,0,0);  
                if (SUCCEEDED(hr))  
                {
                    LONG low,high;
                    SafeArrayGetLBound(vtProp.parray, 1, &low);   
                    SafeArrayGetUBound(vtProp.parray, 1, &high);  
                    length = high - low + 1;
                    BYTE HUGEP* pBuf;
                    hr=SafeArrayAccessData(vtProp.parray,(void HUGEP**)&pBuf);
                    hr=SafeArrayUnaccessData(vtProp.parray);
                    memcpy(pData, pBuf, length);
                }  
            }  
        }  
    }

    if(NULL != m_pEnumClsObj)   
    {  
        hr=m_pEnumClsObj->Release();  
        m_pEnumClsObj = NULL;  
    }  
    if(NULL != m_pWbemClsObj)   
    {  
        hr=m_pWbemClsObj->Release();  
        m_pWbemClsObj = NULL;  
    }  

    hr=m_pWbemLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"),  
        NULL,  
        NULL,  
        0,  
        NULL,  
        0,  
        0,  
        &m_pWbemSvc);  
    BEATS_ASSERT(SUCCEEDED(hr), _T("Init WMI failed!"));
}


bool CWmiDetector::GetFirmwareTables(const LPBYTE biosData, const uint32_t biosDataLength, const EFirmwareTableType firmType, std::vector<SFirmwareTables>& out)
{
    out.clear();
    uint32_t index = 0;
    bool ret = false;
    while (index < biosDataLength)
    {
        //Get Formatted section length
        uint32_t formattedSecLen = 0;
        uint32_t type = biosData[index];
        if (( type >=0) && (type <= 127))
        {
            formattedSecLen = biosData[index + 1];
        }
        //Get UnFormatted section length
        uint32_t unformattedSecLen = 0;
        uint32_t checkPos = index + biosData[index + 1];
        while (checkPos < biosDataLength - 1)
        {
            if ((biosData[checkPos] == 0) && (biosData[checkPos + 1] == 0))
            {
                unformattedSecLen = checkPos - index - formattedSecLen + 2;
                break;
            }
            ++checkPos;
        }
        uint32_t curTableLength = unformattedSecLen + formattedSecLen;
        if (curTableLength > 0)
        {
            if ((biosData[index] == firmType))
            {
                out.push_back(SFirmwareTables(firmType, index, curTableLength));
                ret = true;
            }                
            index += curTableLength;
        }
    }
    return ret;
}

bool CWmiDetector::GetStrFromFirmwareTables(uint32_t targetId, unsigned char* tablesBuff, TString& out )
{
    const uint32_t MAX_STRING = 0x3E8; //1K
    const uint32_t MAX_STRING_TABLE = 0x19; //25
    out.clear();
    char array_of_string_entries[MAX_STRING_TABLE][MAX_STRING];
    int index = (int)*(tablesBuff+1);
    int id=0;
    int still_more_data = *(tablesBuff+index) + *(tablesBuff+index+1);
    while (still_more_data)
    {
        int start_of_this_string = index;
        while (*(tablesBuff+index)!=0)
        {
            index++;
        }
        int len = index - start_of_this_string;
        index++;
        if (len > 0)
        {
            memcpy(array_of_string_entries[id],tablesBuff+start_of_this_string,len+1);
        }
        id++;
        still_more_data = *(tablesBuff+index) + *(tablesBuff+index+1);
    }
    targetId -= 1;
    if (targetId < (uint32_t)id)
    {
#ifdef _UNICODE
        TCHAR temp[256] = {0};
        MultiByteToWideChar(CP_ACP, MB_COMPOSITE, array_of_string_entries[targetId], strlen(array_of_string_entries[targetId]), temp, 256);
        out.assign(temp);
#else
        out.assign(array_of_string_entries[targetId]);
#endif
    }
    return true;
}


void CWmiDetector::DebugEnumClassProperty(const TCHAR* pNameSpace, const TString& ClassName )
{
    m_pWbemLoc->ConnectServer(_bstr_t(pNameSpace),  
        NULL,  
        NULL,  
        0,  
        NULL,  
        0,  
        0,  
        &m_pWbemSvc);  

    OutputDebugString(_T("\n********************Enum"));
    OutputDebugString(ClassName.c_str());
    OutputDebugString(_T(" Start******************** \n"));
    _bstr_t query("SELECT * FROM ");  
    VARIANT vtProp;  
    HRESULT hr;  
    ULONG uReturn;
    if (NULL != m_pWbemSvc)  
    {
        //query by the className, store to m_pEnumClsObj
        query+=_bstr_t(ClassName.c_str());
        BSTR propName;
        long valType; 
        hr=m_pWbemSvc->ExecQuery(_bstr_t("WQL"),query,WBEM_FLAG_FORWARD_ONLY|WBEM_FLAG_RETURN_IMMEDIATELY,  
            0,&m_pEnumClsObj);  
        if (SUCCEEDED(hr))  
        {
            VariantInit(&vtProp);  
            uReturn=0;  

            hr=m_pEnumClsObj->Next(WBEM_INFINITE,1,&m_pWbemClsObj,&uReturn);
            if(SUCCEEDED(hr)&&uReturn>0)   
            { 
                m_pWbemClsObj->BeginEnumeration(0);
                do 
                {
                    hr = m_pWbemClsObj->Next(0, &propName, &vtProp, &valType, NULL);
                    if (hr == WBEM_S_NO_MORE_DATA)
                    {
                        break;
                    }
                    TString result;
                    VariantToString(&vtProp, result);
                    TCHAR szBuffer[10240];
                    WideCharToMultiByte(CP_UTF8, 0, (const wchar_t*)propName, -1, szBuffer, (int)10240, NULL, NULL);
                    OutputDebugString(szBuffer);
                    OutputDebugString(_T(":\n"));
                    OutputDebugString(result.c_str());
                    OutputDebugString(_T("\n"));

                } while (hr == WBEM_S_NO_ERROR);
                m_pWbemClsObj->EndEnumeration();
            }
        }  
        if(NULL != m_pEnumClsObj)   
        {  
            hr=m_pEnumClsObj->Release();  
            m_pEnumClsObj = NULL;  
        }  
        if(NULL != m_pWbemClsObj)   
        {  
            hr=m_pWbemClsObj->Release();  
            m_pWbemClsObj = NULL;  
        }
    }
}