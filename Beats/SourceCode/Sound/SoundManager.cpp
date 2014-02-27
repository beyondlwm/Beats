#include "stdafx.h"
#include "SoundManager.h"
#include "SoundObject/DSWaveObject.h"
#include "SoundObject/DSMP3Object.h"
//Use DX 2009 march.
#include <dsound.h>

CSoundManager* CSoundManager::m_pInstance = NULL;


CSoundManager::CSoundManager()
: m_pDSDevice(NULL)
, m_idCounter(0)
{
}

CSoundManager::~CSoundManager()
{
    std::map<unsigned long, CDSoundObject*>::iterator iter = m_soundObjectMap.begin();

    for (; iter != m_soundObjectMap.end(); ++iter)
    {
        BEATS_SAFE_DELETE(iter->second);
    }
    m_soundObjectMap.clear();
    ReleaseSoundDevice();
}

unsigned long CSoundManager::PlaySound( const TString& file, bool loop)
{
    CDSoundObject* pSoundObject = CreateSoundObject(file);
    pSoundObject->SetLoop(loop);
    bool loadResult = pSoundObject->LoadFile(file, m_pDSDevice);
    BEATS_ASSERT(loadResult, _T("LoadFileFailed!Path: %s\n"), file.c_str());

    unsigned long id = GetValidId();
    BEATS_ASSERT(m_soundObjectMap.find(id) == m_soundObjectMap.end(), _T("Id management error!"));
    m_soundObjectMap[id] = pSoundObject;

    pSoundObject->Play();
    return id;
}

void CSoundManager::PlaySound( const unsigned long id )
{
    BEATS_ASSERT( m_soundObjectMap.find(id) != m_soundObjectMap.end(), _T("Can't find the sound with id : %d"), id);
    return m_soundObjectMap[id]->Play();
}

bool CSoundManager::Init( const HWND& hwnd )
{
    bool result = false;
    HRESULT hr = DirectSoundCreate8(NULL, &m_pDSDevice, NULL);
    if(hr == DS_OK)
    {
        hr = m_pDSDevice->SetCooperativeLevel(hwnd, DSSCL_NORMAL);
        result = hr == DS_OK;
    }
    return result;
}

void CSoundManager::ReleaseSoundDevice()
{
    if(m_pDSDevice != NULL)
    {
        m_pDSDevice->Release();
        m_pDSDevice = NULL;
    }
}

IDirectSound8* CSoundManager::GetDevice()
{
    return m_pDSDevice;
}

unsigned long CSoundManager::GetValidId()
{
    unsigned long id = m_idCounter;
    if (m_freeIdPool.size() > 0)
    {
        id = m_freeIdPool[m_freeIdPool.size() - 1];
        m_freeIdPool.pop_back();
    }
    else
    {
        ++m_idCounter;
    }
    return id;
}

void CSoundManager::StopSound(const unsigned long id, bool recycle)
{
    std::map<unsigned long, CDSoundObject*>::iterator iter = m_soundObjectMap.find(id);
    BEATS_ASSERT( iter != m_soundObjectMap.end(), _T("Stop a sound that not exists with ID:%d\n"), id);
    iter->second->Stop(); //avoid DSound keep loading the data.
    if (recycle)
    {
        BEATS_SAFE_DELETE(iter->second);
        m_freeIdPool.push_back(id);
        m_soundObjectMap.erase(iter);
    }
}

CDSoundObject* CSoundManager::CreateSoundObject(const TString& file)
{
    CDSoundObject* result = NULL;
    size_t pos = file.rfind('.');
    if (pos != TString::npos)
    {
        TString suffix = file.substr(pos+1);    
        BEATS_STR_UPER((TCHAR*)suffix.c_str(), suffix.length()+1);
        if (suffix == _T("WAV"))
        {
            result = new CDSWaveObject();
        }
        else if(suffix == _T("MP3"))
        {
            result = new CDSMP3Object();
        }
    }

    BEATS_ASSERT(result != NULL, _T("Unknown format of audio file %s!"), file.c_str());
    return result;
}

float CSoundManager::GetProgress(const unsigned long id)
{
    BEATS_ASSERT( m_soundObjectMap.find(id) != m_soundObjectMap.end(), _T("Can't find the sound with id : %d"), id);
    return m_soundObjectMap[id]->GetProgress();
}

void CSoundManager::PauseSound( const unsigned long id )
{
    BEATS_ASSERT( m_soundObjectMap.find(id) != m_soundObjectMap.end(), _T("Can't find the sound with id : %d"), id);
    return m_soundObjectMap[id]->Pause();
}

bool CSoundManager::IsPlaying(const unsigned long id)
{
    bool result = false;
    if (m_soundObjectMap.find(id) != m_soundObjectMap.end())
    {
        result = m_soundObjectMap[id]->IsPlaying();
    }
    return result;
}

bool CSoundManager::SetPlayPosByTime( const unsigned long id, const unsigned long timeMs )
{
    BEATS_ASSERT( m_soundObjectMap.find(id) != m_soundObjectMap.end(), _T("Can't find the sound with id : %d"), id);
    return m_soundObjectMap[id]->SetPlayPosByTime(timeMs);
}

void CSoundManager::SetPlayPosByProgress( const unsigned long id, const float progress )
{
    BEATS_ASSERT(progress >= 0 && progress <= 1, _T("Progress %f is out of scope [0, 1]"), progress);
    BEATS_ASSERT( m_soundObjectMap.find(id) != m_soundObjectMap.end(), _T("Can't find the sound with id : %d"), id);
    unsigned long timeMs = (unsigned long)(m_soundObjectMap[id]->GetDuration() * progress);
    m_soundObjectMap[id]->SetPlayPosByTime(timeMs);
}

unsigned long CSoundManager::GetPlayPos( const unsigned long id )
{
    BEATS_ASSERT( m_soundObjectMap.find(id) != m_soundObjectMap.end(), _T("Can't find the sound with id : %d"), id);
    return m_soundObjectMap[id]->GetPlayPos();
}

void CSoundManager::SetPlayPos(const unsigned long id, unsigned long pos )
{
    BEATS_ASSERT( m_soundObjectMap.find(id) != m_soundObjectMap.end(), _T("Can't find the sound with id : %d"), id);
    m_soundObjectMap[id]->SetPlayPos(pos);
}

unsigned long CSoundManager::GetTimeMsByPos( const unsigned long id, unsigned long pos )
{
    CDSoundObject* pSound = m_soundObjectMap[id];
    return (unsigned long)(((float)pos / pSound->GetSampleCount()) * pSound->GetDuration());
}

unsigned long CSoundManager::GetPlayTimeMs( const unsigned long id )
{
    return m_soundObjectMap[id]->GetPlayTimeMs();
}

void CSoundManager::SetSoundVolume(const unsigned long id, long volume )
{
    m_soundObjectMap[id]->SetVolume(volume);
}