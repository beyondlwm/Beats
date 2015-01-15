#ifndef BEATS_DEBUGTOOL_PERFORMDETECTOR_PERFORMDETECTOR_H__INCLUDE
#define BEATS_DEBUGTOOL_PERFORMDETECTOR_PERFORMDETECTOR_H__INCLUDE

#include "PerformDetectorDef.h"

class CPerformDetector
{
    BEATS_DECLARE_SINGLETON(CPerformDetector);

public:
    //void Start(int type, bool isStaticRecord = false);
    void StartDetectNode(int type, uint32_t id);

    float StopDetectNode(int type);
    void ResetFrameResult();
    void ClearFrameResult();//Put this method at the end of the process.

    void SetTypeName(const TCHAR* typeStr[] , uint32_t size);
    const TCHAR* GetTypeName(int type);

    SPerformanceRecord* GetRecord(uint32_t id);
    void GetResultThisFrame(std::vector<SPerformanceResult*>& outResult);

    bool PauseSwitcher();
    bool IsPaused();

    void ClearAllResult();
private:
    SPerformanceResult* GetResultFromPool();
    void IncreaseResultPool();
    void DestroyResultPool();
    void UpdateRecord(SPerformanceResult* pResult);
private:
    uint32_t m_resultPoolIndex;
    uint32_t m_lastResultPoolIndex;
    SPerformanceRecord* m_pCurRecord;
    bool m_bPause;
    bool m_bPauseRequest;    //We can only switch pause when a frame is end. Save the request.
    bool m_bClearAllRequest; //We can only clear result when a frame is end. Save the request.
    LARGE_INTEGER m_freq;

    std::vector<SPerformanceResult*> m_resultPool;//use a pool to avoid calling delete/new frequently.
    std::stack<SPerformanceResult*> m_resultOrder;
    std::vector<const TCHAR*> m_typeName;
    std::map<uint32_t, SPerformanceRecord*> m_recordMap;
    SPerformanceRecord m_rootRecord;
};
#endif