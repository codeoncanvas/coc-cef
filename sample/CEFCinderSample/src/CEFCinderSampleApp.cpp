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

    coc::CinderCEF cefView;
};

void CEFCinderSampleApp::setup() {
	coc::initCinderCEF();
    cefView.setup("http://duckduckgo.com", getWindowSize());
    cefView.registerEvents();
}

void CEFCinderSampleApp::mouseDown(MouseEvent event) {}

void CEFCinderSampleApp::update() {
    coc::updateCEF();
}

void CEFCinderSampleApp::draw() {
    gl::clear(Color{1, 0, 0});

    cefView.draw();
}


CINDER_APP(CEFCinderSampleApp, RendererGl )
