#pragma once

const CefString kPromiseCreatorFunction = "qtcef_CreatePromise";
const CefString kPromiseCreatorScript = ""
"function qtcef_CreatePromise() {"
"   var result = {};"
"   var promise = new Promise(function(resolve, reject) {"
"       result.resolve = resolve; result.reject = reject;"
"   });"
"   result.p = promise;"
"   return result;"
"}";