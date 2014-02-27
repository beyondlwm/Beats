#include "stdafx.h"
#include "../MP3Lib/mpg123.h"
#include "DSMP3Object.h"

CDSMP3Object::CDSMP3Object()
: CDSoundObject(eST_MP3)
, m_pHandle(NULL)
, m_cacheBuffer(NULL)
{
}

CDSMP3Object::~CDSMP3Object()
{
    if(m_pHandle != NULL)
    {
        mpg123_close(m_pHandle);
        mpg123_delete(m_pHandle);
        mpg123_exit();
        m_pHandle = NULL;
    }

    if (m_cacheBuffer != NULL)
    {
        BEATS_SAFE_DELETE_ARRAY(m_cacheBuffer);
    }
}

bool CDSMP3Object::LoadFileImpl(const TString &file, IDirectSound8* /*pDevice*/)
{
    bool result = false;
    if (mpg123_init() == MPG123_OK)
    {
        int ret = -1;
        m_pHandle = mpg123_new(NULL, &ret);
        if(m_pHandle != NULL && ret == MPG123_OK )
        {
#ifndef _UNICODE
            wchar_t wstrFile[256];
            MultiByteToWideChar(CP_ACP, 0, file.c_str(), -1, wstrFile, 256);
            result = (mpg123_topen(m_pHandle, (char*)wstrFile) == MPG123_OK) && SetWaveFormat();
#else
            result = (mpg123_topen(m_pHandle, file.c_str()) == MPG123_OK) && SetWaveFormat();
#endif
        }
    }
    return result;
}

bool CDSMP3Object::SetWaveFormat()
{
    long rate = 0;
    int channel = 0;
    int encoding = 0;
    bool result = false;
    if(mpg123_getformat(m_pHandle, &rate, &channel, &encoding) == MPG123_OK)
    {
        bool is16Encoding = ((encoding & MPG123_ENC_16) == MPG123_ENC_16);
        bool is32Encoding = !is16Encoding && ((encoding & MPG123_ENC_32) == MPG123_ENC_32);
        encoding = is16Encoding ? 16 : 
            is32Encoding ? 32 : 8;

        memset(&m_fmtWave, 0, sizeof(WAVEFORMATEX));
        m_fmtWave.wFormatTag = WAVE_FORMAT_PCM;
        m_fmtWave.nChannels = (WORD)channel;
        m_fmtWave.nSamplesPerSec = rate;
        m_fmtWave.wBitsPerSample = (WORD)encoding;
        m_fmtWave.nBlockAlign = (WORD)(encoding / 8 * channel);
        m_fmtWave.nAvgBytesPerSec = rate * (encoding / 8) * channel;

        result = true;
    }

    return result;
}

bool CDSMP3Object::LoadDSData(unsigned int start, unsigned int count)
{
    //avoid call this at both main thread when change play position and the event thread.
    EnterCriticalSection( &m_CSLoadData);

    bool result = m_EOFPosInBuffer == INVALID_EOF_POS;
    if (result)
    {
        const unsigned long bufferPieceSize = m_bufferSize / BUFFER_PIECE_COUNT;
        BEATS_ASSERT(count == bufferPieceSize || count == m_bufferSize, _T("count should always be full load or piece load!"));
        if (m_cacheBuffer == NULL)
        {
            m_cacheBuffer = new unsigned char[m_bufferSize];
        }

        LPVOID aptr1 = NULL, aptr2 = NULL;
        DWORD abyte1 = NULL, abyte2 = NULL;
        size_t outsize = 0;
        // we should promise no overflow in Lock method, so we don't care about aptr2
        if(m_pDSBuffer->Lock(start, count, &aptr1, &abyte1, &aptr2, &abyte2, 0) == DS_OK)
        {
            int readResult = mpg123_read(m_pHandle, m_cacheBuffer, count, &outsize);
            if (outsize > 0)
            {
                memcpy(aptr1, m_cacheBuffer, outsize);
                result = true;
            }
            if (readResult == MPG123_DONE || readResult == MPG123_NEED_MORE)
            {
                //clear useless data to avoid noise when play to the end.
                long resetDataBytes = bufferPieceSize >= (unsigned long)outsize ? bufferPieceSize - (unsigned long)outsize : m_bufferSize - (unsigned long)outsize;
                memset((LPVOID)((size_t)aptr1 + outsize), 0, resetDataBytes);

                m_EOFPosInBuffer = start + outsize - 1;
                DWORD playPos = 0;
                m_pDSBuffer->GetCurrentPosition(&playPos, NULL);
                if (m_EOFPosInBuffer <= (long)playPos)
                {
                    m_EOFBeforePlayPos = true;
                }
                ResumeThread(m_hCheckPlayEndThread);
            }

            result = true;
#ifdef PLAY_CONSOLE_PRINT
            PrintLoadingData(start);
#endif
            m_pDSBuffer->Unlock(aptr1, abyte1, aptr2, abyte2);
        }
    }

    LeaveCriticalSection(&m_CSLoadData);
    return result;
}

bool CDSMP3Object::SetPlayPos(unsigned int pos)
{
    bool result = false;
    if(mpg123_seek(m_pHandle, pos, SEEK_SET) >= 0 &&
        InitDSData())
    {
        m_buffer1stSample = pos == 0 ? 1 : pos;
        m_pDSBuffer->SetCurrentPosition(0);
        result = true;
    }
    return result;
}

unsigned int CDSMP3Object::GetPlayPos() const
{
    DWORD playPos = 0;
    m_pDSBuffer->GetCurrentPosition(&playPos, NULL);
    unsigned int totalElapsedSample = (unsigned int)((float)playPos / m_fmtWave.nAvgBytesPerSec * m_fmtWave.nSamplesPerSec);
    if(!m_fullCache)
    {
        totalElapsedSample += m_buffer1stSample;
    }    
    return totalElapsedSample;
}

void CDSMP3Object::ResetDuration()
{
    off_t len = mpg123_length(m_pHandle);
    m_durationMs =  (len / m_fmtWave.nSamplesPerSec) * 1000;
    BEATS_ASSERT(m_durationMs > 0, _T("Duration can't be zero!"));
}

