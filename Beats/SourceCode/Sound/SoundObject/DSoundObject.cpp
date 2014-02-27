#include "stdafx.h"
#include "DSoundObject.h"
#include <process.h>
#include "../../Utility/UtilityManager.h"

const long lBufferTime = 3000; //in ms

CDSoundObject::CDSoundObject(ESoundType type)
: m_type(type)
, m_loop(true)
, m_EOFBeforePlayPos(false)
, m_fullCache(false)
, m_buffer1stSample(1)
, m_durationMs(0)
, m_EOFPosInBuffer(INVALID_EOF_POS)
, m_uiNotifyThreadID(0)
, m_pDSBuffer(NULL)
, m_pDSNotify(NULL)
, m_hNotifyThread(NULL)
, m_checkPlayEndThreadID(0)
, m_hCheckPlayEndThread(NULL)
, m_bufferSize(0)
, m_releaseThread(false)
{
    BOOL result = InitializeCriticalSectionAndSpinCount(&m_CSLoadData, 4000);
    BEATS_ASSERT(result != 0, _T("Failed to initialize critical section!"));
    m_hCheckPlayEndThread = (HANDLE)(_beginthreadex(NULL, 0, (unsigned (__stdcall*)(void*))CheckPlayEndProc, (void*)this, CREATE_SUSPENDED, (unsigned*)(&m_checkPlayEndThreadID)));
    CUtilityManager::GetInstance()->SetThreadName(m_checkPlayEndThreadID, "CheckPlayEnd");
    for (size_t i = 0; i < THREAD_COUNT; ++i)
    {
        m_destructEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
    }
}

CDSoundObject::~CDSoundObject()
{
    ReleaseDSoundBuffer();
    //force resume the thread so we can end the thread.
    m_releaseThread = true;
    ReleaseNotifyThread();    
    ResumeThread(m_hCheckPlayEndThread);
    ReleaseCheckPlayEndThread();
    WaitForMultipleObjects(THREAD_COUNT, m_destructEvent, true, INFINITE);//wait for all thread exit.
    DeleteCriticalSection(&m_CSLoadData);
}

bool CDSoundObject::CreateDSoundBuffer(IDirectSound8* pDevice,const unsigned long bufferTimeMs)
{
    IDirectSoundBuffer* psbuffer = NULL;

    DSBUFFERDESC desc;
    memset(&desc, 0, sizeof(DSBUFFERDESC));
    desc.dwSize = sizeof(DSBUFFERDESC);
    desc.dwFlags = DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_LOCSOFTWARE;
    desc.dwBufferBytes = (DWORD)(GetValidBufferTime(bufferTimeMs) * 0.001f * m_fmtWave.nAvgBytesPerSec);
    desc.lpwfxFormat = &m_fmtWave;

    m_bufferSize = desc.dwBufferBytes;

    if(m_pDSBuffer != NULL)
        m_pDSBuffer->Release();

    bool result = false;
    HRESULT hr = pDevice->CreateSoundBuffer(&desc, &psbuffer, NULL);
    if(hr == DS_OK)
    {
        result = psbuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&m_pDSBuffer) == DS_OK;
        psbuffer->Release();
    }
    BEATS_ASSERT(result, _T("Create Buffer Failed!"));
    return result;
}

void CDSoundObject::ReleaseDSoundBuffer()
{
    if(m_pDSBuffer != NULL)
    {
        m_pDSBuffer->Stop();
        m_pDSBuffer->Release();
        m_pDSBuffer = NULL;
    }
}

bool CDSoundObject::CreateNotifyThread()
{
    bool result = false;
    //event
    for (size_t i = 0; i < BUFFER_PIECE_COUNT; ++i)
    {
        m_notifyEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
    }

    m_hNotifyThread = (HANDLE)(_beginthreadex(NULL, 0, (unsigned (__stdcall*)(void*))NotifyHandleProc, (void*)this, 0, (unsigned int*)(&m_uiNotifyThreadID)));
    CUtilityManager::GetInstance()->SetThreadName(m_uiNotifyThreadID, "SoundNotifyThread");
    BEATS_ASSERT(m_hNotifyThread != NULL, _T("Create Notify thread failed!"));
    HRESULT hr = m_pDSBuffer->QueryInterface(IID_IDirectSoundNotify8, (void**)&m_pDSNotify);
    if(hr == DS_OK)
    {
        for (DWORD i = 0; i < BUFFER_PIECE_COUNT; ++i)
        {
            m_notifyPos[i].dwOffset = (i+1) * (DWORD)(m_bufferSize / BUFFER_PIECE_COUNT) - 1;
            m_notifyPos[i].hEventNotify = m_notifyEvent[i];
        }
        BEATS_ASSERT(m_notifyPos[BUFFER_PIECE_COUNT -1].dwOffset == m_bufferSize - 1, _T("Buffer size is not aligned!"));
        hr = m_pDSNotify->SetNotificationPositions(BUFFER_PIECE_COUNT, m_notifyPos);
        result = hr == DS_OK;
        BEATS_ASSERT(result, _T("Set Notification Pos Failed!"));
    }
    return result;
}

