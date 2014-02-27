#ifndef BEATS_AI_FSM_H__INCLUDE
#define BEATS_AI_FSM_H__INCLUDE


enum EFSMRunType
{
    eFSMRT_Enter,
    eFSMRT_Update,
    eFSMRT_Exit,

    eFSMRT_Count,
    eFSMRT_ForceTo32Bit = 0xffffffff
};

template<typename T>
class CFSM
{
    typedef void(T::*FSMState)(EFSMRunType);

public:
    CFSM();
    ~CFSM();

    bool InitState(T* owner, FSMState state, const TString& stateName);
    void Update();
    void Exit();

    void SwitchToState(FSMState state);
    void SetRunType(EFSMRunType runtype);

    void SetOwner(T* owner);
    T* GetOwner();

    void SetCurrentStateName(const TString& name){m_stateNameForPrint = name;}
    TString& GetCurrentStateName(){return m_stateNameForPrint;}

    bool Reverse();
    bool IsInState(FSMState state);

private:
    void PrintInfo();

private:
    T* m_pOwner;
    bool m_printInfo;
    EFSMRunType m_runType;
    FSMState m_pCurrentState;
    FSMState m_pPrevState;
    FSMState m_pInitState;
    TString m_stateNameForPrint;
};

#include "FSM.hpp"

/// Marco
#define DECLARE_FSM(ClassName)\
        CFSM<ClassName> m_fsm;\
        public: CFSM<ClassName>* GetFSM(){return &m_fsm;}\
        private:

#define DECLARE_FSM_STATE(ClassName, stateName)\
    void fsm##stateName(EFSMRunType);\
    public: void SwitchTo##stateName(){m_fsm.SwitchToState(&ClassName::fsm##stateName);}\
    private:

#define INIT_FSM(ClassName, firstState)\
    m_fsm.InitState(this, &ClassName::fsm##firstState, _T(#firstState))

#define UPDATE_FSM() m_fsm.Update()

#define SWITCH_FSM_STATE(Condition, ClassName, stateName)\
    if ((Condition)) {m_fsm.SwitchToState(&ClassName::fsm##stateName);}

#define EXIT_FSM() m_fsm.Exit();m_fsm.SetRunType(eFSMRT_Enter)

#define REVERSE_FSM() m_fsm.Reverse();

#define IS_IN_STATE(ClassName, stateName) m_fsm.IsInState(&ClassName::fsm##stateName)

//////////////////////////////////////////////////////////////////////////
/// implemention
#define IMPL_FSM_BEGIN(ClassName, stateName)\
    void ClassName::fsm##stateName(EFSMRunType state){\
    if(state == eFSMRT_Enter) m_fsm.SetCurrentStateName(_T(#stateName));

#define IMPL_FSM_ENTER\
    if(state == eFSMRT_Enter)

#define IMPL_FSM_UPDATE\
    else if(state == eFSMRT_Update)

#define IMPL_FSM_EXIT\
    else if(state == eFSMRT_Exit)

#define IMPL_FSM_END }

#endif