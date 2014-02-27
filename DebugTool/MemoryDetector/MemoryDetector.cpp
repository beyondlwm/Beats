#include "stdafx.h"
#include "MemoryDetector.h"
#include "../BDTWxApp.h"
#include "../BeatsDebugTool.h"
#include <DbgHelp.h>
#include <Psapi.h>
#include <algorithm>
#pragma comment(lib, "psapi.lib")

CMemoryDetector::new_t CMemoryDetector::m_allocFunc = NULL;
CMemoryDetector::debug_new_t CMemoryDetector::m_allocFuncDbg = NULL;

CMemoryDetector::delete_t CMemoryDetector::m_freeFunc = NULL;
CMemoryDetector::delete_t CMemoryDetector::m_arrayFreeFunc = NULL;

CMemoryDetector* CMemoryDetector::m_pInstance = NULL;

CMemoryDetector::CMemoryDetector()
: m_allocMemorySize(0)
, m_processInDebugMode(false)
, m_processInUnicodeMode(false)
, m_initFlag(false)
, m_curModule(NULL)
{
    InitializeCriticalSection(&m_CSLock);
    size_t clientMode = static_cast<CBDTWxApp*>(wxApp::GetInstance())->GetClientMode();
    m_processInDebugMode = (clientMode & eCPM_Debug) > 0;
    m_processInUnicodeMode = (clientMode & eCPM_Unicode) > 0;

    size_t curEip;
    BEATS_ASSI_GET_EIP(curEip);
    m_curModule = GetModuleFromAddress(curEip);
}

CMemoryDetector::~CMemoryDetector()
{
    CheckMemoryLeak();
    DeleteCriticalSection(&m_CSLock);
    BEATS_SAFE_DELETE_VECTOR(m_recordPool);
    BEATS_SAFE_DELETE_MAP(m_record.recordMap, TRecordMap);
}

SMemoryRecord* CMemoryDetector::GetValidRecord(void* address, size_t size, size_t allocTime, size_t allocId, size_t usage)
{
    if (m_recordPool.size() > 0)
    {
        SMemoryRecord* resut = m_recordPool.back();
        m_recordPool.pop_back();
        resut->Init(address, size, allocTime, allocId, usage);
        return resut;
    }
    return new SMemoryRecord(address, size, allocTime, allocId, usage );
}

void CMemoryDetector::AddRecord( void* pAddress, size_t size, size_t allocId, size_t usage /*= 0*/ )
{
#ifdef _DEBUG
    bool recordExisting = m_record.recordMap.find(pAddress) != m_record.recordMap.end();
    if (recordExisting)
    {
        DWORD displacement = 0;
        IMAGEHLP_LINE info;
        HANDLE processHandle = GetCurrentProcess();
        SymGetLineFromAddr(processHandle, allocId, &displacement, &info);
        IMAGEHLP_LINE info2;
        SymGetLineFromAddr(processHandle, m_record.recordMap[pAddress]->m_allocId, &displacement, &info2);
#ifdef _UNICODE
        TCHAR filePath1[MAX_PATH] = {0};
        TCHAR filePath2[MAX_PATH] = {0};
        MultiByteToWideChar(CP_ACP, 0, info.FileName, -1, filePath1, MAX_PATH);
        MultiByteToWideChar(CP_ACP, 0, info2.FileName, -1, filePath2, MAX_PATH);
        BEATS_ASSERT(false, _T("Try to alloc an existing address %x when add memory record at \n%s line %d\nbecause it has been alloc at \n%s line %d"), pAddress, filePath1, info.LineNumber, filePath2, info2.LineNumber);
#else
        BEATS_ASSERT(false, _T("Try to alloc an existing address %x when add memory record at \n%s line %d\nbecause it has been alloc at \n%s line %d"), pAddress, info.FileName, info.LineNumber, info2.FileName, info2.LineNumber);
#endif
    }
#endif
    size_t curTime = GetTickCount();
    SMemoryRecord* pNewRecord = GetValidRecord(pAddress, size, curTime, allocId, usage);
    BEATS_ASSERT(size > 0, _T("Alloc Memory size should be greater than 0"));
    m_record.recordMap[pAddress] = pNewRecord;
    m_allocMemorySize += size;
    //for cache
    m_record.recordMapForLocation[allocId].insert(pNewRecord);
    m_record.recordMapForSize[size].insert(pNewRecord);
    m_record.recordMapForTime[curTime].insert(pNewRecord);
}

