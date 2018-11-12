// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "stable.h"
#include <string>
#include "qceffunctionhandler.h"
#include "client_app.h"

QClientApp::QClientApp()
{
    renderProcessHandler_ = new QCefRenderProcessHandler();
}

void QClientApp::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
{
    registrar->AddCustomScheme("qcef-file", true, false, false, false, true, false);
}
