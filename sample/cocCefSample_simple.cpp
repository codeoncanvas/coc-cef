#pragma once

#include "cocCefWrapper.h"

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
    
    coc::CefWrapper wrapper;
};

void cocCefSample::setup() {
    wrapper.setup("codeoncanvas.cc", getWindowSize() );
    wrapper.registerEvents();
}

void cocCefSample::keyDown( KeyEvent event ) {
//
}

void cocCefSample::keyUp( KeyEvent event ) {
//
}

void cocCefSample::mouseDown( MouseEvent event ) {
//
}

void cocCefSample::mouseUp( MouseEvent event ) {
//
}

void cocCefSample::mouseWheel( MouseEvent event ) {
//
}

void cocCefSample::mouseMove( MouseEvent event ) {
//
}

void cocCefSample::mouseDrag( MouseEvent event ) {
//
}

void cocCefSample::update() {
    wrapper.update();
}

void cocCefSample::draw() {
    wrapper.draw();
}

void cocCefSample::resize() {
    //wrapper.resize( getWindowSize() );
}

void cocCefSample::cleanup() {
    //wrapper.cleanup();
}

CINDER_APP( cocCefSample, RendererGl )