void CMemoryDetector::DeleteRecord( void* pAddress )
{
    std::map<void*, SMemoryRecord*>::iterator iter = m_record.recordMap.find(pAddress);
    if(iter != m_record.recordMap.end())
    {
        SMemoryRecord* pCurRecord = iter->second;
        BEATS_ASSERT(pCurRecord != NULL, _T("Get an inexistent record!"));
        pCurRecord->m_freeTime = GetTickCount();
        pCurRecord->m_isFree = true;
        pCurRecord->m_freeId = 0;
        m_allocMemorySize -= pCurRecord->m_size;
        m_recordPool.push_back(pCurRecord);
        m_record.recordMap.erase(iter);
        //for cache
        m_record.recordMapForLocation[pCurRecord->m_allocId].erase(pCurRecord);
        if (m_record.recordMapForLocation[pCurRecord->m_allocId].size() == 0)
        {
            m_record.recordMapForLocation.erase(pCurRecord->m_allocId);
        }
        m_record.recordMapForSize[pCurRecord->m_size].erase(pCurRecord);
        if (m_record.recordMapForSize[pCurRecord->m_size].size() == 0)
        {
            m_record.recordMapForSize.erase(pCurRecord->m_size);
        }
        m_record.recordMapForTime[pCurRecord->m_allocTime].erase(pCurRecord);
        if (m_record.recordMapForTime[pCurRecord->m_allocTime].size() == 0)
        {
            m_record.recordMapForTime.erase(pCurRecord->m_allocTime);
        }
    }
}

bool CMemoryDetector::PatchDll(HMODULE importmodule, LPCSTR exportmodulename, LPCSTR exportmodulepath, LPCSTR importname,
                               LPCVOID replacement)
{
    HMODULE exportmodule;
    // Get the *real* address of the import. If we find this address in the IAT,
    // then we've found the entry that needs to be patched.
    if (exportmodulepath != NULL) 
    {
        // Always try to use the full path if available. There seems to be a bug
        // (or is this a new feature of the "side-by-side" kruft?) where
        // GetModuleHandle sometimes fails if the full path is not supplied for
        // DLLs in the side-by-side cache.
        exportmodule = GetModuleHandleA(exportmodulepath);
    }
    else 
    {
        // No full path available. Try using just the module name by itself.
        exportmodule = GetModuleHandleA(exportmodulename);
    }
    FARPROC import;
    BEATS_ASSERT(exportmodule != NULL, _T("exportmodule is null!"));
    import = GetProcAddress(exportmodule, importname);
    BEATS_ASSERT(import != NULL, _T("import is null!")); // Perhaps the named export module does not actually export the named import?
    return ReplaceFunc(importmodule, (char*)exportmodulename, import, replacement);
}

LPVOID CMemoryDetector::OnHeapAlloc( SIZE_T size )
{
    static CMemoryDetector* pMemoryDetector = CMemoryDetector::GetInstance();

    EnterCriticalSection(pMemoryDetector->GetHeapOperationCS());
    void* newSize = m_allocFunc(size);

    size_t curEBPValuel;
    BEATS_ASSI_GET_EBP(curEBPValuel);
    //increase the value to get the Eip value. imagine the stack.
    size_t eip = *((size_t*)curEBPValuel + 1);
    if (!pMemoryDetector->FilterRecordWithEIP(eip))
    {
        pMemoryDetector->AddRecord(newSize, size, eip);
        if (pMemoryDetector->m_processInDebugMode)
        {
            pMemoryDetector->AddCallStackInfo((size_t)newSize);
        }
    }
    LeaveCriticalSection(pMemoryDetector->GetHeapOperationCS());
    return newSize;
}

