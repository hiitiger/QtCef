# QtCef

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/82b587014d2548db9724bb6db8fd5d43)](https://app.codacy.com/app/hiitiger/QtCef?utm_source=github.com&utm_medium=referral&utm_content=hiitiger/QtCef&utm_campaign=Badge_Grade_Dashboard)

Integrate CEF and Qt,  inject C++ API to CEF web page by utilizing reflection system based on Qt's MOC.


## Note
1. this is for Qt5, checkout qt4archive branch for Qt4
2. need VS2015 or later

 ## Build
1. download CEF 3.3538 to deps direcotry and unzip to dir cef3538
2. build cef libcef_dll_wrapper with VS2015
3. copy cef.lib and libcef_dll_wrapper.lib to common/lib/debug or common/lib/release, copy cef release dlls and resources to bin/debug or bin/release
4. generator VS2015 solution with qmake -tp vc -r
5. build all 

## Usage

With Qt's MOC based reflection, we can utilize it to inject C++ api into Cef web page without manually add every signle method and event for every single api.

```c++
    //main process
    QCefOSWidget* webWidget = new QCefOSWidget();
    MyAppApi* api = new MyAppApi(parent);
    QCefApiAdapter* adapter = new QCefApiAdapter(webWidget, parent);
    adpater->initApi(api, "myapp", "app");

    MyToolApi* toolApi = new MyToolApi(parent);
    QCefApiAdapter* toolApiAdapter = new QCefApiAdapter(webWidget, parent);
    toolApiAdapter->initApi(toolApi, adapter->apiPath(), "tool");

    //renderer process
     MyAppApi api;
     injectApi(context, "myapp.app", &api);

     MyToolApi toolApi;
     injectApi(context, "myapp.app.tool", &toolApi);

     //webpage
     myapp.app.tool.func(data => console.log(data));
     myapp.app.someEvent.addListener( data => console.log(data));
```

