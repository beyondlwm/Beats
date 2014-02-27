#ifndef BEATS_SOUND_SOUNDMANAGER_H__INCLUDE
#define BEATS_SOUND_SOUNDMANAGER_H__INCLUDE

struct IDirectSound8;
class CDSoundObject;
class CSoundManager
{
    BEATS_DECLARE_SINGLETON(CSoundManager);

public:
    bool Init(const HWND& hwnd);
    IDirectSound8* GetDevice();

    unsigned long PlaySound(const TString& file, bool loop = false);
    void StopSound(const unsigned long id, bool recycle = false);
    void PauseSound(const unsigned long id);
    void PlaySound(const unsigned long id);
    bool IsPlaying(const unsigned long id);
    float GetProgress(const unsigned long id);
    unsigned long GetPlayTimeMs(const unsigned long id);
    unsigned long GetPlayPos(const unsigned long id);
    void SetPlayPos(const unsigned long id, unsigned long pos);
    bool SetPlayPosByTime(const unsigned long id, const unsigned long timeMs);
    unsigned long GetTimeMsByPos(const unsigned long id, unsigned long pos);

    void SetPlayPosByProgress(const unsigned long id, const float progress);

    void SetSoundVolume(const unsigned long id, long volume);
    
private:
    void ReleaseSoundDevice();
    unsigned long GetValidId();
    CDSoundObject* CreateSoundObject(const TString& file);

private:
    IDirectSound8 * m_pDSDevice;

    unsigned long m_idCounter;
    std::vector<unsigned long> m_freeIdPool;

    std::map<unsigned long, CDSoundObject*> m_soundObjectMap;
};

#endif