LPVOID CMemoryDetector::OnHeapAllocDbg (SIZE_T size, int usage, const char* filePath, int line)
{
    static CMemoryDetector* pMemoryDetector = CMemoryDetector::GetInstance();

    EnterCriticalSection(pMemoryDetector->GetHeapOperationCS());
    void* newSize = m_allocFuncDbg(size, usage, filePath, line);

    size_t curEBPValuel;
    BEATS_ASSI_GET_EBP(curEBPValuel);
    //increase the value to get the Eip value. imagine the stack.
    size_t eip = *((size_t*)curEBPValuel + 1);
    if (!pMemoryDetector->FilterRecordWithEIP(eip))
    {
        pMemoryDetector->AddRecord(newSize, size, eip);
        if (pMemoryDetector->m_processInDebugMode)
        {
            pMemoryDetector->AddCallStackInfo((size_t)newSize);
        }
    }
    LeaveCriticalSection(pMemoryDetector->GetHeapOperationCS());
    return newSize;
}

void CMemoryDetector::OnHeapFree( void* pAddress )
{
    static CMemoryDetector* pMemoryDetector = CMemoryDetector::GetInstance();
    EnterCriticalSection(pMemoryDetector->GetHeapOperationCS());
    m_freeFunc(pAddress);
    pMemoryDetector->OnHeapFreeImpl(pAddress);
    LeaveCriticalSection(pMemoryDetector->GetHeapOperationCS());
}


void CMemoryDetector::OnHeapArrayFree( void* pAddress )
{
    static CMemoryDetector* pMemoryDetector = CMemoryDetector::GetInstance();
    EnterCriticalSection(pMemoryDetector->GetHeapOperationCS());
    m_arrayFreeFunc(pAddress);
    pMemoryDetector->OnHeapFreeImpl(pAddress);
    LeaveCriticalSection(pMemoryDetector->GetHeapOperationCS());
}

void CMemoryDetector::OnHeapFreeImpl( void* pAddress )
{
    if (m_record.recordMap.find(pAddress) != m_record.recordMap.end())
    {
        DeleteRecord(pAddress);
        if (m_processInDebugMode)
        {
            DeleteCallStackInfo(size_t(pAddress));
        }
    }
}

void CMemoryDetector::Init( std::vector<std::string>* pMemHookModuleList )
{
    bool bHookCrt = false;
    bool bHookMFC = false;

    for (size_t i = 0; i < pMemHookModuleList->size(); ++i)
    {
       m_hookModuleList.push_back(pMemHookModuleList->at(i));
       const char* pModuleName = (*pMemHookModuleList)[i].c_str();
       bHookMFC = HookMFC(true, pModuleName);
       bHookCrt = HookCRT(true, pModuleName);
    }
    m_initFlag = bHookMFC || bHookCrt;

    BEATS_ASSERT(m_initFlag, _T("Failed to init Memory detector!"));
}

