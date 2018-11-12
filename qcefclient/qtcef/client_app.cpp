#include "stable.h"
#include <string>
#include "client_app.h"
#include "client_handler.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"
#include "extensionhandler.h"

QCefClientApp::QCefClientApp()
{
}


void QCefClientApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line)
{
    if (process_type.empty())
    {
        //register plugins ; change command line
    }
}

void QCefClientApp::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
{
    registrar->AddCustomScheme("qcef-file", true, false, false, false, true, false);
}

void QCefClientApp::OnContextInitialized() 
{
    CEF_REQUIRE_UI_THREAD();
    CefRegisterSchemeHandlerFactory("qcef-file", "", new QClientFileHandlerFactory());
    emit contextInitlialized();
}