void CDSoundObject::ReleaseNotifyThread()
{
    if(m_hNotifyThread != NULL)
    {
        SetEvent(m_notifyEvent[0]);
        CloseHandle(m_hNotifyThread);
        m_hNotifyThread = NULL;
    }

    m_pDSNotify->Release();
    m_pDSNotify = NULL;

    for(int i = 0; i < BUFFER_PIECE_COUNT; ++ i)
    {
        CloseHandle(m_notifyEvent[i]);
        m_notifyEvent[i] = NULL;
    }
}

void CDSoundObject::ReleaseCheckPlayEndThread()
{
    CloseHandle(m_hCheckPlayEndThread);
    m_hCheckPlayEndThread = NULL;
}

DWORD CDSoundObject::NotifyHandleProc(LPVOID param)
{
    CDSoundObject* obj = static_cast<CDSoundObject*>(param);
    BEATS_ASSERT(obj != NULL, _T("Get NULL obj!"));
    while(true)
    {
        DWORD ret = WaitForMultipleObjects(BUFFER_PIECE_COUNT, obj->GetEventHandle(), FALSE, INFINITE);
        if (obj->m_releaseThread)
        {
            break;
        }
        if(ret != WAIT_FAILED)
        {
            DWORD notify = ret - WAIT_OBJECT_0;
            const int bufferPieceSize = obj->GetBufferSize() / BUFFER_PIECE_COUNT;
            obj->LoadDSData(notify * bufferPieceSize, bufferPieceSize);

            if (notify == BUFFER_PIECE_COUNT - 1)
            {
                DWORD playPos;
                obj->m_pDSBuffer->GetCurrentPosition(&playPos, NULL);
                obj->OnPlayAtBufferEnd();
            }
        }
    }
    SetEvent(obj->m_destructEvent[0]);
    return 0;
}

DWORD CDSoundObject::CheckPlayEndProc( LPVOID param )
{
    CDSoundObject* obj = static_cast<CDSoundObject*>(param);
    BEATS_ASSERT(obj != NULL, _T("Get NULL obj!"));
    while(!obj->m_releaseThread)
    {
        DWORD playPos = 0;
        obj->m_pDSBuffer->GetCurrentPosition(&playPos, NULL);
        if (!obj->m_EOFBeforePlayPos && playPos >= (DWORD)(obj->m_EOFPosInBuffer))
        {
            // stop the buffer to avoid trigger notify event. 
            obj->Stop();
            if (obj->m_loop)
            {
                obj->Play();
            }
            if (!obj->m_fullCache)
            {
                SuspendThread(obj->m_hCheckPlayEndThread);
            }
        }
        Sleep(10);
    }
    SetEvent(obj->m_destructEvent[1]);
    return 0;
}

void CDSoundObject::Play()
{
    m_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);
}

void CDSoundObject::Pause()
{
    m_pDSBuffer->Stop();
}

void CDSoundObject::Stop()
{
    m_pDSBuffer->Stop();
    m_pDSBuffer->SetCurrentPosition(0);
    if (!m_fullCache)//full cache never clear eof
    {
        m_EOFPosInBuffer = INVALID_EOF_POS;
    }
    SetPlayPos(0);

}

