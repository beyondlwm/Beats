#include "stdafx.h"
#include "SystemDetector.h"
#include "../WmiDetector/wmidetector.h"
#include "../DXVerDetector/DXVerDetector.h"
#include "SystemDetectorDef.h"
#include "../../Utility/Serializer/Serializer.h"
#include <devguid.h>
#include <Setupapi.h>
#pragma comment(lib, "setupapi.lib")

CSystemDetector* CSystemDetector::m_pInstance = NULL;

CSystemDetector::CSystemDetector()
{    

}

CSystemDetector::~CSystemDetector()
{
}

bool CSystemDetector::GetComputerModel(TString& result)
{
    result.clear();
    bool ret = CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_ComputerSystem"), _T("Manufacturer"), result);
    if (ret)
    {
        TString model;
        ret = CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_ComputerSystem"), _T("Model"), model);
        result.append(" " + model);
    }
    return ret;
}

bool CSystemDetector::GetMonitorInfo( TString& result )
{
    bool ret = false;
    result.clear();
    BYTE edidData[512];
    size_t dataLength = 512;
    bool edidValid = GetEDIDFromReg(edidData, dataLength);
    BEATS_ASSERT(edidValid, _T("Failed get edid from registry!"));
    if (edidValid)
    {
        //analyse EDID data, see reference about the structure of EDID
        long factoryId = (edidData[8] << 8) + edidData[9];
        const long LAST_FIVE_BIT = 31;// last five bit is 1
        char thirdChar = (factoryId & LAST_FIVE_BIT) + 'A' - 1;//get the last 5 bit
        char secondChar = ((factoryId>>5) & LAST_FIVE_BIT) + 'A' - 1;
        char firstChar = ((factoryId>>10) & LAST_FIVE_BIT) + 'A' - 1;
        long productId = (edidData[11] << 8) + edidData[10];
        long initDate = edidData[17] + 1990;
        long initWeek = edidData[16];
        long monitorWidth = edidData[21];
        long monitorHeight = edidData[22];
        TCHAR monitorInfoCache[256];

        //hard code to check if the number is valid: it's not possible to over 70cm for most of monitor.
        if (monitorHeight > 70 || monitorWidth > 70)
        {
            //maybe the edid value is not supported, so don't print wrong value.
            _stprintf_s(monitorInfoCache, _T("%c%c%c%.4X"),firstChar, secondChar, thirdChar, productId);
        }
        else
        {
            _stprintf_s(monitorInfoCache, _T("%c%c%c%.4X (宽%d厘米 x 高%d厘米) 出厂日期：%d年"),firstChar, secondChar, thirdChar, productId, monitorWidth, monitorHeight, initDate);
            if (initWeek > 0)
            {
                TCHAR weekTime[16];
                _stprintf_s(weekTime, _T("第%d周"), initWeek);
                _tcscat_s(monitorInfoCache, weekTime);
            }
        }
        result.assign(monitorInfoCache);
        ret = true;
    }
    return ret;
}

bool CSystemDetector::GetEDIDFromReg( void* pData, size_t& dataLength )
{
    HDEVINFO hDevInfo = NULL;
    bool result = false;
    hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_MONITOR, NULL, NULL, DIGCF_PRESENT);
    if(hDevInfo != INVALID_HANDLE_VALUE)
    {
        SP_DEVINFO_DATA spDevInfoData;
        spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
        for (int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &spDevInfoData) == TRUE; ++i)
        {
            HKEY hDevRegKey;
            hDevRegKey = SetupDiOpenDevRegKey(hDevInfo, &spDevInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_ALL_ACCESS);
            if (hDevRegKey != INVALID_HANDLE_VALUE)
            {
                TCHAR value[32];
                DWORD valueLength = 32;
                for (int index = 0; ERROR_NO_MORE_ITEMS != RegEnumValue(hDevRegKey, index, value, &valueLength, NULL, NULL, NULL, NULL); ++index)
                {
                    if (!_tcscmp(value, _T("EDID")))
                    {
                        LONG queryResult = RegQueryValueEx(hDevRegKey, _T("EDID"), NULL, NULL, (LPBYTE)pData, (LPDWORD)(&dataLength));
                        BEATS_ASSERT(queryResult == 0, _T("Query EDID failed!"));
                        result = true;
                        break;
                    }
                }
                RegCloseKey(hDevRegKey);
            }
        }
    }
    SetupDiDestroyDeviceInfoList(hDevInfo);

    return result;
}

