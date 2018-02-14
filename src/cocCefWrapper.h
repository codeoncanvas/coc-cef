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

#include "browser_client.hpp"
#include "render_handler.hpp"


namespace coc {

class CefWrapper {

public:
	~CefWrapper(){ cleanup(); }
	void setup(std::string url, ci::ivec2 size);
	void update();
	void draw(  ci::vec2 pos = ci::vec2(0)  );
	void resize( ci::ivec2 size );
	void cleanup();

	ci::gl::TextureRef getTexture();
	void registerEvents();

	void keyDown( ci::app::KeyEvent event );
	void keyUp( ci::app::KeyEvent event );
	void mouseDown( ci::app::MouseEvent event );
	void mouseUp( ci::app::MouseEvent event );
	void mouseWheel( ci::app::MouseEvent event );
	void mouseMove( ci::app::MouseEvent event );
	void mouseDrag( ci::app::MouseEvent event );

private:

	CefRefPtr<CefBrowser> mBrowser;
	CefRefPtr<BrowserClient> mBrowserClient;
	std::unique_ptr<RenderHandler> mRenderHandler;

	static constexpr auto mScrollSensitivity = 50;  // set arbitrarily
	const std::vector<int> mNonCharKeys = std::vector<int>{
			ci::app::KeyEvent::KEY_UP, ci::app::KeyEvent::KEY_DOWN,
			ci::app::KeyEvent::KEY_LEFT, ci::app::KeyEvent::KEY_RIGHT,
			ci::app::KeyEvent::KEY_HOME, ci::app::KeyEvent::KEY_END,
			ci::app::KeyEvent::KEY_PAGEUP, ci::app::KeyEvent::KEY_PAGEDOWN,
			ci::app::KeyEvent::KEY_DELETE, ci::app::KeyEvent::KEY_BACKSPACE};


};//class CocBrowser

}




