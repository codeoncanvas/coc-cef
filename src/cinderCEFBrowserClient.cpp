#include "include/wrapper/cef_helpers.h"
#include "cinderCEF.h"
#include "cinderCEFBrowserClient.h"
#include "cinder/Log.h"

CinderCEFBrowserClient::CinderCEFBrowserClient(
        CinderCEF *parent, CinderCEFRenderHandler *renderHandler) {
    mParent = parent;
    mHandler = renderHandler;
}

CefRefPtr<CefRenderHandler> CinderCEFBrowserClient::GetRenderHandler() {
    return mHandler;
}

CefRefPtr<CefLoadHandler> CinderCEFBrowserClient::GetLoadHandler() { return this; }

CefRefPtr<CefLifeSpanHandler> CinderCEFBrowserClient::GetLifeSpanHandler() {
    return this;
}

void CinderCEFBrowserClient::OnLoadStart(
        CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) {
    mParent->onLoadStart();
}

void CinderCEFBrowserClient::OnLoadEnd(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame, int httpStatusCode) {
    mParent->onLoadEnd(httpStatusCode);
}

bool CinderCEFBrowserClient::OnProcessMessageReceived(
        CefRefPtr<CefBrowser> browser, CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) {

    // Retrieve the argument list object.
    CefRefPtr<CefListValue> args = message->GetArgumentList();

    // Retrieve the argument values.
    std::string name = message->GetName().ToString();

    //ofLogVerbose() << "Message: " << name << " ; " << args;
    CI_LOG_V("Message: " << name << " ; " << args);

    std::string jsBindPrefix = "js-bind-";
    if (name.compare(0, jsBindPrefix.size(), jsBindPrefix) == 0) {
        mParent->bindCallFromJS(args);
        return true;
    } else if (name == "OnV8ContextCreated") {
        mParent->mV8ContextCreated = true;
    } else if (name == "OnV8ContextReleased") {
        mParent->mV8ContextCreated = false;
    }

    return true;
}

void CinderCEFBrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    // Must be executed on the UI thread.
    CEF_REQUIRE_UI_THREAD();

    if (!mBrowser.get()) {
        // Keep a reference to the main browser.
        mBrowser = browser;
        mBrowserId = browser->GetIdentifier();
    }

    // Keep track of how many browsers currently exist.
    mBrowserCount++;
}

void CinderCEFBrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    // Must be executed on the UI thread.
    CEF_REQUIRE_UI_THREAD();

    if (mBrowserId == browser->GetIdentifier()) {
        // Free the browser pointer so that the browser can be destroyed.
        mBrowser = NULL;
    }

    if (--mBrowserCount == 0) {}
}
