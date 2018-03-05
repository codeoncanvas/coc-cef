#pragma once

#include <vector>
#include <memory>       // unique_ptr
#include <algorithm>    // if_any
#include <iso646.h>     // alternative logical operator tokens

#include "include/cef_application_mac.h"
#include "include/wrapper/cef_helpers.h"

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cef_helpers.h"
#include "cinderCEFBrowserClient.h"
#include "cinderCEFRenderHandler.h"


namespace coc {

void initCinderCEF(int argc, char **argv);
void initCinderCEF();
void updateCEF();

class CinderCEF {

public:
    ~CinderCEF() { cleanup(); }
    void setup(std::string url, ci::ivec2 size);
    void draw(ci::vec2 pos = ci::vec2(0));
    void reshape( ci::ivec2 size );
    void cleanup();

    void executeJS(const std::string& command);
    void notificationHandler();

    ci::gl::TextureRef getTexture();
    void registerEvents();
    void unregisterEvents();
    void enableResize();
    void windowResized();
    void onLoadStart();
    void onLoadEnd(int httpStatusCode);

    template <typename ArgumentsType, class ListenerClass>
    //void bind(const std::string& functionName, ListenerClass * listener, void (ListenerClass::*listenerMethod)(ArgumentsType&), int prio = OF_EVENT_ORDER_AFTER_APP);

    // Don't call this
    void bindCallFromJS(CefRefPtr<CefListValue> args);

    //ofEvent<ofxCEFEventArgs> eventFromCEF;

    bool V8ContextCreated = false; // Don't set this
    bool isReady() const { return V8ContextCreated; } //&& mRenderHandler->initialized && browser(); }

    void keyDown( ci::app::KeyEvent event );
    void keyUp( ci::app::KeyEvent event );
    void mouseDown( ci::app::MouseEvent event );
    void mouseUp( ci::app::MouseEvent event );
    void mouseWheel( ci::app::MouseEvent event );
    void mouseMove( ci::app::MouseEvent event );
    void mouseDrag( ci::app::MouseEvent event );

private:

    bool fixedSize;
    float width_, height_;

    CefRefPtr<CefBrowser> browser() const { return client->GetBrowser(); }

    CefRefPtr<CinderCEFBrowserClient> client;
    CinderCEFRenderHandler* renderHandler;

    CefRefPtr<CefListValue> messageFromJS;

    static constexpr auto scrollSensitivity = 50;  // set arbitrarily
    const std::vector<int> nonCharKeys = std::vector<int>{
            ci::app::KeyEvent::KEY_UP, ci::app::KeyEvent::KEY_DOWN,
            ci::app::KeyEvent::KEY_LEFT, ci::app::KeyEvent::KEY_RIGHT,
            ci::app::KeyEvent::KEY_HOME, ci::app::KeyEvent::KEY_END,
            ci::app::KeyEvent::KEY_PAGEUP, ci::app::KeyEvent::KEY_PAGEDOWN,
            ci::app::KeyEvent::KEY_DELETE, ci::app::KeyEvent::KEY_BACKSPACE};


}; // class cinderCef

} // namespace coc

