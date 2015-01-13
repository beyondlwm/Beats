#include "stdafx.h"
#include "DSWaveObject.h"

CDSWaveObject::CDSWaveObject()
: CDSoundObject(eST_WAVE)
, _uiReadSize(0)
{
}

CDSWaveObject::~CDSWaveObject()
{
    if(_ifStream.is_open())
        _ifStream.close();
}

bool CDSWaveObject::LoadFileImpl(const TString &file, IDirectSound8* /*pDevice*/)
{
    bool result = false;
    _ifStream.open(file.c_str(), std::ios::in | std::ios::binary);
    if(_ifStream.is_open())
    {
        memset(&_headerWave, 0, sizeof(WAVE_HEADER));

        if(ReadWaveHeader(_headerWave))
        {
            result = SetWaveFormat();
        }
    }
    return result;
}

bool CDSWaveObject::ReadWaveHeader(CDSWaveObject::WAVE_HEADER &header)
{
    _ifStream.seekg(0, std::ios::beg);
    _ifStream.read((char*)&header, sizeof(WAVE_HEADER));

    bool result = false;
    if(_ifStream.good())
    {
        bool checkFormat = memcmp(header.riff_sig, "RIFF", 4) ||
            memcmp(header.wave_sig, "WAVE", 4) ||
            memcmp(header.format_sig, "fmt ", 4) || 
            memcmp(header.data_sig, "data", 4);
        result = !checkFormat;
    }
    return result;
}

bool CDSWaveObject::SetWaveFormat()
{
    memset(&m_fmtWave, 0, sizeof(WAVEFORMATEX));
    m_fmtWave.wFormatTag = WAVE_FORMAT_PCM;
    m_fmtWave.nChannels = _headerWave.channels;
    m_fmtWave.nSamplesPerSec = _headerWave.sample_rate;
    m_fmtWave.wBitsPerSample = _headerWave.bits_per_sample;
    m_fmtWave.nBlockAlign = _headerWave.bits_per_sample / 8 * _headerWave.channels;// header.block_align;
    m_fmtWave.nAvgBytesPerSec = _headerWave.sample_rate * _headerWave.block_align;//header.
    return true;
}

bool CDSWaveObject::LoadDSData(unsigned int start, unsigned int count)
{
    //avoid call this at both main thread when change play position and the event thread.
    EnterCriticalSection( &m_CSLoadData);

    bool result = false;
    BEATS_ASSERT(start >= 0 && count > 0, _T("Invalid arg for LoadDSData!"));
    if (m_EOFPosInBuffer == INVALID_EOF_POS && _ifStream.good())
    {
        const unsigned long bufferPieceSize = m_bufferSize / BUFFER_PIECE_COUNT;

        LPVOID aptr1 = NULL, aptr2 = NULL;
        DWORD abyte1 = 0, abyte2 = 0;

        if(m_pDSBuffer->Lock(start, count, &aptr1, &abyte1, &aptr2, &abyte2, 0) == DS_OK)
        {
            bool meetsEnd = false; // don't use eof because the wave file may contains some useless data;
            if ((unsigned int)(_ifStream.tellg()) + count > _headerWave.data_size)
            {
                count = _headerWave.data_size - (unsigned long)_ifStream.tellg();
                meetsEnd = true;
            }
            _ifStream.read((char*)aptr1, count);
            if (meetsEnd)
            {
                //clear useless data to avoid noise when play to the end.
                long resetDataBytes = bufferPieceSize >= count ? bufferPieceSize - count : m_bufferSize - count;
                memset((LPVOID)((size_t)aptr1 + count), 0, resetDataBytes);

                m_EOFPosInBuffer = start + count - 1;
                DWORD playPos = 0;
                m_pDSBuffer->GetCurrentPosition(&playPos, NULL);
                if (m_EOFPosInBuffer <= (long)playPos)
                {
                    m_EOFBeforePlayPos = true;
                }
                ResumeThread(m_hCheckPlayEndThread);            
            }

#ifdef PLAY_CONSOLE_PRINT
            PrintLoadingData(start);
#endif
            m_pDSBuffer->Unlock(aptr1, abyte1, aptr2, abyte2);
        }
    }
  
    LeaveCriticalSection(&m_CSLoadData);

    return result;
}

//TODO: why not refrence pos? a bug?
bool CDSWaveObject::SetPlayPos(unsigned int /*pos*/)
{
    bool result = false;
    _ifStream.seekg(sizeof(WAVE_HEADER), std::ios::beg);
    if(LoadDSData(0, m_bufferSize)  )
    {
        result = true;
        m_pDSBuffer->SetCurrentPosition(0);
    }
    return result;
}

void CDSWaveObject::ResetDuration()
{
    m_durationMs = (_headerWave.data_size * 1000 / m_fmtWave.nAvgBytesPerSec);
}