bool CMemoryDetector::CheckMemoryLeak()
{
    size_t recordTotalSize = 0;
    for(std::map<void*, SMemoryRecord*>::iterator iter = m_record.recordMap.begin(); iter != m_record.recordMap.end(); ++iter)
    {
        recordTotalSize += iter->second->m_size;
    }
    BEATS_ASSERT(recordTotalSize == m_allocMemorySize,
        _T("Data is not sync when check the memory leak! cache value %d record sum value %d!"),
        m_allocMemorySize, recordTotalSize);
    if (m_allocMemorySize > 0)
    {
        BEATS_PRINT(_T("\n\n***********BEATS_MEMORY_DETECTOR**********\n"));
        BEATS_PRINT(_T("Memory leak detected! size: %d\n"), m_allocMemorySize);
        BEATS_PRINT(_T("******************************************\n\n"));
        DWORD displacement = 0;
        IMAGEHLP_LINE info;
        HANDLE processHandle = GetCurrentProcess();
        size_t counter = 0;
        for(std::map<void*, SMemoryRecord*>::iterator iter = m_record.recordMap.begin(); iter != m_record.recordMap.end(); ++iter, ++counter)
        {
            BEATS_PRINT(_T("\n---------Block:%d-----------\n"), counter);

            std::vector<size_t> callStack;
            GetCallStack((size_t)(iter->first), callStack);
            for (size_t i = 0; i < callStack.size(); ++i)
            {
                bool ret = SymGetLineFromAddr(processHandle, callStack[i], &displacement, &info) == TRUE;
                if (ret)
                {
#ifdef _UNICODE
                    TCHAR temp[MAX_PATH];
                    MultiByteToWideChar(CP_ACP, 0, info.FileName, -1, temp, MAX_PATH);
                    BEATS_PRINT(_T("%s line:%d\n"),temp, info.LineNumber);
#else
                    BEATS_PRINT(_T("%s line:%d\n"), info.FileName, info.LineNumber);
#endif
                }
                BEATS_SAFE_DELETE(iter->second->m_pAddress);
            }
            BEATS_PRINT(_T("\n--------------------------\n"));

        }
    }
    return m_allocMemorySize == 0;
}
// return data in sync mode for other thread.
void CMemoryDetector::GetRecordCache(const SMemoryFrameRecord* pOriginData, SMemoryFrameRecord& out, EMemoryFrameRecordType type, size_t startPos, size_t itemCount)
{
    EnterCriticalSection(&m_CSLock);
    if (pOriginData == NULL)
    {
        pOriginData = &m_record;
    }
    out.Clear();
    out.type = type;
    out.totalItemCount = pOriginData->recordMap.size();
    out.startPos = startPos;

    switch (type)
    {
    case eMFRT_All:
        out = *pOriginData;
        break;
    case eMFRT_Location:
        {
            if (startPos < pOriginData->recordMapForLocation.size())
            {
                TRecordU32Map::const_iterator startIter = pOriginData->recordMapForLocation.begin();
                TRecordU32Map::const_iterator endIter = pOriginData->recordMapForLocation.end();

                for (size_t counter = 0; counter < startPos; ++counter)
                {
                    ++startIter;
                }
                
                for (size_t counter = 0; (itemCount == 0 || counter < itemCount) && (startIter != endIter); ++counter )
                {
                    out.recordMapForLocation[startIter->first] = startIter->second;
                    ++startIter;
                }
            }
        }
        break;
    case eMFRT_Address:
        {
            if (startPos < pOriginData->recordMap.size())
            {
                TRecordMap::const_iterator startIter = pOriginData->recordMap.begin();
                TRecordMap::const_iterator endIter = pOriginData->recordMap.end();

                for (size_t counter = 0; counter < startPos; ++counter)
                {
                    ++startIter;
                }

                for (size_t counter = 0; (itemCount == 0 || counter < itemCount) && (startIter != endIter); ++counter )
                {
                    out.recordMap[startIter->first] = startIter->second;
                    ++startIter;
                }
            }
        }
        break;
    case eMFRT_Size:
        {
            if (startPos < pOriginData->recordMap.size())
            {
                TRecordU32Map::const_iterator startIter = pOriginData->recordMapForSize.begin();
                TRecordU32Map::const_iterator endIter = pOriginData->recordMapForSize.end();
                size_t counter = 0;
                for (; (counter + startIter->second.size()) < startPos; ++startIter)
                {
                    counter += startIter->second.size();
                }
                for (; startIter != endIter; ++startIter)
                {
                    std::set<SMemoryRecord*>::const_iterator subIter = startIter->second.begin();
                    std::set<SMemoryRecord*>::const_iterator subEndIter = startIter->second.end();
                    out.recordMapForSize[startIter->first] = std::set<SMemoryRecord*>();
                    for (; subIter != subEndIter; ++subIter)
                    {
                        if (counter >= startPos && (itemCount == 0 || counter < (itemCount + startPos)))
                        {
                            out.recordMapForSize[startIter->first].insert(*subIter);
                        }
                        ++counter;
                    }
                }
            }
        }
        break;
    case eMFRT_Time:
        if (startPos < pOriginData->recordMap.size())
        {
            TRecordU32Map::const_iterator startIter = pOriginData->recordMapForTime.begin();
            TRecordU32Map::const_iterator endIter = pOriginData->recordMapForTime.end();
            size_t counter = 0;
            for (; (counter + startIter->second.size()) < startPos; ++startIter)
            {
                counter += startIter->second.size();
            }
            for (; startIter != endIter; ++startIter)
            {
                std::set<SMemoryRecord*>::const_iterator subIter = startIter->second.begin();
                std::set<SMemoryRecord*>::const_iterator subEndIter = startIter->second.end();
                out.recordMapForTime[startIter->first] = std::set<SMemoryRecord*>();
                for (; subIter != subEndIter; ++subIter)
                {
                    if (counter >= startPos && (itemCount == 0 || counter < (itemCount + startPos)))
                    {
                        out.recordMapForTime[startIter->first].insert(*subIter);
                    }
                    ++counter;
                }
            }
        }
        break;
    default:
        BEATS_ASSERT(false, _T("Never reach here!"));
        break;
    }
    LeaveCriticalSection(&m_CSLock);
}

