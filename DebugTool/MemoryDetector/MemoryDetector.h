#ifndef BEATS_DEBUGTOOL_MEMORYDETECTOR_MEMORYDETECTOR_H__INCLUDE
#define BEATS_DEBUGTOOL_MEMORYDETECTOR_MEMORYDETECTOR_H__INCLUDE

#include "MemoryDetectorDef.h"

class CMemoryDetector
{
    BEATS_DECLARE_SINGLETON(CMemoryDetector);

    typedef void* (__cdecl *new_t) (uint32_t);
    typedef void* (__cdecl *debug_new_t) (uint32_t, int usage, const char* filePath, int line);
    typedef void (__cdecl *delete_t) (void*);
public:
    void Init(std::vector<std::string>* pMemHookModuleList);
    void Uninit();
    bool IsInit();

    void AddHookModule(const char* moduleName);
    void AddRecord(void* pAddress, uint32_t size, uint32_t allocId, uint32_t usage = 0);
    void DeleteRecord(void* pAddress);
    void GetCallStack(uint32_t address, std::vector<uint32_t>& out );

    SMemoryRecord* GetValidRecord(void* pAddress, uint32_t size, uint32_t allocTime, uint32_t allocId, uint32_t usage);
    void GetRecordCache(const SMemoryFrameRecord* pOriginData, SMemoryFrameRecord& out, EMemoryFrameRecordType type = eMFRT_All, uint32_t startPos = 0, uint32_t itemCount = 0);
    uint32_t GetAllocMemorySize();
    uint32_t GetAllocMemoryCount();
    uint32_t GetMemoryBlockMaxSize();
    uint32_t GetLatestMemoryBlockTime();
    void AddFilterStr(std::string filter);
    void DelFilterStr(std::string filter);
    bool FilterRecordWithEIP(uint32_t eip);
    const std::vector<std::string>& GetFilterStr();

    void SetProcessDebugFlag(bool inDebug);
    bool GetHookModuleName(char*& crtLibName, char*& mfcLibName);
    bool IsHookableModule( HMODULE module );

    CRITICAL_SECTION* GetHeapOperationCS();

private:
    bool PatchDll(HMODULE importmodule, LPCSTR exportmodulename, LPCSTR exportmodulepath, LPCSTR importname,
        LPCVOID replacement);
    bool ReplaceFunc(HMODULE hookModule, const char* exprotModuleName, LPCVOID targetFunc, LPCVOID replaceFunc);
    IMAGE_IMPORT_DESCRIPTOR* GetIDTE(HMODULE module, const char* exportmoduleName);
    bool CheckMemoryLeak();
    static LPVOID OnHeapAlloc (uint32_t size);
    static LPVOID OnHeapAllocDbg (uint32_t size, int usage, const char* filePath, int line);

    static void OnHeapFree (void* pAddress);
    static void OnHeapArrayFree(void* pAddress);
    void OnHeapFreeImpl(void* pAddress);
    bool HookMFC(bool patchOrRestore, const char* hookModuleName);
    bool HookCRT(bool patchOrRestore, const char* hookModuleName);
    bool InitRunTimeLib(const char* hookModuleName,
                        const char* crtMouduleName, 
                        const char* newSymbol,
                        const char* newDbgSymbol,
                        const char* delSymbol,
                        const char* arrayDelSymbol,
                        const new_t newFunc, 
                        const delete_t delFunc,
                        const delete_t arrayDeleteFunc);
    bool HookImpl(bool patchOrRestore,
                    const char* hookModuleName,
                    const char* crtModuleName, 
                    const char* newSymbol,
                    const char* newDbgSymbol,
                    const char* delSymbol,
                    const char* arrayDelSymbol,
                    const new_t newFunc, 
                    const delete_t delFunc);

    void AddCallStackInfo(uint32_t address);
    bool AddCallStackInfoByEBP(std::vector<uint32_t>& callStack);

    void DeleteCallStackInfo(uint32_t address);
    HMODULE GetModuleFromAddress(uint32_t address);

private:
    static new_t m_allocFunc;
    static debug_new_t m_allocFuncDbg;
    static delete_t m_freeFunc;
    static delete_t m_arrayFreeFunc;

    uint32_t m_allocMemorySize;
    bool m_processInDebugMode;
    bool m_processInUnicodeMode;
    bool m_initFlag;
    HMODULE m_curModule;

    std::vector<SMemoryRecord*> m_recordPool;
    std::vector<std::string> m_filterList;
    std::vector<std::string> m_hookModuleList;
    std::map<uint32_t, std::vector<uint32_t>> m_callStackPool;
    std::map<uint32_t, bool> m_eipExamMap;
    SMemoryFrameRecord      m_record;
    CRITICAL_SECTION m_CSLock;

};

#endif