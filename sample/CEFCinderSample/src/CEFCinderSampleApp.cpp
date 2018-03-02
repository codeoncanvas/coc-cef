#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinderCEF.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CEFCinderSampleApp : public App {
public:
    void setup() override;
    void mouseDown(MouseEvent event) override;
    void update() override;
    void draw() override;

    coc::CinderCEF mWebViewWrapper;
};

void CEFCinderSampleApp::setup() {
    mWebViewWrapper.setup("codeoncanvas.cc", getWindowSize());
    mWebViewWrapper.registerEvents();
}

void CEFCinderSampleApp::mouseDown(MouseEvent event) {}

void CEFCinderSampleApp::update() {
    mWebViewWrapper.update();
}

void CEFCinderSampleApp::draw() {
    gl::clear(Color{1, 0, 0});

    static float count = 0;
    gl::drawSolidCircle(vec2{250, 250}, sin(count++ / 180));

    mWebViewWrapper.draw();
}

void prepareSettings(App::Settings *settings) {
    char *argv[] = {};
    coc::initCinderCEF(0, argv);
}

CINDER_APP(CEFCinderSampleApp, RendererGl, &prepareSettings)