size_t CMemoryDetector::GetAllocMemorySize()
{
    EnterCriticalSection(&m_CSLock);
    size_t result = m_allocMemorySize;
    LeaveCriticalSection(&m_CSLock);
    return result;
}

size_t CMemoryDetector::GetAllocMemoryCount()
{
    EnterCriticalSection(&m_CSLock);
    size_t result = m_record.recordMap.size();
    LeaveCriticalSection(&m_CSLock);
    return result;
}

size_t CMemoryDetector::GetMemoryBlockMaxSize()
{
    EnterCriticalSection(&m_CSLock);
    size_t result = m_record.recordMapForSize.size() > 0 ? m_record.recordMapForSize.rbegin()->first : 0;
    LeaveCriticalSection(&m_CSLock);
    return result;
}

size_t CMemoryDetector::GetLatestMemoryBlockTime()
{
    EnterCriticalSection(&m_CSLock);
    size_t result = m_record.recordMapForTime.size() > 0 ? m_record.recordMapForTime.rbegin()->first : 0;
    LeaveCriticalSection(&m_CSLock);
    return result;
}

bool CMemoryDetector::HookCRT(bool patchOrRestore, const char* hookModuleName)
{
    char* crtName;
    char* mfcName;
    GetHookModuleName(crtName, mfcName);

    const char* newSymbol = "??2@YAPAXI@Z";
    const char* newDbgSymbol = "??2@YAPAXIHPBDH@Z";

    const char* delSymbol = "??3@YAXPAX@Z";
    const char* delArraySymbol = "??_V@YAXPAX@Z";

    return HookImpl(patchOrRestore, hookModuleName, crtName, newSymbol, newDbgSymbol, delSymbol, delArraySymbol, &OnHeapAlloc, &OnHeapFree);
}

bool CMemoryDetector::HookMFC(bool patchOrRestore, const char* hookModuleName)
{
    char* crtName;
    char* mfcName;
    GetHookModuleName(crtName, mfcName);
    const char* newSymbol = (LPCSTR)895;
    const char* delSymbol = (LPCSTR)901;
    return HookImpl(patchOrRestore, hookModuleName, mfcName, newSymbol, NULL, delSymbol, NULL, &OnHeapAlloc, &OnHeapFree);
}

