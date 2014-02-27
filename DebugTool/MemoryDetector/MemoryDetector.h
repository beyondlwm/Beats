#ifndef BEATS_DEBUGTOOL_MEMORYDETECTOR_MEMORYDETECTOR_H__INCLUDE
#define BEATS_DEBUGTOOL_MEMORYDETECTOR_MEMORYDETECTOR_H__INCLUDE

#include "MemoryDetectorDef.h"

class CMemoryDetector
{
    BEATS_DECLARE_SINGLETON(CMemoryDetector);

    typedef void* (__cdecl *new_t) (SIZE_T);
    typedef void* (__cdecl *debug_new_t) (SIZE_T, int usage, const char* filePath, int line);
    typedef void (__cdecl *delete_t) (void*);
public:
    void Init(std::vector<std::string>* pMemHookModuleList);
    void Uninit();
    bool IsInit();

    void AddHookModule(const char* moduleName);
    void AddRecord(void* pAddress, size_t size, size_t allocId, size_t usage = 0);
    void DeleteRecord(void* pAddress);
    void GetCallStack(size_t address, std::vector<size_t>& out );

    SMemoryRecord* GetValidRecord(void* pAddress, size_t size, size_t allocTime, size_t allocId, size_t usage);
    void GetRecordCache(const SMemoryFrameRecord* pOriginData, SMemoryFrameRecord& out, EMemoryFrameRecordType type = eMFRT_All, size_t startPos = 0, size_t itemCount = 0);
    size_t GetAllocMemorySize();
    size_t GetAllocMemoryCount();
    size_t GetMemoryBlockMaxSize();
    size_t GetLatestMemoryBlockTime();
    void AddFilterStr(std::string filter);
    void DelFilterStr(std::string filter);
    bool FilterRecordWithEIP(size_t eip);
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
    static LPVOID OnHeapAlloc (SIZE_T size);
    static LPVOID OnHeapAllocDbg (SIZE_T size, int usage, const char* filePath, int line);

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

    void AddCallStackInfo(size_t address);
    bool AddCallStackInfoByEBP(std::vector<size_t>& callStack);

    void DeleteCallStackInfo(size_t address);
    HMODULE GetModuleFromAddress(size_t address);

private:
    static new_t m_allocFunc;
    static debug_new_t m_allocFuncDbg;
    static delete_t m_freeFunc;
    static delete_t m_arrayFreeFunc;

    size_t m_allocMemorySize;
    bool m_processInDebugMode;
    bool m_processInUnicodeMode;
    bool m_initFlag;
    HMODULE m_curModule;

    std::vector<SMemoryRecord*> m_recordPool;
    std::vector<std::string> m_filterList;
    std::vector<std::string> m_hookModuleList;
    std::map<size_t, std::vector<size_t>> m_callStackPool;
    std::map<size_t, bool> m_eipExamMap;
    SMemoryFrameRecord      m_record;
    CRITICAL_SECTION m_CSLock;

};

#endif