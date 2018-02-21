#include <string>

#include "cinder/Log.h"
#include "ofxCEFClientApp.h"

void cinderCEFClientApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE {
    v8context = context;
    // Retrieve the context's window object.
    // CefRefPtr<CefV8Value> globalObject = context->GetGlobal();

    // printf("context pointer: '%n' \n", context.get());

    // Create an instance of my CefV8Handler object.
    // CefRefPtr<CefV8Handler> v8handler = new MyV8Handler();

    // Create the "myfunc" function.
    // CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("myfunc",
    // v8handler);

    // Add the "myfunc" function to the "window" object.
    // globalObject->SetValue("myfunc", func, V8_PROPERTY_ATTRIBUTE_NONE);

    CI_LOG_I("OnContextCreated\n");

    // Tell browser that V8 Context was created
    CefRefPtr<CefProcessMessage> message =
            CefProcessMessage::Create(CefString("OnV8ContextCreated"));
    // Send message
    browser->SendProcessMessage(PID_BROWSER, message);
}

void cinderCEFClientApp::OnContextReleased(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE {

    v8context = NULL;
    // Tell browser that V8 Context was released
    CefRefPtr<CefProcessMessage> message =
            CefProcessMessage::Create(CefString("OnV8ContextReleased"));
    // Send message
    browser->SendProcessMessage(PID_BROWSER, message);
}

bool cinderCEFClientApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) OVERRIDE {

    // Retrieve the argument list object.
    CefRefPtr<CefListValue> args = message->GetArgumentList();

    // Retrieve message name
    std::string name = message->GetName().ToString();

    CI_LOG_I("Renderer received message: " << name);

    if (name == "js-make-bind") {

        // Check if context pointer is null. if nil store jsfunction name to
        // bind them when the context was created.
        // CefRefPtr<CefV8Context> v8context =
        // CefV8Context::GetCurrentContext();

        CI_LOG_I("Context is valid: " << v8context.get());
        if (v8context.get()) {

            CI_LOG_I("arg size: " << args->GetSize());
            std::string jsFunctionName = args->GetString(0).ToString();

            CI_LOG_I("Context is valid: " << v8context->IsValid());

            CI_LOG_I("Context enter: " << v8context->Enter());

            CefRefPtr<CefV8Value> globalObject = v8context->GetGlobal();

            // Create the "myfunc" function.
            CefRefPtr<CefV8Value> func =
                    CefV8Value::CreateFunction(jsFunctionName, v8handler);

            // Add the "myfunc" function to the "window" object.
            globalObject->SetValue(
                    jsFunctionName, func, V8_PROPERTY_ATTRIBUTE_NONE);

            v8context->Exit();

            v8handler->bindJSFunctionNames.insert(jsFunctionName);

            CI_LOG_I("Created js function " << jsFunctionName);

            return true;
        }
    }

    return false;
}
