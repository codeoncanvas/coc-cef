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

#include "cocCefWrapper.h"

namespace coc {


using namespace ci;
using namespace ci::app;
using namespace std;

void CefWrapper::setup( string url, ci::ivec2 size ) {
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
			new RenderHandler{ size.x, size.y } };

	CefWindowInfo windowInfo;

	// in linux set a gtk widget, in windows a hwnd. If not available set
	// nullptr - may cause some render errors, in context-menu and plugins.
	// false means no transparency (site background colour)
	windowInfo.SetAsWindowless(nullptr, false);

//	auto url = std::string{"codeoncanvas.cc"};
	mBrowserClient = new BrowserClient{mRenderHandler.get()};
	mBrowser = CefBrowserHost::CreateBrowserSync(windowInfo, mBrowserClient.get(),
			url, CefBrowserSettings{}, nullptr);

}

void CefWrapper::registerEvents()
{
	getWindow()->getSignalKeyDown().connect( signals::slot( this, &CefWrapper::keyDown) );
	getWindow()->getSignalKeyUp().connect( signals::slot( this, &CefWrapper::keyUp) );
	getWindow()->getSignalMouseDown().connect( signals::slot( this, &CefWrapper::mouseDown) );
	getWindow()->getSignalMouseUp().connect( signals::slot( this, &CefWrapper::mouseUp) );
	getWindow()->getSignalMouseWheel().connect( signals::slot( this, &CefWrapper::mouseWheel) );
	getWindow()->getSignalMouseMove().connect( signals::slot( this, &CefWrapper::mouseMove) );
	getWindow()->getSignalMouseDrag().connect( signals::slot( this, &CefWrapper::mouseDrag) );
}

void CefWrapper::keyDown( KeyEvent event ) {

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

void CefWrapper::keyUp( KeyEvent event ) {

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

void CefWrapper::mouseDown( MouseEvent event ) {

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

void CefWrapper::mouseUp( MouseEvent event ) {

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

void CefWrapper::mouseWheel( MouseEvent event ) {

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

void CefWrapper::mouseMove( MouseEvent event ) {

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

void CefWrapper::mouseDrag( MouseEvent event ) {
	// not yet implemented, see ClientHandler::StartDragging if required
}

void CefWrapper::update() {
	// Single iteration of message loop, does not block
	CefDoMessageLoopWork();
}

void CefWrapper::draw( ci::vec2  pos ) {
	gl::draw(getTexture(), pos);
	// TODO implement cursor changes, see CefRenderHandler::OnCursorChange
}

ci::gl::TextureRef CefWrapper::getTexture()
{
	return mRenderHandler->getTexture();
}

void CefWrapper::resize( ci::ivec2 size ) {
	//TODO this doesn't work fully

	mRenderHandler->resize(size.x,size.y);

	// Check host is available
	if (mBrowser == nullptr) { return; }
	const auto browserHost = mBrowser->GetHost();
	if (browserHost == nullptr) { return; }

	browserHost->WasResized();
}

void CefWrapper::cleanup() {

	// Shut down CEF.
	mBrowser->GetHost()->CloseBrowser(false);
	CefShutdown();

	//TODO fix crash
	// http://www.magpcss.org/ceforum/viewtopic.php?f=6&t=11441&p=24037&hilit=AutoreleasePoolPage#p24037
}

}