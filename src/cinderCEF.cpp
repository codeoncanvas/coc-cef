/**
 *
 *      ┌─┐╔═╗┌┬┐┌─┐
 *      │  ║ ║ ││├┤
 *      └─┘╚═╝─┴┘└─┘
 *   ┌─┐┌─┐╔╗╔┬  ┬┌─┐┌─┐
 *   │  ├─┤║║║└┐┌┘├─┤└─┐
 *   └─┘┴ ┴╝╚╝ └┘ ┴ ┴└─┘
 *
 * Copyright (c) 2018 Code on Canvas Pty Ltd, http://CodeOnCanvas.cc
 *
 * This software is distributed under the MIT license
 * https://tldrlegal.com/license/mit-license
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code
 *
 **/

#include "cinder/Log.h"
#include "cinder/Exception.h"
#include "cinderCEF.h"

namespace coc {


using namespace ci;
using namespace ci::app;
using namespace std;

void CinderCEF::setup( string url, ci::ivec2 size ) {
    // Spoof CEF command-line arguments
    //int argc = 0;
    //char const* argv[] = {  // not sure these are working
    //        "off-screen-rendering-enabled", "disable-gpu", "disable-gpu-compositing",
    //        "enable-begin-frame-scheduling", "enable-media-stream"};

#if defined(TARGET_OSX)
    CefMainArgs mainArgs{argc, const_cast<char **>(argv)};

#elif defined(TARGET_WIN32)
    CefMainArgs main_args(::GetModuleHandle(NULL));

    // These flags must match the Chromium values.
    const char kProcessType[] = "type";
    const char kRendererProcess[] = "renderer";

//#if defined(OS_LINUX)
//    const char kZygoteProcess[] = "zygote";
//
//#endif

    // Parse command-line arguments.
    CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
    command_line->InitFromString(::GetCommandLineW());

    CI_LOG_I("Args: " << command_line->GetCommandLineString().ToString());

    // Create a ClientApp of the correct type.
    CefRefPtr<CefApp> app;

    // The command-line flag won't be specified for the browser process.
    if (command_line->HasSwitch(kProcessType)) {
        const std::string& process_type = command_line->GetSwitchValue(kProcessType);
        ofLogNotice() << "Process type: " << process_type;

        if (process_type == kRendererProcess) {
            app = new ofxCEFClientApp();

//#if defined(OS_LINUX)
//        } else if (process_type == kZygoteProcess) {
//            return ZygoteProcess;
//
//#endif // defined(OS_LINUX)
        } else {
            //app = new ClientAppOther();
        }

    } else {
        //app = new ClientAppBrowser();
    }

    const auto didExecute = CefExecuteProcess(mainArgs, nullptr, nullptr);
    if (not didExecute) { throw ci::Exception{"CEF process execution failed"}; }

#endif // defined(TARGET_WIN32)

    // Specify CEF global cefSettings here.
    // checkout detailed cefSettings options:
    // http://magpcss.org/ceforum/apidocs/projects/%28default%29/_cef_settings_t.html
    CefSettings cefSettings;

    cefSettings.background_color = 0xFFFF00FF;
    cefSettings.single_process = false;
    cefSettings.windowless_rendering_enabled = true;
    cefSettings.command_line_args_disabled = true;

#if defined(TARGET_OSX)
    cefSettings.remote_debugging_port = 8088;
    // On Windows this leads to:
    // tcp_socket_win.cc bind() retunred an error: an attempt was made to access a socket in a way forbidden by its access permissions

#else
    CefMainArgs mainArgs{};

#endif

    // This could be used on windows, could improve performance
    // If you enable this, 'CefDoMessageLoopWork()' should not be called
    //cefSettings.multi_threaded_message_loop = true;

    // Implement external message pump?! see 'main_message_loop_external_pump' in 'ceftest/shared/browser'
    //cefSettings.external_message_pump = true;

    // Default is LOGSEVERITY_INFO
    //cefSettings.log_severity = LOGSEVERITY_VERBOSE;

    const auto didInitialize = CefInitialize(mainArgs, cefSettings, nullptr, nullptr);
    if (not didInitialize) { throw ci::Exception{"CEF process execution failed"}; }

    mRenderHandler = std::unique_ptr<CinderCEFRenderHandler>{
        new CinderCEFRenderHandler{ size.x, size.y } };

    CefWindowInfo windowInfo;

#if defined(TARGET_OSX)
    // in linux set a gtk widget, in windows a hwnd. If not available set
    // nullptr - may cause some render errors, in context-menu and plugins.
    // false means no transparency (site background colour)
    windowInfo.SetAsWindowless(nullptr, false);

#elif defined(TARGET_WIN32)
    HWND hWnd = ofGetWin32Window();
    windowInfo.SetAsWindowless(hWnd);

#endif // defined(TARGET_WIN32)

    CefBrowserSettings browserSettings;
    browserSettings.webgl = STATE_ENABLED;
    browserSettings.windowless_frame_rate = 60;
    browserSettings.background_color = 0x00FFFFFF;
    browserSettings.web_security = STATE_DISABLED;

    //TODO reconcile with ofxCEF
    mBrowserClient = new CinderCEFBrowserClient{this, mRenderHandler.get()};
    mBrowser = CefBrowserHost::CreateBrowserSync(windowInfo, mBrowserClient.get(),
            url, CefBrowserSettings{}, nullptr);
}

void CinderCEF::registerEvents() {
    getWindow()->getSignalKeyDown().connect( signals::slot( this, &CinderCEF::keyDown) );
    getWindow()->getSignalKeyUp().connect( signals::slot( this, &CinderCEF::keyUp) );
    getWindow()->getSignalMouseDown().connect( signals::slot( this, &CinderCEF::mouseDown) );
    getWindow()->getSignalMouseUp().connect( signals::slot( this, &CinderCEF::mouseUp) );
    getWindow()->getSignalMouseWheel().connect( signals::slot( this, &CinderCEF::mouseWheel) );
    getWindow()->getSignalMouseMove().connect( signals::slot( this, &CinderCEF::mouseMove) );
    getWindow()->getSignalMouseDrag().connect( signals::slot( this, &CinderCEF::mouseDrag) );
}

void CinderCEF::unregisterEvents() {
    //TODO disconnect signals
    //getWindow()->getSignalKeyDown().disconnect( signals::slot( this, &CinderCEF::keyDown) );
    //getWindow()->getSignalKeyUp().disconnect( signals::slot( this, &CinderCEF::keyUp) );
    //getWindow()->getSignalMouseDown().disconnect( signals::slot( this, &CinderCEF::mouseDown) );
    //getWindow()->getSignalMouseUp().disconnect( signals::slot( this, &CinderCEF::mouseUp) );
    //getWindow()->getSignalMouseWheel().disconnect( signals::slot( this, &CinderCEF::mouseWheel) );
    //getWindow()->getSignalMouseMove().disconnect( signals::slot( this, &CinderCEF::mouseMove) );
    //getWindow()->getSignalMouseDrag().disconnect( signals::slot( this, &CinderCEF::mouseDrag) );
}

void CinderCEF::keyDown( KeyEvent event ) {

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

void CinderCEF::keyUp( KeyEvent event ) {

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

void CinderCEF::mouseDown( MouseEvent event ) {

    const auto browserHost = mBrowser->GetHost();
    if (browserHost == nullptr) { return; }

    const auto mouseButtonType =
            event.isLeft() ? MBT_LEFT :
                    //event.isRight() ? MBT_RIGHT :  // causes crash, so treat as middle
                    //TODO can use EVENTFLAG_LEFT_MOUSE_BUTTON
                    MBT_MIDDLE;

    CefMouseEvent cefMouseEvent;
    cefMouseEvent.x = event.getX();
    cefMouseEvent.y = event.getY();
    cefMouseEvent.modifiers = event.getNativeModifiers();

    mBrowser->GetHost()->SendMouseClickEvent(cefMouseEvent, mouseButtonType, false, 1);
}

void CinderCEF::mouseUp( MouseEvent event ) {

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

void CinderCEF::mouseWheel( MouseEvent event ) {

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

void CinderCEF::mouseMove( MouseEvent event ) {

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

void CinderCEF::mouseDrag( MouseEvent event ) {
    // not yet implemented, see ClientHandler::StartDragging if required
}

void CinderCEF::update() {
    // Single iteration of message loop, does not block
    CefDoMessageLoopWork();
}

void CinderCEF::draw( ci::vec2  pos ) {
    if (!isReady()) { return; }

    gl::draw(getTexture(), pos);

    // TODO implement cursor changes, see CefRenderHandler::OnCursorChange
}

ci::gl::TextureRef CinderCEF::getTexture()
{
    return mRenderHandler->getTexture();
}

void CinderCEF::resize( ci::ivec2 size ) {
    //TODO this doesn't work fully

    mRenderHandler->resize(size.x,size.y);

    // Check host is available
    if (mBrowser == nullptr) { return; }
    const auto browserHost = mBrowser->GetHost();
    if (browserHost == nullptr) { return; }

    browserHost->WasResized();
}

void CinderCEF::onLoadStart() {

}

void CinderCEF::onLoadEnd(int httpStatusCode) {

}

//void CinderCEF::bindCallFromJS(CefRefPtr<CefListValue> args);
//void CinderCEF::bindCallFromJS(CefRefPtr<CefListValue> args) {
//    mMessageFromJS = args;
//
//    //TODO ofNotifyEvent(messageFromJS, msg, this);
//}

void CinderCEF::executeJS(const string& command){
    if (mBrowser == nullptr) { return; }

    CefRefPtr<CefFrame> frame = browser()->GetMainFrame();
    frame->ExecuteJavaScript(command, frame->GetURL(), 0);
}

void CinderCEF::cleanup() {
    unregisterEvents();

    // Shut down CEF.
    if (browser()) {
        mBrowser->GetHost()->CloseBrowser(true);
    }

    // The following call to CefShutdown make the app crash on OS X. Still not working on Windows neither.
    // http://www.magpcss.org/ceforum/viewtopic.php?f=6&t=11441&p=24037&hilit=AutoreleasePoolPage#p24037
    //CefShutdown();
}

} // namespace coc
