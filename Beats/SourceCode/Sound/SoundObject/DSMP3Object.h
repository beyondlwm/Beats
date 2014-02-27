#ifndef BEATS_SOUND_SOUNDOBJECT_CDSMP3OBJECT_H__INCLUDE
#define BEATS_SOUND_SOUNDOBJECT_CDSMP3OBJECT_H__INCLUDE

#include <string>
#include "DSoundObject.h"

struct mpg123_handle_struct;

class CDSMP3Object : public CDSoundObject
{
public:
    CDSMP3Object();
    virtual ~CDSMP3Object();

protected:
    virtual bool SetWaveFormat();
    virtual bool LoadDSData(unsigned int start, unsigned int count);
    virtual bool LoadFileImpl(const TString& file, IDirectSound8* pDevice);
    virtual bool SetPlayPos(unsigned int sampleIndex);
    virtual unsigned int GetPlayPos() const;
    virtual void ResetDuration();

private:
    mpg123_handle_struct* m_pHandle;
    unsigned char* m_cacheBuffer;
};

#endif
