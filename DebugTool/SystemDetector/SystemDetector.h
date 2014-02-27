#ifndef BEATS_DEBUGTOOL_SYSTEMDETECTOR_SYSTEMDETECTOR_H__INCLUDE
#define BEATS_DEBUGTOOL_SYSTEMDETECTOR_SYSTEMDETECTOR_H__INCLUDE

class CSystemDetector
{
    BEATS_DECLARE_SINGLETON(CSystemDetector);
public:

    bool GetComputerModel(TString& result);
    bool GetMonitorInfo(TString& result);
    bool GetOSName(TString& result);
    bool GetOSVersion(TString& result);
    bool GetGraphicsCardInfo(TString& result);
    bool GetProcessorInfo(TString& result);
    bool GetBaseBoardInfo(TString& result);
    bool GetMemoryInfo(TString& result);
    bool GetHardDiskInfo(TString& result);
    bool GetSoundCardInfo(TString& result);
    bool GetDirectXInfo(TString& result);

    bool GetHotFixList(std::vector<TString>& result);

private:
    void ConvertMemoryManufactuer(const TString& inCode, TString& outResult);
    void ConvertMemoryType(const size_t type, TString& outResult);
    bool GetEDIDFromReg(void* pData, size_t& dataLength);
};

#endif