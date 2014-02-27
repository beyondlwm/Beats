#include "stdafx.h"
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef __WXMSW__
#error "This sample is MSW-only"
#endif

#include "wx/dynlib.h"
#include <process.h> // for _beginthreadex()
#include "BDTWxApp.h"
#include "BDTWxFrame.h"
#include "PerformDetector/PerformDetector.h"
#include "MemoryDetector/MemoryDetector.h"
#include "BeatsDebugTool.h"
#include "ComponentSystem/wxWidgetsProperty/wxWidgetsPropertyBase.h"
#include <dbghelp.h>
#include <psapi.h>

// Handle of wx "main" thread if running, NULL otherwise
HANDLE gs_wxMainThread = NULL;

// ----------------------------------------------------------------------------
// application startup
// ----------------------------------------------------------------------------

// we can't have WinMain() in a DLL and want to start the app ourselves
IMPLEMENT_APP_NO_MAIN(CBDTWxApp)

namespace
{
    // Critical section that guards everything related to wxWidgets "main" thread
    // startup or shutdown.
    wxCriticalSection gs_wxStartupCS;


    //  wx application startup code -- runs from its own thread
    unsigned wxSTDCALL MyAppLauncher(void* event)
    {
        // Note: The thread that called run_wx_gui_from_dll() holds gs_wxStartupCS
        //       at this point and won't release it until we signal it.

        // We need to pass correct HINSTANCE to wxEntry() and the right value is
        // HINSTANCE of this DLL, not of the main .exe, use this MSW-specific wx
        // function to get it. Notice that under Windows XP and later the name is
        // not needed/used as we retrieve the DLL handle from an address inside it
        // but you do need to use the correct name for this code to work with older
        // systems as well.
        const HINSTANCE
            hInstance = wxDynamicLibrary::MSWGetModuleHandle("DebugTool",
            &gs_wxMainThread);
        BEATS_ASSERT(hInstance != NULL, _T("failed to get DLL's handle"));

        // IMPLEMENT_WXWIN_MAIN does this as the first thing
        wxDISABLE_DEBUG_SUPPORT();

        // We do this before wxEntry() explicitly, even though wxEntry() would
        // do it too, so that we know when wx is initialized and can signal
        // run_wx_gui_from_dll() about it *before* starting the event loop.
        wxInitializer wxinit;

        bool initResult = wxinit.IsOk();
        BEATS_ASSERT(initResult, _T("failed to Init wxwidgets!"));

        // Signal run_wx_gui_from_dll() that it can continue
        HANDLE hEvent = *(static_cast<HANDLE*>(event));
        BOOL setEventResult = SetEvent(hEvent);
        BEATS_ASSERT(setEventResult == 1, _T("failed setting up the mutex!"));

        // Run the app:
        wxEntry(hInstance);

        return 1;
    }

} // anonymous namespace

// ----------------------------------------------------------------------------
// public DLL interface
// ----------------------------------------------------------------------------