bool CSystemDetector::GetOSName( TString& result )
{
    result.clear();
    bool ret = CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_OperatingSystem"), _T("Caption"), result);
    return ret;
}

bool CSystemDetector::GetOSVersion( TString& result )
{
    result.clear();
    TString version;
    bool ret = CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_OperatingSystem"), _T("Version"), version);
    result.append(version);
    ret = ret && CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_OperatingSystem"), _T("CSDVersion"), version);
    result.append(_T(" ")).append(version);
    ret = ret && CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_OperatingSystem"), _T("BuildNumber"), version);
    result.append(_T(" Build: ")).append(version);
    return ret;
}

bool CSystemDetector::GetGraphicsCardInfo( TString& result )
{
    result.clear();
    bool ret = CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_VideoController"), _T("VideoProcessor"), result);
    TString tempData;
    ret = ret && CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_VideoController"), _T("DriverVersion"), tempData);
    result.append(_T(" 驱动版本: ")).append(tempData);
    ret = ret && CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_VideoController"), _T("AdapterRAM"), tempData);
    long ramInBytes = _tstoi(tempData.c_str());
    size_t ramInMB = ramInBytes / (1024 * 1024);
    TCHAR cache[128];
    _stprintf_s(cache, _T("%d MB"), ramInMB);
    result.append(_T(" 显存: ")).append(cache);
    return ret;
}

bool CSystemDetector::GetProcessorInfo( TString& result )
{
    result.clear();
    bool ret = false;
    HDEVINFO hDevInfo = NULL;
    hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PROCESSOR, NULL, NULL, DIGCF_PRESENT);
    if(hDevInfo != INVALID_HANDLE_VALUE)
    {
        SP_DEVINFO_DATA spDevInfoData;
        spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
        for (int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &spDevInfoData) == TRUE; ++i)
        {
            DWORD buffersize = 1024;
            BYTE buffer[1024] = {0};
            ret = (TRUE == SetupDiGetDeviceRegistryProperty(hDevInfo, 
                &spDevInfoData,
                SPDRP_FRIENDLYNAME,
                NULL,
                (PBYTE)buffer,
                buffersize,
                &buffersize));
            if (ret)
            {
                result.append((TCHAR*)buffer);
                break;//only record one core.
            }
        }
    }
    TString temp;
    CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_Processor"), _T("ProcessorId"), temp);
    result.append(_T("\nCPUID:"));
    result.append(temp);

    return ret;
}

bool CSystemDetector::GetBaseBoardInfo( TString& result )
{
    result.clear();
    TString tempData;
    bool ret = CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_BaseBoard"), _T("Manufacturer"), result);
    ret = ret && CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_BaseBoard"), _T("Product"), tempData);
    result.append(_T(" ")).append(tempData.c_str());
    ret = ret && CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_BIOS"), _T("Manufacturer"), tempData);
    result.append(_T(" ")).append(tempData.c_str());
    ret = ret && CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_BIOS"), _T("SMBIOSBIOSVersion"), tempData);
    result.append(_T(" ")).append(tempData.c_str());
    return ret;
}