bool CMemoryDetector::InitRunTimeLib(const char* hookModuleName,
                                     const char* crtModuleName,
                                     const char* newSymbol,
                                     const char* newDbgSymbol,
                                     const char* delSymbol,
                                     const char* arrayDelSymbol,
                                     const new_t newFunc, 
                                     const delete_t delFunc,
                                     const delete_t arrayDeleteFunc)
{   
    bool result = false;
    HMODULE allocatorModule = GetModuleHandleA(crtModuleName);//force use multi-byte.
    if (allocatorModule != NULL)
    {
        m_allocFunc = (new_t)GetProcAddress(allocatorModule, newSymbol);
        m_allocFuncDbg = (debug_new_t)GetProcAddress(allocatorModule, newDbgSymbol);
        m_freeFunc = (delete_t)GetProcAddress(allocatorModule, delSymbol);
        m_arrayFreeFunc = (delete_t)GetProcAddress(allocatorModule, arrayDelSymbol);

        HMODULE hookModule = GetModuleHandleA(hookModuleName);
        BEATS_ASSERT(hookModule != NULL, _T("Hook Module failed : %s"), hookModuleName);
        bool bPatchNew = PatchDll(hookModule, crtModuleName, NULL, newSymbol, newFunc);
        bool bPatchDbgNew = PatchDll(hookModule, crtModuleName, NULL, newDbgSymbol, newFunc);
        bool bPatchDelete = PatchDll(hookModule, crtModuleName, NULL, delSymbol, delFunc);
        bool bPatchArrayDelete = PatchDll(hookModule, crtModuleName, NULL, arrayDelSymbol, arrayDeleteFunc);
        result = bPatchNew || bPatchDelete || bPatchArrayDelete || bPatchDbgNew;
    }
    return result;
}

void CMemoryDetector::Uninit()
{
    if (m_initFlag)
    {
        m_allocMemorySize = 0;
        BEATS_SAFE_DELETE_VECTOR(m_recordPool);
        BEATS_SAFE_DELETE_MAP(m_record.recordMap, TRecordMap);
        m_record.recordMapForLocation.clear();
        m_record.recordMapForSize.clear();
        m_record.recordMapForTime.clear();

        for (size_t i = 0; i < m_hookModuleList.size(); ++i)
        {
            const char* pModuleName = m_hookModuleList[i].c_str();
            HookMFC(false, pModuleName);
            HookCRT(false, pModuleName);
        }
        HookMFC(false, NULL);
        HookCRT(false, NULL);

        m_initFlag = false;
    }
}

bool CMemoryDetector::IsInit()
{
    return m_initFlag;
}

bool CMemoryDetector::ReplaceFunc( HMODULE hookModule, const char* exprotModuleName, LPCVOID targetFunc, LPCVOID replaceFunc )
{
    bool ret = false;

    IMAGE_IMPORT_DESCRIPTOR *idte = GetIDTE(hookModule, exprotModuleName);
    if (idte != NULL)
    {
        DWORD protect;
        // Locate the import's IAT entry. this is the address of function address tables
        size_t** ppFunc = (size_t**)R2VA(hookModule, idte->FirstThunk);
        size_t* pFunc = *ppFunc;
        while (pFunc != NULL) 
        {
            if (pFunc == (size_t*)targetFunc) 
            {
                // Found the IAT entry. Overwrite the address stored in the IAT
                // entry with the address of the replacement. Note that the IAT
                // entry may be write-protected, so we must first ensure that it is
                // writable.
                if (replaceFunc != NULL)
                {
                    VirtualProtect(ppFunc, sizeof(ppFunc), PAGE_READWRITE, &protect);
                    *ppFunc = (size_t*)replaceFunc;
                    VirtualProtect(ppFunc, sizeof(ppFunc), protect, &protect);
                }
                // The patch has been installed in the import module.
                ret = true;
                break;
            }
            ++ppFunc;
            pFunc = *ppFunc;
        }
    }

    return ret;
}

IMAGE_IMPORT_DESCRIPTOR* CMemoryDetector::GetIDTE( HMODULE importmodule, const char* exportmodulename )
{
    IMAGE_SECTION_HEADER    *section;
    ULONG                    size;

    BEATS_ASSERT(exportmodulename != NULL, _T("export module name is NULL!"));

    // Locate the importing module's Import Directory Table (IDT) entry for the
    // exporting module. The importing module actually can have several IATs --
    // one for each export module that it imports something from. The IDT entry
    // gives us the offset of the IAT for the module we are interested in.
    IMAGE_IMPORT_DESCRIPTOR* idte = (IMAGE_IMPORT_DESCRIPTOR*)ImageDirectoryEntryToDataEx((PVOID)importmodule, TRUE,
        IMAGE_DIRECTORY_ENTRY_IMPORT, &size, &section);
    //it must import something so it can't be null.
    if (idte != NULL) 
    {
        while (idte->OriginalFirstThunk != 0x0) 
        {
            PCHAR temp = (PCHAR)R2VA(importmodule, idte->Name);
            if (_stricmp(temp, exportmodulename) == 0) 
            {
                // Found the IDT entry for the exporting module.
                break;
            }
            idte++;
        }
        if (idte->OriginalFirstThunk == 0x0) {
            // The importing module does not import anything from the exporting
            // module.
            idte = NULL;
        }
    }
    return idte;
}

