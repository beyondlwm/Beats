// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 SPY_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// BEATS_SPY_DLL_DECL 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。


#ifdef __cplusplus
extern "C" {
#endif

#ifdef _USRDLL
#ifdef SPY_EXPORTS
#define BEATS_SPY_DLL_DECL __declspec(dllexport)
#else
#define BEATS_SPY_DLL_DECL __declspec(dllimport)
#endif
#else
#define BEATS_SPY_DLL_DECL
#endif

struct SBeatsSocket;
class CFileFilter;
BEATS_SPY_DLL_DECL bool Init(void);
BEATS_SPY_DLL_DECL bool Uninit(void);
BEATS_SPY_DLL_DECL const hostent* Spy_GetHostInfo();
BEATS_SPY_DLL_DECL const SBeatsSocket* Spy_Connect(uint32_t uIPAddress);
BEATS_SPY_DLL_DECL bool Spy_GetNetAdapterInfo(std::string& strInfo);


typedef void(*TCommandFeedBackFunc)(const char* pszFeedBackInfo, void* pUserDataPtr);
BEATS_SPY_DLL_DECL bool Spy_SendSystemCommand(SOCKET sock, const char* pszCmd, TCommandFeedBackFunc pFeedbackFunc, void* pUserDataPtr);

typedef void(*TFileInfoFeedbackFunc)(char m_type, void* pFileInfo, void* pUserDataPtr);
BEATS_SPY_DLL_DECL bool Spy_SendFileInfoCommand(SOCKET sock, const TCHAR* pszCmd, TFileInfoFeedbackFunc pFeedbackFunc, void* pUserDataPtr);
BEATS_SPY_DLL_DECL bool Spy_SendMessageCommand(SOCKET sock, const TCHAR* pszMsg, const TCHAR* pszCapital);

BEATS_SPY_DLL_DECL bool Spy_DownloadFiles(SOCKET sock, const std::vector<TString>& files, const TString& strStorePath, CFileFilter* pFilter);
BEATS_SPY_DLL_DECL bool Spy_UploadFiles(SOCKET sock, const std::vector<TString>& files, const TString& strStorePath, CFileFilter* pFilter);

#ifdef __cplusplus
}
#endif
