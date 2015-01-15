#define SIGNAL_NAME GEN_NAME(Signal, SIGNAL_PARAM_COUNT)

#if SIGNAL_PARAM_COUNT == 0
#define SIGNAL_COMMA
#else
#define SIGNAL_COMMA ,
#endif

// Signal Implementation
template< typename R SIGNAL_COMMA GEN_PARAM( SIGNAL_PARAM_COUNT, typename Arg ) >
class SIGNAL_NAME
{
public:

    typedef Function< R ( GEN_PARAM( SIGNAL_PARAM_COUNT, Arg ) ) > FunctionType;
    typedef std::set< FunctionType > FunctionSet;

    void operator()( GEN_PARAM_EX( SIGNAL_PARAM_COUNT, Arg, arg ) )
    {
        for (typename FunctionSet::iterator iter = m_functionSet.begin(); iter != m_functionSet.end(); ++iter)
        {
            FunctionType* pFunc = (FunctionType*)&(*iter); // Avoid call copy constructor
            (*pFunc)( GEN_PARAM( SIGNAL_PARAM_COUNT, arg ) );
        }
    }

    void Connect( const FunctionType& func )
    {
        m_functionSet.insert( func );
    }

    void Disconnect( const FunctionType& func )
    {
        BEATS_ASSERT(m_functionSet.find(func) != m_functionSet.end(), _T("Can't find function to disconnect!"));
        m_functionSet.erase( func );
    }

    bool IsConnected( const FunctionType& func ) const
    {
        bool isFind = m_functionSet.find(func) != m_functionSet.end();

        return isFind;
    }

    uint32_t GetConnectedCount() const
    {
        return m_functionSet.size();
    }

private:

    FunctionSet m_functionSet;
};


///////////////////////////////////////////////////////////////////////////////


template< typename Signature >                                                                      
struct SignalImpl< SIGNAL_PARAM_COUNT, Signature >                                                     
{                                                                                                   
    typedef SIGNAL_NAME                                                                   
        <                                                                                           
            typename FunctionTraits< Signature >::ReturnType SIGNAL_COMMA                                
            GEN_PARAM( SIGNAL_PARAM_COUNT, typename FunctionTraits< Signature >::ArgType ) 
        > Type;                                                                                     
};


#undef SIGNAL_NAME
#undef SIGNAL_COMMA
