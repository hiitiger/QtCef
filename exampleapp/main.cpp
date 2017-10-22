#include "stable.h"
#include "../qcefclient/qtcef/qcefwidgetos.h"
#include "../qcefclient/qtcef/qcefclient.h"

int main(int argc, char** argv )
{
    QApplication app(argc, argv);

    QCefClient cefClient;
    cefClient.initCefClient();

    QCefOSWidget w;

    w.show();
    w.setUrl("https://www.duckduckgo.com");

    app.exec();
    cefClient.shutDown();

    return 0;
}