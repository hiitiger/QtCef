#include "stable.h"
#include "client_app.h"

int main(int, const char**)
{
    setupTypeNames();
    CefMainArgs main_args(GetModuleHandle(NULL));
    CefRefPtr<QClientApp> cefapp(new QClientApp);
    return CefExecuteProcess(main_args, cefapp.get(), NULL);
}
