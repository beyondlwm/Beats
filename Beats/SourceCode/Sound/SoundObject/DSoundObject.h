#ifndef BEATS_SOUND_SOUNDOBJECT_DSOUNDOBJECT_H__INCLUDE
#define BEATS_SOUND_SOUNDOBJECT_DSOUNDOBJECT_H__INCLUDE

#ifdef _DEBUG
//#define PLAY_CONSOLE_PRINT //switch to print buffer progress at output
#endif

#include <dsound.h>

#include <string>

#define BUFFER_PIECE_COUNT 3
#define INVALID_EOF_POS -1
#define THREAD_COUNT 2

enum ESoundType 
{ 
    eST_WAVE, 
    eST_MP3,

    eST_Count,
    eST_ForceTo32Bite = 0xffffffff
};

class CDSoundObject
{
public:
    CDSoundObject(ESoundType type);
    virtual ~CDSoundObject();

    void Play();
    void Pause();
    void Stop();
    bool IsPlaying() const;
    bool IsLoop() const;
    void SetLoop(bool isLoop);

    void SetVolume(long vol);
    long GetVolume() const;
    long GetBufferSize() const;

    const HANDLE* GetEventHandle() const;
    bool SetPlayPosByTime(unsigned long timeMs);
    unsigned long GetPlayTimeMs() const;
    bool LoadFile(const TString& file, IDirectSound8* pDevice);

    unsigned long GetDuration() const;
    float GetProgress() const;
    unsigned long GetSampleCount();

    ///debug function
    void PrintPlayingProgress();
    void PrintLoadingData(unsigned int startByte);


    ///Get pos in sample index.
    virtual unsigned int GetPlayPos() const = 0;
    virtual bool SetPlayPos(unsigned int sampleIndex) = 0;

protected:

    bool CreateDSoundBuffer(IDirectSound8* pDevice, const unsigned long bufferTime);

    bool CreateNotifyThread();
    void ReleaseNotifyThread();
    bool InitDSData();

    HANDLE GetCheckPlayEndThread() const;

    virtual void ResetDuration() = 0;
    virtual bool SetWaveFormat() = 0;
    virtual bool LoadDSData(unsigned int start, unsigned int count) = 0;
    virtual bool LoadFileImpl(const TString& file, IDirectSound8* pDevice) = 0;

    static DWORD NotifyHandleProc(LPVOID param);
    static DWORD CheckPlayEndProc(LPVOID param);

private:

    unsigned long GetValidBufferTime(const long rawBufferTime);
    void OnPlayAtBufferEnd();
    void ReleaseDSoundBuffer();
    void ReleaseCheckPlayEndThread();

protected:
    ESoundType m_type;
    bool m_loop;
    bool m_EOFBeforePlayPos;
    bool m_fullCache;
    long m_EOFPosInBuffer; 
    unsigned long m_bufferSize;
    unsigned long m_durationMs;
    long m_buffer1stSample;
    IDirectSoundBuffer8 * m_pDSBuffer;

protected:
    IDirectSoundNotify8* m_pDSNotify;

    DWORD m_uiNotifyThreadID;
    DWORD m_checkPlayEndThreadID;
    HANDLE m_hNotifyThread;
    HANDLE m_hCheckPlayEndThread;
    HANDLE m_destructEvent[THREAD_COUNT];
    HANDLE m_notifyEvent[BUFFER_PIECE_COUNT];
    DSBPOSITIONNOTIFY m_notifyPos[BUFFER_PIECE_COUNT];

protected:
    WAVEFORMATEX    m_fmtWave;
    CRITICAL_SECTION m_CSLoadData;

private:
    bool m_releaseThread;
};

#endif