bool CSystemDetector::GetMemoryInfo( TString& result )
{
    result.clear();
    BYTE temp[10240] = {0};
    size_t length;
    CWmiDetector::GetInstance()->GetSMBiosData((LPBYTE)temp, length);
    std::vector<SFirmwareTables> tables;
    CWmiDetector::GetInstance()->GetFirmwareTables(temp, length, eFTT_MemDev, tables);

    //there will be lots of magic numbers below, to know more about the structure of FirmwareTable.
    char table[0x10000] = {0};//64k
    for (size_t i = 0; i < tables.size(); ++i)
    {
        //must copy it out so we can detect the end of the table in GetStrFromFirmwareTables.
        memcpy(table, &temp[tables[i].startPos], tables[i].length);
        const size_t sizeOffset = 0x0C;
        size_t memorySize = 0;
        memcpy(&memorySize, & table[sizeOffset], 2);
        if (memorySize > 0)
        {
            //if we got several memory disks, show info in multi-lines
            if (result.size() > 0)
            {
                result.append(_T("\n"));
            }
            TString tempString;
            const size_t manufactuerOffset = 0x17;
            CWmiDetector::GetInstance()->GetStrFromFirmwareTables(table[manufactuerOffset], (unsigned char*)table, tempString);
            //delete space suffix
            tempString = tempString.substr(0, tempString.find_last_not_of(' ') + 1);
            ConvertMemoryManufactuer(tempString, tempString);

            result.append(tempString);
            const size_t typeOffset = 0x12;
            size_t memoryType = table[typeOffset];
            ConvertMemoryType(memoryType, tempString);
            result.append(_T(" ")).append(tempString);
            TCHAR tempCache[128];
            _stprintf_s(tempCache, _T(" %d MB"), memorySize);
            result.append(tempCache);

            const size_t frequencyOffset = 0x15;
            size_t frequency = 0;
            memcpy(&frequency, & table[frequencyOffset], 2);
            _stprintf_s(tempCache, _T("%d MHz"), frequency);
            result.append(_T(" ")).append(tempCache);

            CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_OperatingSystem"), _T("FreePhysicalMemory"), tempString);

            _stprintf_s(tempCache, _T(" 当前可用: %s KB"), tempString.c_str());
            result.append(tempCache);
        }
    }

    return true;
}

void CSystemDetector::ConvertMemoryManufactuer( const TString& inCode, TString& outResult )
{
    if (inCode.compare(_T("02FE")) == 0)
    {
        outResult.assign(_T("尔必达"));
    }
    else if ((inCode.compare(_T("80CE")) == 0) || (inCode.compare(_T("Samsung")) == 0))
    {
        outResult.assign(_T("三星"));
    }
    else if (inCode.compare(_T("802C")) == 0)
    {
        outResult.assign(_T("镁光"));
    }
    else if ((inCode.compare(_T("0198")) == 0) || (inCode.compare(_T("Kingston")) == 0))
    {
        outResult.assign(_T("金士顿"));
    }
    else if (inCode.compare(_T("80AD")) == 0)
    {
        outResult.assign(_T("海力士"));
    }
    else if (inCode.compare(_T("830B")) == 0)
    {
        outResult.assign(_T("南亚"));
    }
    else if (inCode.compare(_T("Ramaxel")) == 0)
    {
        outResult.assign(_T("记忆科技"));
    }
    else
    {
        TCHAR tempCache[512];
        _stprintf_s(tempCache, _T("未知品牌：%s"), inCode.c_str());
        outResult.assign(tempCache);
    }    
}

void CSystemDetector::ConvertMemoryType( const size_t type, TString& outResult )
{
    switch (type)
    {
    case eMT_DRAM:
        outResult.assign(_T("DRAM"));
        break;
    case eMT_EDRAM:
        outResult.assign(_T("EDRAM"));
        break;
    case eMT_VRAM:
        outResult.assign(_T("VRAM"));
        break;
    case eMT_SRAM:
        outResult.assign(_T("SRAM"));
        break;
    case eMT_RAM:
        outResult.assign(_T("RAM"));
        break;
    case eMT_ROM:
        outResult.assign(_T("ROM"));
        break;
    case eMT_FLASH:
        outResult.assign(_T("FLASH"));
        break;
    case eMT_EEPROM:
        outResult.assign(_T("EEPROM"));
        break;
    case eMT_FEPROM:
        outResult.assign(_T("FEPROM"));
        break;
    case eMT_EPROM:
        outResult.assign(_T("EPROM"));
        break;
    case eMT_CDRAM:
        outResult.assign(_T("CDRAM"));
        break;
    case eMT_3DRAM:
        outResult.assign(_T("3DRAM"));
        break;
    case eMT_SDRAM:
        outResult.assign(_T("SDRAM"));
        break;
    case eMT_SGRAM:
        outResult.assign(_T("SGRAM"));
        break;
    case eMT_RDRAM:
        outResult.assign(_T("RDRAM"));
        break;
    case eMT_DDR:
        outResult.assign(_T("DDR"));
        break;
    case eMT_DDR2:
        outResult.assign(_T("DDR2"));
        break;
    case eMT_DDR3:
        outResult.assign(_T("DDR3"));
        break;
    case eMT_DDR2_FB_DIMM:
        outResult.assign(_T("DDR2_FB_DIMM"));
        break;
    case eMT_FBD2:
        outResult.assign(_T("FBD2"));
        break;
    default:
        {
            TCHAR tempCache[16];
            _stprintf_s(tempCache, _T("未知类型：%d"), type);        
            outResult.assign(tempCache);
        }
        break;
    }
}

