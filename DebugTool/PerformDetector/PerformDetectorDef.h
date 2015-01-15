#ifndef BEATS_DEBUGTOOL_PERFORMDETECTOR_PERFORMDETECTORDEF_H__INCLUDE
#define BEATS_DEBUGTOOL_PERFORMDETECTOR_PERFORMDETECTORDEF_H__INCLUDE


struct SPerformanceResult
{
    uint32_t id;
    float result;
    LARGE_INTEGER startCount;
};

struct SPerformanceRecord
{
    int type;
    uint32_t id;//this will be an unique id for each record.
    uint32_t updateCount;
    float maxValue;
    float totalValue;
    SPerformanceRecord* pParent;
    const TCHAR* typeStr;
    std::vector<SPerformanceRecord*> children;

    SPerformanceRecord(uint32_t idParam = 0, int typeParam = 0)
        : id (idParam)
        , updateCount(0)
        , type(typeParam)
        , maxValue(0)
        , totalValue(0)
        , pParent(NULL)
        , typeStr(NULL)
    {

    }

    void Reset()
    {
        updateCount = 0;
        maxValue = 0;
        totalValue = 0;
        for (uint32_t i = 0; i < children.size(); ++i)
        {
            children[i]->Reset();
        }
    }
};

#endif