extern "C"
{

    bool BEATS_INIT_DEBUG_TOOL_FUNC()
    {
        BEATS_ASSERT(CBDTWxApp::m_bInit == false);
        // In order to prevent conflicts with hosting app's event loop, we
        // launch wx app from the DLL in its own thread.
        //
        // We can't even use wxInitializer: it initializes wxModules and one of
        // the modules it handles is wxThread's private module that remembers
        // ID of the main thread. But we need to fool wxWidgets into thinking that
        // the thread we are about to create now is the main thread, not the one
        // from which this function is called.
        //
        // Note that we cannot use wxThread here, because the wx library wasn't
        // initialized yet. wxCriticalSection is safe to use, though.

        wxCriticalSectionLocker lock(gs_wxStartupCS);

        if ( !gs_wxMainThread )
        {
            HANDLE hEvent = CreateEvent
                (
                NULL,  // default security attributes
                FALSE, // auto-reset
                FALSE, // initially non-signaled
                NULL   // anonymous
                );
            if ( !hEvent )
                return false; // error

            // NB: If your compiler doesn't have _beginthreadex(), use CreateThread()
            gs_wxMainThread = (HANDLE)_beginthreadex
                (
                NULL,           // default security
                0,              // default stack size
                &MyAppLauncher,
                &hEvent,        // arguments
                0,              // create running
                NULL
                );

            if ( !gs_wxMainThread )
            {
                CloseHandle(hEvent);
                BEATS_ASSERT(false, _T("Create Thread Failed!"));
            }

            // Wait until MyAppLauncher signals us that wx was initialized. This
            // is because we use wxMessageQueue<> and wxString later and so must
            // be sure that they are in working state.
            WaitForSingleObject(hEvent, INFINITE);
            CloseHandle(hEvent);
        }
        CBDTWxApp* pWxApp = static_cast<CBDTWxApp*>(wxApp::GetInstance());

        // Send a message to wx thread to show a new frame:
        wxThreadEvent *event = new wxThreadEvent(wxEVT_THREAD, CBDTWxApp::CMD_SHOW_WINDOW);
        event->SetString(("StartUpFrame"));
        wxQueueEvent(wxApp::GetInstance(), event);

        // Wait for CStartupFrame to set the launch parameter.
        WaitForSingleObject(pWxApp->GetLaunchEvent(), INFINITE);
        return CBDTWxApp::m_bInit;
    }

    void BEATS_UNINIT_DEBUG_TOOL_FUNC()
    {
        if (CBDTWxApp::GetBDTWxApp() != NULL)
        {
            CBDTWxApp::m_bInit = false;
            wxCriticalSectionLocker lock(gs_wxStartupCS);

            if (gs_wxMainThread)
            {
                // If wx main thread is running, we need to stop it. To accomplish this,
                // send a message telling it to terminate the app.
                wxThreadEvent *event = new wxThreadEvent(wxEVT_THREAD, CBDTWxApp::CMD_TERMINATE);
                wxQueueEvent(wxApp::GetInstance(), event);

                // We must then wait for the thread to actually terminate.
                WaitForSingleObject(gs_wxMainThread, INFINITE);
                CloseHandle(gs_wxMainThread);
                gs_wxMainThread = NULL;
            }
        }

    }

    void BEATS_PERFORMDETECT_RESET_FUNC()
    {
        if (CBDTWxApp::m_bInit && CBDTWxApp::GetBDTWxApp()->IsFrameValid())
        {
            CPerformDetector::GetInstance()->ResetFrameResult();
        }
    }
    float BEATS_PERFORMDETECT_STOP_IMPL_FUNC(int type)
    {
        float result = 0;
        if (CBDTWxApp::m_bInit && CBDTWxApp::GetBDTWxApp()->IsFrameValid() && CBDTWxApp::GetBDTWxApp()->IsEnable(eBDTFT_Performance))
        {
            result = CPerformDetector::GetInstance()->StopDetectNode(type);
        }
        return result;
    }
    void BEATS_PERFORMDETECT_START_IMPL_FUNC(int type, bool /*isStaticRecord*/, size_t id)
    {
        if (CBDTWxApp::m_bInit && CBDTWxApp::GetBDTWxApp()->IsFrameValid() && CBDTWxApp::GetBDTWxApp()->IsEnable(eBDTFT_Performance))
        {
            CPerformDetector::GetInstance()->StartDetectNode(type, id);
        }
    }
    void BEATS_PERFORMDETECT_INIT_FUNC( const wchar_t* typeName[], size_t size )
    {
        if (CBDTWxApp::m_bInit && CBDTWxApp::GetBDTWxApp()->IsFrameValid() && CBDTWxApp::GetBDTWxApp()->IsEnable(eBDTFT_Performance))
        {
            CPerformDetector::GetInstance()->SetTypeName(typeName, size);
        }
    }

    MY_DLL_DECL void BEATS_INFOBOARD_ADDLOG_IMPL_FUNC( const TCHAR* pCatalog, const TCHAR* log, size_t logPos /*= 0*/, size_t fontColor /*= 0*/ )
    {
        if (CBDTWxApp::m_bInit && CBDTWxApp::GetBDTWxApp()->IsFrameValid() && CBDTWxApp::GetBDTWxApp()->IsEnable(eBDTFT_InfoBoard))
        {
            CBDTWxApp::GetBDTWxApp()->AddInfoLog(log, pCatalog, logPos, fontColor);
        }
    }

    MY_DLL_DECL void BEATS_INFOBOARD_UPDATEPROPERTY_UI_IMPL_FUNC( size_t propertyId, const TCHAR* displayName, const TCHAR* pCatalog, void* value, EPropertyType type )
    {
        if (CBDTWxApp::m_bInit && CBDTWxApp::GetBDTWxApp()->IsFrameValid() && CBDTWxApp::GetBDTWxApp()->IsEnable(eBDTFT_InfoBoard))
        {
            CBDTWxApp::GetBDTWxApp()->UpdatePropertyInfo(pCatalog, displayName, propertyId, value, type);
        }
    }

    MY_DLL_DECL void BEATS_INFOBOARD_DELETEPROPERTY_UI_IMPL_FUNC( const TCHAR* displayName, const TCHAR* pCatalog)
    {
        if (CBDTWxApp::m_bInit && CBDTWxApp::GetBDTWxApp()->IsFrameValid() && CBDTWxApp::GetBDTWxApp()->IsEnable(eBDTFT_InfoBoard))
        {
            CBDTWxApp::GetBDTWxApp()->DeleteProperty(pCatalog, displayName);
        }
    }

    MY_DLL_DECL void BEATS_INFOBOARD_REGISTER_PROPERTYCHANGE_HANDLER( TPropertyChangedHandler handlerFunc )
    {
        if (CBDTWxApp::m_bInit && CBDTWxApp::GetBDTWxApp()->IsFrameValid() && CBDTWxApp::GetBDTWxApp()->IsEnable(eBDTFT_InfoBoard))
        {
            CBDTWxApp::GetBDTWxApp()->RegisterPropertyChangeHandler(handlerFunc);
        }
    }

} // extern "C"

