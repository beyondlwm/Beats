#ifndef BEATS_DEBUGTOOL_SYSTEMDETECTOR_SYSTEMDETECTORDEF_H__INCLUDE
#define BEATS_DEBUGTOOL_SYSTEMDETECTOR_SYSTEMDETECTORDEF_H__INCLUDE

//device type
enum EMemoryType
{
    eMT_DRAM = 0x03,
    eMT_EDRAM = 0x04,
    eMT_VRAM = 0x05,
    eMT_SRAM = 0x06,
    eMT_RAM = 0x07,
    eMT_ROM = 0x08,
    eMT_FLASH = 0x09,
    eMT_EEPROM = 0x0A,
    eMT_FEPROM = 0x0B,
    eMT_EPROM = 0x0C,
    eMT_CDRAM = 0x0D,
    eMT_3DRAM = 0x0E,
    eMT_SDRAM = 0x0F,
    eMT_SGRAM = 0x10,
    eMT_RDRAM = 0x11,
    eMT_DDR = 0x12,
    eMT_DDR2 = 0x13,
    eMT_DDR2_FB_DIMM = 0x14,
    eMT_DDR3 = 0x18,
    eMT_FBD2 = 0x19,

    eMT_Count,
    eMT_ForceTo32Bit = 0xffffffff
};


// to know why we organize data like this. read some thing about S.M.A.R.T. attribute.
// to set the pack 1, is for serializing.
#pragma pack(push)
#pragma pack(1)
struct SSMARTAttribute
{
    BYTE m_attrType;//ESMARTAttributeType
    WORD m_flags;
    BYTE m_value;
    BYTE m_worstValue;
    WORD m_rawValue;
    BYTE m_data[5];
};
#pragma pack(pop)

enum ESMARTAttributeType
{
    eSMART_AT_RAW_READ_ERROR_RATE =                 1,
    eSMART_AT_THROUGHPUT_PERFORMANCE =              2,
    eSMART_AT_SPIN_UP_TIME =                        3,
    eSMART_AT_START_STOP_COUNT =                    4,
    eSMART_AT_START_REALLOCATION_SECTOR_COUNT =     5,
    eSMART_AT_SEEK_ERROR_RATE =                     7,
    eSMART_AT_POWER_ON_HOURS_COUNT =                9,
    eSMART_AT_SPIN_RETRY_COUNT =                    10,
    eSMART_AT_RECALIBRATION_RETRIES    =            11,
    eSMART_AT_DEVICE_POWER_CYCLE_COUNT =            12,
    eSMART_AT_SOFT_READ_ERROR_RATE =                13,
    eSMART_AT_LOAD_UNLOAD_CYCLE_COUNT =             193,
    eSMART_AT_TEMPERATURE =                         194,
    eSMART_AT_ECC_ON_THE_FLY_COUNT =                195,
    eSMART_AT_REALLOCATION_EVENT_COUNT =            196,
    eSMART_AT_CURRENT_PENDING_SECTOR_COUNT =        197,
    eSMART_AT_UNCORRECTABLE_SECTOR_COUNT =          198,
    eSMART_AT_ULTRA_DMA_CRC_ERROR_COUNT =           199,
    eSMART_AT_WRITE_ERROR_RATE =                    200,
    eSMART_AT_TA_COUNTER_INCREASED =                202,
    eSMART_AT_GSENSE_ERROR_RATE =                   221,
    eSMART_AT_POWER_OFF_RETRACT_COUNT =             228,

    eSMART_AT_COUNT =                               23,
    eSMART_AT_Force32Bit =                          0xffffffff
};


#endif