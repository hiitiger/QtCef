# QtCef
Integrate CEF and Qt,  inject C++ API to CEF web page by utilizing reflection system based on Qt's MOC.

## Usage
This project is built for Qt4, with very little change it can be use with Qt5, but you probably should use QWebEngine in Qt5.

With Qt's MOC based reflection, we can utilize it to inject C++ api into Cef web page without manually add every signle method and event for every single api.

```c++
    //main process
    MyAppApi* api = new MyAppApi(parent);
    QCefApiAdapter* adapter = new QCefApiAdapter(webWidget, parent);
    adpater->initApi(api, "myapp", "app");

    MyToolApi* toolApi = new MyToolApi(parent);
    QCefApiAdapter* toolApiAdapter = new QCefApiAdapter(webWidget, parent);
    toolApiAdapter->initApi(toolApi, adapter->apiPath, "tool");

    //brower process
     MyAppApi api;
     injectApi(context, "myapp.app", &api);

     MyToolApi toolApi;
     injectApi(context, "myapp.app.tool", &toolApi);

     //js
     myapp.app.tool.func(data => console.log(data));
     myapp.app.someEvent.addListener( data => console.log(data));
```

## Build
1. Download libcef and build
2. Build qjson-backport
3. qmake -tp vc -r
