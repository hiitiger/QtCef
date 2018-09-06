#include "stable.h"
#include "../qcefclient/qtcef/qcefwidgetos.h"
#include "../qcefclient/qtcef/qcefclient.h"
#include "demoapi.h"

int main(int argc, char** argv )
{
    QApplication app(argc, argv);

    QCefClient cefClient;
    cefClient.initCefClient();

    DemoApi* api = new DemoApi();
    cefClient.regTopName("qcef");
    cefClient.regApi(api, "qcef", "demoapi");

    QCefOSWidget w;

    w.show();
    w.setUrl("http://127.0.0.1:5000");

    QCefApiAdapter * adapter = new QCefApiAdapter(&w, &w);
    adapter->initApi(api, "qcef", "demoapi");

    app.exec();
    cefClient.shutDown();

    return 0;
}