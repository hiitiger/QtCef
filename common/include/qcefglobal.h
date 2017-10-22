#pragma once

#ifdef Q_DLL_QCEFCLIENT
#define Q_DLL_QCEFCLIENT_API __declspec( dllexport )
#else
#define  Q_DLL_QCEFCLIENT_API __declspec( dllimport )
#endif
