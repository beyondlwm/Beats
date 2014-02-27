#ifndef BEATS_GAME_GAME_H__INCLUDE
#define BEATS_GAME_GAME_H__INCLUDE

#include "../ai/FSM.h"
#include "Tasks/TaskDef.h"

class CTaskBase;

class CGame
{
    BEATS_DECLARE_SINGLETON(CGame);

public:
    void Init(HWND hwnd);
    bool Update();
    HWND GetHwnd();
    void SetActive(bool active);
    void RegisterTask(int type, CTaskBase* taskBase);

    void SetAudioPath(const TString& path);
    void SetAudioName(const TString& name);

    void SetAudioFullPath(const TString& fullPath);
    void SetBRFFilePath(const TString& fullPath);
    const TString& GetAudioFullPath()const ;
    const TString& GetBRFFilePath()const;

    float GetDeltaTime() const;

    void SetTask(ETaskType type);

    void Exit();
    long GetFrameCount() const;

    const TString& GetWorkRootPath() const;

private:
    HWND m_hwnd;
    bool m_active;
    float m_deltaTimeMs;
    long m_frameCounter;
    CTaskBase* m_pCurTask;
    TString m_audioFullPath;
    TString m_brfFilePath;
    TString m_strWorkRootPath;
    std::map<int, CTaskBase*> m_tasks;
};

#endif