bool CMemoryDetector::HookImpl( bool patchOrRestore,
                               const char* hookModuleName,
                               const char* crtModuleName,
                               const char* newSymbol,
                               const char* newDbgSymbol,
                               const char* delSymbol,
                               const char* arrayDelSymbol,
                               const new_t /*newFunc*/, 
                               const delete_t /*delFunc*/ )
{
    bool ret = false;
    if (patchOrRestore)
    {
        ret = InitRunTimeLib(hookModuleName,
            crtModuleName, 
            newSymbol,
            newDbgSymbol,
            delSymbol,
            arrayDelSymbol,
            &OnHeapAlloc,
            &OnHeapFree,
            &OnHeapArrayFree);
    }
    else
    {
        HMODULE hookModule = GetModuleHandleA(hookModuleName);
        BEATS_ASSERT(hookModule != NULL, _T("Module is null :%s"), hookModuleName);
        // Locate the import's IAT entry.
        ret = ReplaceFunc(hookModule, crtModuleName, &OnHeapAlloc, m_allocFunc);
        ret = ret && ReplaceFunc(hookModule, crtModuleName, &OnHeapFree, m_freeFunc);
    }
    return ret;
}

void CMemoryDetector::AddFilterStr( std::string filter )
{
    std::transform(filter.begin(), filter.end(), filter.begin(), tolower);
    m_filterList.push_back(filter);
}

void CMemoryDetector::DelFilterStr( std::string filter )
{
    std::transform(filter.begin(), filter.end(), filter.begin(), tolower);
    for (size_t i = 0; i < m_filterList.size(); ++i)
    {
        if (m_filterList[i].compare(filter) == 0)
        {
            m_filterList[i] = m_filterList.back();
            m_filterList.pop_back();
        }
    }
}

const std::vector<std::string>& CMemoryDetector::GetFilterStr()
{
    return m_filterList;
}

void CMemoryDetector::AddCallStackInfo(size_t address)
{
    BEATS_ASSERT(m_callStackPool.find(address) == m_callStackPool.end(),
        _T("The address 0x%x is already in call stack pool!"), address);
    std::vector<size_t>& callstack = m_callStackPool[address];
    callstack.clear();
    AddCallStackInfoByEBP(callstack);
    //the last 2 elements are invalid, reason is unknown.
    for (size_t i = 0; i < callstack.size() && i < 2; ++i)
    {
        callstack.pop_back();
    }
}

void CMemoryDetector::DeleteCallStackInfo( size_t address )
{
    BEATS_ASSERT (m_callStackPool.find(address) != m_callStackPool.end(), _T("Call stack data is not existing."));
    m_callStackPool.erase(address);
}

void CMemoryDetector::GetCallStack( size_t address, std::vector<size_t>& out )
{
    BEATS_ASSERT(m_callStackPool.find(address) != m_callStackPool.end(),
        _T("Can't Find the address 0x%x in call stack pool!"), address);
    EnterCriticalSection(&m_CSLock);
    out = m_callStackPool[address];
    LeaveCriticalSection(&m_CSLock);
}