bool CDSoundObject::IsPlaying() const
{
    bool result = false;
    if(m_pDSBuffer != NULL)
    {
        DWORD status = 0;
        HRESULT hr = m_pDSBuffer->GetStatus(&status);
        result = hr == DS_OK && (status & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING;
    }
    return result;
}

void CDSoundObject::SetVolume(long vol)
{
    if(vol > DSBVOLUME_MAX)
        vol = DSBVOLUME_MAX;
    if(vol < DSBVOLUME_MIN)
        vol = DSBVOLUME_MIN;
    if(m_pDSBuffer != NULL)
    {
        m_pDSBuffer->SetVolume(vol);
    }
}

long CDSoundObject::GetVolume() const
{
    long vol = 0;
    m_pDSBuffer->GetVolume(&vol);
    return vol;
}

bool CDSoundObject::InitDSData()
{
    bool result = LoadDSData(0, m_bufferSize);
    return result;
}

long CDSoundObject::GetBufferSize() const
{
    return m_bufferSize;
}

bool CDSoundObject::SetPlayPosByTime( unsigned long timeMs )
{
    BEATS_ASSERT(timeMs <= m_durationMs, _T("time is out of range!"));
    unsigned int offSet = (unsigned int)(timeMs * 0.001f * m_fmtWave.nSamplesPerSec);
    return SetPlayPos(offSet);
}

const HANDLE* CDSoundObject::GetEventHandle() const
{
    return &(m_notifyEvent[0]);
}

bool CDSoundObject::IsLoop() const
{
    return m_loop;
}

unsigned long CDSoundObject::GetPlayTimeMs() const
{
    unsigned long playTimeMs = (unsigned long)((float)GetPlayPos() / m_fmtWave.nSamplesPerSec * 1000);
    ///it is possible to overflow, because DSound load data in multithread
    ///even it meets the end of the data, the playPos Ptr will keep going a little while
    if (playTimeMs > m_durationMs)
    {
        playTimeMs = m_durationMs;
    }
    return playTimeMs;
}

unsigned long CDSoundObject::GetDuration() const
{
    return m_durationMs;
}

bool CDSoundObject::LoadFile( const TString& file, IDirectSound8* pDevice )
{
    bool result = LoadFileImpl(file, pDevice);
    if (result)
    {
        ResetDuration();
        result = CreateDSoundBuffer(pDevice, lBufferTime);
        BEATS_ASSERT(result, _T("CREATE SOUND BUFFER FAILED!"));
        m_fullCache = m_durationMs * 0.001f * m_fmtWave.nAvgBytesPerSec <= m_bufferSize;
        CreateNotifyThread(); 
        InitDSData();
    }
    return result;
}

HANDLE CDSoundObject::GetCheckPlayEndThread() const
{
    return m_hCheckPlayEndThread;
}

unsigned long CDSoundObject::GetValidBufferTime( const long rawBufferTime )
{
    //align the buffer to avoid data lose!
    unsigned long realBufferTimeMs = rawBufferTime + 1;
    bool bufferSizeValid = false;
    do 
    {
        --realBufferTimeMs;//at 1st time, we will restore it to rawBufferTime
        BEATS_ASSERT(realBufferTimeMs > 0, _T("bufferTimeMs is not enough!"));
        float bufferTimeInSec = realBufferTimeMs * 0.001f;
        bool bufferBytesAlign = (realBufferTimeMs * m_fmtWave.nAvgBytesPerSec) % 1000 == 0;
        bool bufferPieceBytesAlign = ((long)(bufferTimeInSec * m_fmtWave.nAvgBytesPerSec)) % BUFFER_PIECE_COUNT == 0;
        float samplesPerPiece = (bufferTimeInSec * m_fmtWave.nSamplesPerSec) / BUFFER_PIECE_COUNT;
        bool bufferPieceSampleAlign =  samplesPerPiece == long(samplesPerPiece);
        
        bufferSizeValid = bufferBytesAlign && bufferPieceBytesAlign && bufferPieceSampleAlign;
    } while (!bufferSizeValid);
    return realBufferTimeMs;
}

void CDSoundObject::OnPlayAtBufferEnd()
{
    //force set play position
    //because the event may be triggered a little earlier before the play pos meets the ends.(Don't know why)
    m_pDSBuffer->SetCurrentPosition(0);
    m_EOFBeforePlayPos = false;
    m_buffer1stSample += (long)((float)m_bufferSize * m_fmtWave.nSamplesPerSec / m_fmtWave.nAvgBytesPerSec);
    if (m_buffer1stSample >= (long)(m_durationMs * 0.001f * m_fmtWave.nSamplesPerSec ))
    {
        m_buffer1stSample = 1;
    }
}

void CDSoundObject::SetLoop( bool isLoop )
{
    m_loop = isLoop;
}

float CDSoundObject::GetProgress() const
{
    float currentPlayTimeInMs =  (float)GetPlayTimeMs();
    return currentPlayTimeInMs / m_durationMs;
}


unsigned long CDSoundObject::GetSampleCount()
{
    return (unsigned long)(m_fmtWave.nSamplesPerSec * (m_durationMs * 0.001f));
}


//////////////////////////////////////////////////////////////////////////
///Debug Tool Func
void CDSoundObject::PrintPlayingProgress()
{
    DWORD playPos;
    m_pDSBuffer->GetCurrentPosition(&playPos,NULL);
    int rate = (long)((float)playPos / m_bufferSize * 100);
    int eofRate = -1;
    if (m_EOFPosInBuffer != INVALID_EOF_POS)
    {
        eofRate = (int)((float)m_EOFPosInBuffer * 100 / m_bufferSize );
    }
    for (int i = 0; i < 100; ++i)
    {
        bool isItEof = eofRate != -1 && i == eofRate;
        bool isPlayed = i < rate;
        
        BEATS_PRINT_VARIABLE(isItEof ? _T("2") : isPlayed ? _T("1") : _T("0"));
    }
    BEATS_PRINT(_T("\n"));
}

void CDSoundObject::PrintLoadingData(unsigned int startByte)
{
    long rate = (long)((float)startByte * 100 / m_bufferSize);
    long pieceRate = (long)((float)100 / BUFFER_PIECE_COUNT) + rate;
    for (int i = 0; i < 100; ++i)
    {
        bool isLoadingData = i >= rate && i <= pieceRate;
        BEATS_PRINT_VARIABLE(isLoadingData ? _T("8") : _T("0"));
    }
    BEATS_PRINT(_T("\n"));
}
