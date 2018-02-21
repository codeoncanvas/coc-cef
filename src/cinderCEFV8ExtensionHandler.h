#ifndef CINDERCEFV8EXTENSIONHANDLER_H
#define CINDERCEFV8EXTENSIONHANDLER_H

#include "include/cef_app.h"
#include <set>
#include <string>

struct ofxCEFV8ExtensionHandler : public CefV8Handler {
    ofxCEFV8ExtensionHandler(CefRefPtr<CefApp> app);

    bool Execute(const CefString &name, CefRefPtr<CefV8Value> object,
            const CefV8ValueList &arguments, CefRefPtr<CefV8Value> &retval,
            CefString &exception) OVERRIDE;

    std::set<std::string> bindJSFunctionNames;

private:
    CefRefPtr<CefApp> app;

    IMPLEMENT_REFCOUNTING(ofxCEFV8ExtensionHandler);
};

#endif
