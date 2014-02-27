
template<typename T>
CFSM<T>::CFSM()
: m_pOwner(NULL)
, m_runType(eFSMRT_Enter)
, m_printInfo(true)
, m_pCurrentState(NULL)
, m_pPrevState(NULL)
, m_pInitState(NULL)
{

}

template<typename T>
CFSM<T>::~CFSM()
{

}

template<typename T>
bool CFSM<T>::InitState( T* owner, FSMState state, const TString& stateName )
{
    m_pCurrentState = state;
    m_pInitState = state;
    SetOwner(owner);
    SetCurrentStateName(stateName);
    return true;
}

template<typename T>
void CFSM<T>::Update()
{
    BEATS_ASSERT(m_pOwner != NULL, _T("Init FSM first before update!"));
    if (m_runType == eFSMRT_Exit)
    {
        (m_pOwner->*m_pPrevState)(eFSMRT_Exit);
        PrintInfo();
        m_runType = eFSMRT_Enter;
    }
    else
    {
        (m_pOwner->*m_pCurrentState)(m_runType);
        PrintInfo();
        if (m_runType != eFSMRT_Exit)//done a state traverse
        {
            m_runType = eFSMRT_Update;
        }
    }
}

template<typename T>
void CFSM<T>::PrintInfo()
{
    if (m_printInfo)
    {
        static EFSMRunType lastState = eFSMRT_ForceTo32Bit;
        if (lastState != m_runType)
        {
            TString state = m_runType == eFSMRT_Exit ? _T("Exiting") : 
                                m_runType == eFSMRT_Enter ? _T("Entering") : _T("Updating");
            BEATS_PRINT(_T("FSM: State %s is %s. FSM Owner: 0x%x\n"), m_stateNameForPrint.c_str(), state.c_str(), m_pOwner);
            lastState = m_runType;
        }
    }
}

template<typename T>
void CFSM<T>::SwitchToState( FSMState state )
{
    if (m_pCurrentState != state)
    {
        m_pPrevState = m_pCurrentState;
        m_pCurrentState = state;
        m_runType = eFSMRT_Exit;
    }
}

template<typename T>
void CFSM<T>::SetOwner( T* pOwner )
{
    m_pOwner = pOwner;
}

template<typename T>
T* CFSM<T>::GetOwner()
{
    BEATS_ASSERT(m_pOwner != NULL, _T("Owner is unset!"));
    return m_pOwner;
}

template<typename T>
bool CFSM<T>::Reverse()
{
    bool result = false;
    if (m_pPrevState != NULL)
    {
        SwitchToState(m_pPrevState);
        result = true;
    }
    return result;
}

template<typename T>
void CFSM<T>::Exit()
{
    if (m_pCurrentState != NULL)
    {
        (m_pOwner->*m_pCurrentState)(eFSMRT_Exit);
        m_pCurrentState = NULL;//avoid call FSM_UPDATE after we exit.
    }
}

template<typename T>
void CFSM<T>::SetRunType( EFSMRunType runtype )
{
    m_runType = runtype;
}

template<typename T>
bool CFSM<T>::IsInState( FSMState state )
{
    return m_pCurrentState == state;
}