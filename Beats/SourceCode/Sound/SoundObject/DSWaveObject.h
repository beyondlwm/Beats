#ifndef BEATS_SOUND_SOUNDOBJECT_DSWAVEOBJECT_H__INCLUDE
#define BEATS_SOUND_SOUNDOBJECT_DSWAVEOBJECT_H__INCLUDE

#include <fstream>

#include "DSoundObject.h"

class CDSWaveObject : public CDSoundObject
{
protected:
    // .WAV file header
    struct WAVE_HEADER
    {
        TCHAR    riff_sig[4];            // 'RIFF'
        long    waveform_chunk_size;    // 8
        TCHAR    wave_sig[4];            // 'WAVE'
        TCHAR    format_sig[4];          // 'fmt ' (notice space after)
        long    format_chunk_size;      // 16;
        short   format_tag;             // WAVE_FORMAT_PCM
        short   channels;               // # of channels
        long    sample_rate;            // sampling rate
        long    bytes_per_sec;          // bytes per second
        short   block_align;            // sample block alignment
        short   bits_per_sample;        // bits per second
        TCHAR    data_sig[4];            // 'data'
        unsigned long    data_size;              // size of waveform data
    };

public:
    CDSWaveObject();
    virtual ~CDSWaveObject();

    virtual void ResetDuration();
    virtual bool SetPlayPos(unsigned int sampleIndex);
    virtual unsigned int GetPlayPos() const { return 0; }
protected:

    bool ReadWaveHeader(WAVE_HEADER& header);

    virtual bool SetWaveFormat();

    virtual bool LoadDSData(unsigned int start, unsigned int count);
    virtual bool LoadFileImpl(const TString& file, IDirectSound8* pDevice);

private:
    DWORD _uiReadSize;
    WAVE_HEADER _headerWave;
    std::ifstream _ifStream;
};


#endif