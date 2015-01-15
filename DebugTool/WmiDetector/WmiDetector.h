#ifndef BEATS_DEBUGTOOL_WMIDETECTOR_WMIDETECTOR_H__INCLUDE
#define BEATS_DEBUGTOOL_WMIDETECTOR_WMIDETECTOR_H__INCLUDE

#include <WbemIdl.h>  

//To know more about SMBIOS data, read some documents.
enum EFirmwareTableType
{
    eFTT_BiosInfo = 0x00,
    eFTT_SystemInfo = 0x01,
    eFTT_Enclosure = 0x03,
    eFTT_Processor = 0x04,
    eFTT_Cache = 0x07,
    eFTT_Slots = 0x09,
    eFTT_MemDev = 0x11,

    eFTT_Count,
    eFTT_ForceTo32Bit = 0xffffffff
};

struct SFirmwareTables
{
    EFirmwareTableType type;
    uint32_t startPos;
    uint32_t length;
    SFirmwareTables()
        : type(eFTT_Count)
        , startPos(0)
        , length(0)
    {

    }
    SFirmwareTables(EFirmwareTableType firmType, uint32_t pos, uint32_t size)
        : type(firmType)
        , startPos(pos)
        , length(size)
    {

    }
};

#define MAX_DATA 0xFA00 //64K

class CWmiDetector
{
    BEATS_DECLARE_SINGLETON(CWmiDetector)
public:
    void init();
    void Uninit();
    bool GetItemInfo(const TString&, const TString&, std::vector<TString>&);
    bool GetItemInfo(const TString&, const TString&, TString&);
    void GetSMBiosData(LPBYTE pData, uint32_t& length);
    void GetHDDSMARTData(LPBYTE pData, uint32_t& length);

    bool GetFirmwareTables(const LPBYTE biosData, const uint32_t biosDataLen, const EFirmwareTableType type, std::vector<SFirmwareTables>& out);
    bool GetStrFromFirmwareTables(uint32_t id, unsigned char* tablesBuff, TString& out);

private:  
    void VariantToString(const LPVARIANT, TString &) const;
    void DebugEnumClassProperty(const TCHAR* pNameSpace, const TString& ClassName );

private:
    IEnumWbemClassObject* m_pEnumClsObj;  
    IWbemClassObject* m_pWbemClsObj;  
    IWbemServices* m_pWbemSvc;  
    IWbemLocator* m_pWbemLoc;
    bool m_comInitByOtherModule;
};


#endif