bool CSystemDetector::GetHardDiskInfo( TString& result )
{    
    result.clear();
    bool ret = CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_DiskDrive"), _T("Model"), result);
    TString tempData;
    ret = ret && CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_DiskDrive"), _T("size"), tempData);
    TCHAR tempCache[512];
    _stprintf_s(tempCache, _T(" 容量: %s GB"), tempData.substr(0, tempData.length() - 9).c_str());//hard code to convert to GB
    result.append(tempCache);
    BYTE buffer[1024];
    size_t length = 0;
    CWmiDetector::GetInstance()->GetHDDSMARTData(buffer, length);
    if (length > 0)
    {
        CSerializer serializer(length, buffer);
        WORD smartVersion = 0;
        serializer >> smartVersion;
        SSMARTAttribute attribute;
        static const size_t MAX_ATTR_COUNT = 30;
        size_t useCount = 0;
        size_t useHours = 0;
        size_t temperature = 0;
        for (size_t i = 0; i < MAX_ATTR_COUNT; ++i)
        {
            serializer >> attribute;
            if (attribute.m_attrType == eSMART_AT_DEVICE_POWER_CYCLE_COUNT)
            {
                useCount = attribute.m_rawValue;
            }
            if (attribute.m_attrType == eSMART_AT_POWER_ON_HOURS_COUNT)
            {
                useHours = attribute.m_rawValue;
            }
            if (attribute.m_attrType == eSMART_AT_TEMPERATURE)
            {
                temperature = attribute.m_rawValue;
            }
        }
        ret = ret && CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_PhysicalMedia"), _T("SerialNumber"), tempData);
        _stprintf_s(tempCache, _T("\n序列号：%s\n已使用%d次 共计%d小时 当前温度%d℃"), tempData.c_str(), useCount, useHours, temperature);
        result.append(tempCache);
    }

    return ret;
}

bool CSystemDetector::GetSoundCardInfo( TString& result )
{
    result.clear();
    bool ret = CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_SoundDevice"), _T("ProductName"), result);
    return ret;
}

bool CSystemDetector::GetHotFixList( std::vector<TString>& result )
{
    result.clear();
    bool ret = CWmiDetector::GetInstance()->GetItemInfo(_T("Win32_QuickFixEngineering"), _T("HotFixID"), result);
    return ret;
}

bool CSystemDetector::GetDirectXInfo( TString& result )
{
    DWORD versionId;
    TCHAR versionDesc[64] = {0};
    DWORD desLen = 64;
    bool ret = CDXVerDetector::GetInstance()->GetVersion(&versionId, versionDesc, desLen);
    if (ret)
    {
        if (versionId > 0)
        {
            TCHAR versionDescTmp[64] = {0};
            _stprintf_s(versionDescTmp, _T("( DirectX %s)"), versionDesc);
            result.assign(versionDescTmp);
            //delete space suffix
            result = result.substr(0, result.find_last_not_of(' ') + 1);
        }
        else
        {
            result.assign(_T("(No DirectX installed)"));
        }
    }
    return ret;
}