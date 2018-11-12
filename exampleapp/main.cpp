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

    QCefOSWidget* w = new QCefOSWidget();

    w->show();
    w->setUrl("https://baidu.com");

    QCefApiAdapter * adapter = new QCefApiAdapter(w, w);
    adapter->initApi(api, "qcef", "demoapi");

    app.setQuitOnLastWindowClosed(false);

    w->setAttribute(Qt::WA_DeleteOnClose, true);

    QObject::connect(w, &QObject::destroyed, [&]() {
        cefClient.shutDown();

        app.quit();
    });

    app.exec();

    return 0;
}