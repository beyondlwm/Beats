#include "stdafx.h"
#include "mainapp.h"
#include "codereviewer.h"

IMPLEMENT_APP(MainApp)

bool MainApp::OnInit()
{
    MainFrame *p = new MainFrame(NULL);
    p->Show();
    return true;
}