bool CMemoryDetector::AddCallStackInfoByEBP(std::vector<size_t>& callStack)
{
    size_t eBPValue = 0;
    BEATS_ASSI_GET_EBP(eBPValue);
    size_t eIPValue = (*(size_t*)((size_t*)(eBPValue) + 1));//current eip,ignore it.
    size_t lastEBPValue = *(size_t*)(eBPValue);
    size_t ignoreFrameCount = 2;
    while (eIPValue != 0)
    {
        ignoreFrameCount == 0 ? callStack.push_back(eIPValue) : --ignoreFrameCount;

        lastEBPValue = *(size_t*)(eBPValue);
        // If the module enable FPO optimize, the address may be invalid.
        eBPValue = lastEBPValue;
        size_t* pPtr = (size_t*)((size_t*)(eBPValue) + 1);
        if (lastEBPValue == 0 || IsBadCodePtr((FARPROC)pPtr))
        {
            break;
        }
        eIPValue = (*pPtr);
    }
    return callStack.size() > 0;
}

void CMemoryDetector::SetProcessDebugFlag( bool inDebug )
{
    m_processInDebugMode = inDebug;
}

bool CMemoryDetector::FilterRecordWithEIP( size_t eip )
{
    std::map<size_t, bool>::iterator iter = m_eipExamMap.find(eip);
    if (iter == m_eipExamMap.end())
    {
        DWORD displacement = 0;
        IMAGEHLP_LINE info;
        HANDLE processHandle = GetCurrentProcess();
        bool ret = SymGetLineFromAddr(processHandle, eip, &displacement, &info) == TRUE;
        //hard code to filter the call from stl container to avoid waste time.
        //For example we will call alloc and dealloc once when we call push_back of vector eachtime
        ret = ret && strstr(info.FileName, "xmemory") && (info.LineNumber == 44);
        m_eipExamMap[eip] = ret;        
    }
    return m_eipExamMap[eip];
}

void CMemoryDetector::AddHookModule( const char* moduleName )
{
    m_hookModuleList.push_back(moduleName);
}

bool CMemoryDetector::GetHookModuleName( char*& crtLibName, char*& mfcLibName )
{
    if (m_processInDebugMode)
    {
        mfcLibName = m_processInUnicodeMode ? "mfc80ud.dll" : "mfc80d.dll";
        crtLibName = "msvcr80d.dll";
    }
    else
    {
        mfcLibName = m_processInUnicodeMode ? "mfc80u.dll" : "mfc80.dll";
        crtLibName = "msvcr80.dll";
    }
    return true;
}

bool CMemoryDetector::IsHookableModule( HMODULE module )
{
    bool bRet = false;
    // Don't hook itself.
    if (module != m_curModule)
    {
        char* crtName;
        char* mfcName;
        GetHookModuleName(crtName, mfcName);
        const char* crtNewSymbol = "??2@YAPAXI@Z";
        const char* crtNewDbgSymbol = "??2@YAPAXIHPBDH@Z";
        const char* mfcNewSymbol = (LPCSTR)895;
        HMODULE crtModule = GetModuleHandleA(crtName);
        HMODULE mfcModule = GetModuleHandleA(mfcName);
        bool bIsRuntimeModule = (module == crtModule) || (module == mfcModule);
        if ((crtModule || mfcModule) && !bIsRuntimeModule)
        {
            FARPROC crtNewFunc = (FARPROC)GetProcAddress(crtModule, crtNewSymbol);
            FARPROC crtNewDbgFunc = (FARPROC)GetProcAddress(crtModule, crtNewDbgSymbol);
            FARPROC mfcNewFunc = (FARPROC)GetProcAddress(mfcModule, mfcNewSymbol);
            bRet = ReplaceFunc(module, crtName, crtNewFunc, NULL) || ReplaceFunc(module, mfcName, mfcNewFunc, NULL) || ReplaceFunc(module, crtName, crtNewDbgFunc, NULL);
        }
    }

    return bRet;
}

HMODULE CMemoryDetector::GetModuleFromAddress( size_t address )
{
    MEMORY_BASIC_INFORMATION m = { 0 };
    VirtualQuery((void*)address, &m, sizeof(MEMORY_BASIC_INFORMATION));
    return (HMODULE) m.AllocationBase;
}

CRITICAL_SECTION* CMemoryDetector::GetHeapOperationCS()
{
    return &m_CSLock;
}