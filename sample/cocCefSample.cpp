#include <vector>
#include <memory>       // unique_ptr
#include <algorithm>    // if_any
#include <iso646.h>     // alternative logical operator tokens

#include "include/cef_application_mac.h"
#include "include/wrapper/cef_helpers.h"

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "browser_client.hpp"
#include "render_handler.hpp"

using namespace ci;
using namespace ci::app;

class cocCefSample : public App {
public:
    void setup() override;
    void keyDown( KeyEvent event ) override;
    void keyUp( KeyEvent event ) override;
    void mouseDown( MouseEvent event ) override;
    void mouseUp( MouseEvent event ) override;
    void mouseWheel( MouseEvent event ) override;
    void mouseMove( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
    void update() override;
    void draw() override;
    void resize() override;
    void cleanup() override;

    CefRefPtr<CefBrowser> mBrowser;
    CefRefPtr<BrowserClient> mBrowserClient;
    std::unique_ptr<RenderHandler> mRenderHandler;

private:
    static constexpr auto mScrollSensitivity = 50;  // set arbitrarily
    const std::vector<int> mNonCharKeys = std::vector<int>{
            KeyEvent::KEY_UP, KeyEvent::KEY_DOWN,
            KeyEvent::KEY_LEFT, KeyEvent::KEY_RIGHT,
            KeyEvent::KEY_HOME, KeyEvent::KEY_END,
            KeyEvent::KEY_PAGEUP, KeyEvent::KEY_PAGEDOWN,
            KeyEvent::KEY_DELETE, KeyEvent::KEY_BACKSPACE};
};

void cocCefSample::setup() {
    // Spoof CEF command-line arguments
    int argc = 0;
    char const* argv[] = {  // not sure these are working
        "off-screen-rendering-enabled", "disable-gpu", "disable-gpu-compositing",
        "enable-begin-frame-scheduling", "enable-media-stream"};
    CefMainArgs mainArgs{argc, const_cast<char **>(argv)};

    const auto didExecute = CefExecuteProcess(mainArgs, nullptr, nullptr);
    if (not didExecute) {}  // handle error here

    // Specify CEF global settings here.
    // checkout detailed settings options:
    // http://magpcss.org/ceforum/apidocs/projects/%28default%29/_cef_settings_t.html
    CefSettings settings;
    settings.windowless_rendering_enabled = true;
    const auto didInitialize = CefInitialize(mainArgs, settings, nullptr, nullptr);
    if (not didInitialize) {}  // handle error here

    mRenderHandler = std::unique_ptr<RenderHandler>{
        new RenderHandler{getWindowWidth(), getWindowHeight()}};

    CefWindowInfo windowInfo;

    // in linux set a gtk widget, in windows a hwnd. If not available set
    // nullptr - may cause some render errors, in context-menu and plugins.
    // false means no transparency (site background colour)
    windowInfo.SetAsWindowless(nullptr, false);

    auto url = std::string{"codeoncanvas.cc"};
    mBrowserClient = new BrowserClient{mRenderHandler.get()};
    mBrowser = CefBrowserHost::CreateBrowserSync(windowInfo, mBrowserClient.get(),
            url, CefBrowserSettings{}, nullptr);
}

void cocCefSample::keyDown( KeyEvent event ) {

    // Check host is available
    if (mBrowser == nullptr) { return; }
    const auto browserHost = mBrowser->GetHost();
    if (browserHost == nullptr) { return; }

    CefKeyEvent cefKeyEvent;

    // hack inherited from ofxCEF
    // https://github.com/ofZach/ofxCef/blame/master/src/ofxCEF.cpp#L425
    if (std::any_of(mNonCharKeys.cbegin(), mNonCharKeys.cend(),
            [&event](int k){ return k == event.getCode(); })) {
        cefKeyEvent.type = KEYEVENT_KEYDOWN;

    } else {
        cefKeyEvent.type = KEYEVENT_CHAR;
        cefKeyEvent.character = event.getChar();
    }

    cefKeyEvent.native_key_code = event.getNativeKeyCode();
    browserHost->SendKeyEvent(cefKeyEvent);
}

void cocCefSample::keyUp( KeyEvent event ) {

    // Check host is available
    if (mBrowser == nullptr) { return; }
    const auto browserHost = mBrowser->GetHost();
    if (browserHost == nullptr) { return; }

    CefKeyEvent cefKeyEvent;

    if (std::any_of(mNonCharKeys.cbegin(), mNonCharKeys.cend(),
            [&event](int key){ return key == event.getCode(); })) {
        cefKeyEvent.type = KEYEVENT_CHAR;
        cefKeyEvent.character = event.getChar();

    } else {
        cefKeyEvent.type = KEYEVENT_KEYUP;
    }

    cefKeyEvent.native_key_code = event.getNativeKeyCode();
    browserHost->SendKeyEvent(cefKeyEvent);
}

void cocCefSample::mouseDown( MouseEvent event ) {

    const auto browserHost = mBrowser->GetHost();
    if (browserHost == nullptr) { return; }

    const auto mouseButtonType =
            event.isLeft() ? MBT_LEFT :
            //event.isRight() ? MBT_RIGHT :  // causes crash, so treat as middle
            MBT_MIDDLE;

    CefMouseEvent cefMouseEvent;
    cefMouseEvent.x = event.getX();
    cefMouseEvent.y = event.getY();
    cefMouseEvent.modifiers = event.getNativeModifiers();

    mBrowser->GetHost()->SendMouseClickEvent(cefMouseEvent, mouseButtonType, false, 1);
}

void cocCefSample::mouseUp( MouseEvent event ) {

    // Check host is available
    if (mBrowser == nullptr) { return; }
    const auto browserHost = mBrowser->GetHost();
    if (browserHost == nullptr) { return; }

    const auto mouseButtonType =
            event.isLeft() ? MBT_LEFT :
            //event.isRight() ? MBT_RIGHT :  // causes crash, so treat as middle
            MBT_MIDDLE;

    CefMouseEvent cefMouseEvent;
    cefMouseEvent.x = event.getX();
    cefMouseEvent.y = event.getY();
    cefMouseEvent.modifiers = event.getNativeModifiers();

    mBrowser->GetHost()->SendMouseClickEvent(cefMouseEvent, mouseButtonType, true, 1);
}

void cocCefSample::mouseWheel( MouseEvent event ) {

    // Check host is available
    if (mBrowser == nullptr) { return; }
    const auto browserHost = mBrowser->GetHost();
    if (browserHost == nullptr) { return; }

    const int deltaY = std::round(event.getWheelIncrement() * mScrollSensitivity);

    CefMouseEvent cefMouseEvent;
    cefMouseEvent.x = event.getX();
    cefMouseEvent.y = event.getY();
    cefMouseEvent.modifiers = event.getNativeModifiers();

    mBrowser->GetHost()->SendMouseWheelEvent(cefMouseEvent, 0, deltaY);
}

void cocCefSample::mouseMove( MouseEvent event ) {

    // Check host is available
    if (mBrowser == nullptr) { return; }
    const auto browserHost = mBrowser->GetHost();
    if (browserHost == nullptr) { return; }

    CefMouseEvent cefMouseEvent;
    cefMouseEvent.x = event.getX();
    cefMouseEvent.y = event.getY();
    cefMouseEvent.modifiers = event.getNativeModifiers();

    browserHost->SendMouseMoveEvent(cefMouseEvent, false);
}

void cocCefSample::mouseDrag( MouseEvent event ) {
    // not yet implemented, see ClientHandler::StartDragging if required
}

void cocCefSample::update() {
    // Single iteration of message loop, does not block
    CefDoMessageLoopWork();
}

void cocCefSample::draw() {
    gl::draw(mRenderHandler->getTexture());
    // TODO implement cursor changes, see CefRenderHandler::OnCursorChange
}

void cocCefSample::resize() {
    //TODO this doesn't work fully

    mRenderHandler->resize(getWindowWidth(), getWindowHeight());

    // Check host is available
    if (mBrowser == nullptr) { return; }
    const auto browserHost = mBrowser->GetHost();
    if (browserHost == nullptr) { return; }

    browserHost->WasResized();
}

void cocCefSample::cleanup() {

    // Shut down CEF.
    mBrowser->GetHost()->CloseBrowser(false);
    CefShutdown();

    //TODO fix crash
    // http://www.magpcss.org/ceforum/viewtopic.php?f=6&t=11441&p=24037&hilit=AutoreleasePoolPage#p24037
}

CINDER_APP( cocCefSample, RendererGl )
