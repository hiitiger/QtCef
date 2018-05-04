#include "stable.h"
#include "../qcefclient/qtcef/qcefwidgetos.h"
#include "../qcefclient/qtcef/qcefclient.h"
#include "demoapi.h"

int main(int argc, char** argv )
{
    QApplication app(argc, argv);

    QCefClient cefClient;
    cefClient.initCefClient();

    QCefOSWidget w;

    w.show();
    w.setUrl("https://www.duckduckgo.com");

	DemoApi* api = new DemoApi(&w);
	QCefApiAdapter * adapter = new QCefApiAdapter(&w, &w);
	adapter->initApi(api, "qcef", "demoapi");

    app.exec();
    cefClient.shutDown();

    return 0;
}