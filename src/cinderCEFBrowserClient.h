#ifndef BROWSER_CLIENT_H
#define BROWSER_CLIENT_H

#include "cef_client.h"

#include "cinderCEFRenderHandler.h"

class CinderCEF;

class CinderCEFBrowserClient : public CefClient,
                               public CefLoadHandler,
                               public CefLifeSpanHandler {
public:
    CinderCEFBrowserClient(
            CinderCEF *mParent, CinderCEFRenderHandler *renderHandler);

    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE;

    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE;

    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE;

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
            CefProcessId source_process,
            CefRefPtr<CefProcessMessage> message) OVERRIDE;

    virtual void OnLoadStart(
            CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame);

    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
            CefRefPtr<CefFrame> frame, int httpStatusCode) OVERRIDE;

    // CefLifeSpanHandler methods.
    void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
    void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

    CefRefPtr<CefBrowser> GetBrowser() const { return mBrowser; }

private:
    CefRefPtr<CefRenderHandler> mHandler;
    CinderCEF *mParent;

    CefRefPtr<CefBrowser> mBrowser;
    int mBrowserId;
    int mBrowserCount;

    IMPLEMENT_REFCOUNTING(CinderCEFBrowserClient);
};

#endif
