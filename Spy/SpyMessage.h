#ifndef BEATS_SPY_SPYMESSAGE__H
#define BEATS_SPY_SPYMESSAGE__H

#include <Winsock2.h>

enum ESpyTCPMessageType
{
    eSMT_Invalid = 0,
    eSMT_Command,
    eSMT_FileData,

    eSMT_Count,
    eSMT_Force32Bit = 0xFFFFFFFF
};

struct SSpyTCPMessageHeader
{
    ESpyTCPMessageType m_type : 8;
    size_t m_size;
};

enum ESocketContextFlagBit
{
    // eSCFB_CanRead and eSCFB_CanSend used to indicate if this socket has data to send. 
    // Although we can judge it by m_pSendBuffer's unRead size, but use this bool we can avoid entering section.
    eSCFB_CanRead,
    eSCFB_CanSend,
    eSCFB_Invalid,

    eSCFB_Count,
    eSCFB_Force32Bit = 0xFFFFFFFF
};

struct SSocketContext
{
    SSocketContext()
        : m_uSocketFlagBit(0)
        , m_socket(INVALID_SOCKET)
        , m_pReceiveBuffer(new CSerializer(10240))
        , m_pSendBuffer(new CSerializer(10240))
    {
        InitializeCriticalSection(&m_receiveSection);
        InitializeCriticalSection(&m_sendSection);
    }

    ~SSocketContext()
    {
        BEATS_SAFE_DELETE(m_pReceiveBuffer);
        BEATS_SAFE_DELETE(m_pSendBuffer);
        DeleteCriticalSection(&m_receiveSection);
        DeleteCriticalSection(&m_sendSection);
    }

    bool GetBitFlag(ESocketContextFlagBit bitFlag)
    {
        return ((1 << bitFlag) & m_uSocketFlagBit) > 0;
    }

    void SetBitFlag(ESocketContextFlagBit bitFlag, bool bParam)
    {
        if (bParam)
        {
            m_uSocketFlagBit |= (1 << bitFlag);
        }
        else
        {
            m_uSocketFlagBit &= ~(1 << bitFlag);
        }
    }

    size_t m_uSocketFlagBit;
    SOCKET m_socket;
    CSerializer* m_pReceiveBuffer;
    CSerializer* m_pSendBuffer;
    CRITICAL_SECTION m_receiveSection;
    CRITICAL_SECTION m_sendSection;
};


#endif