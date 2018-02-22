#include <string>

#include "cinder/Log.h"
#include "cinderCEFClientApp.h"
#include "cef_build.h"

void CinderCEFClientApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
    mV8Context = context;
    // Retrieve the context's window object.
    // CefRefPtr<CefV8Value> globalObject = context->GetGlobal();

    // printf("context pointer: '%n' \n", context.get());

    // Create an instance of my CefV8Handler object.
    // CefRefPtr<CefV8Handler> mV8Handler = new MyV8Handler();

    // Create the "myfunc" function.
    // CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("myfunc",
    // mV8Handler);

    // Add the "myfunc" function to the "window" object.
    // globalObject->SetValue("myfunc", func, V8_PROPERTY_ATTRIBUTE_NONE);

    CI_LOG_I("OnContextCreated\n");

    // Tell browser that V8 Context was created
    CefRefPtr<CefProcessMessage> message =
            CefProcessMessage::Create(CefString("OnV8ContextCreated"));
    // Send message
    browser->SendProcessMessage(PID_BROWSER, message);
}

void CinderCEFClientApp::OnContextReleased(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {

    mV8Context = NULL;
    // Tell browser that V8 Context was released
    CefRefPtr<CefProcessMessage> message =
            CefProcessMessage::Create(CefString("OnV8ContextReleased"));
    // Send message
    browser->SendProcessMessage(PID_BROWSER, message);
}

bool CinderCEFClientApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) {

    // Retrieve the argument list object.
    CefRefPtr<CefListValue> args = message->GetArgumentList();

    // Retrieve message name
    std::string name = message->GetName().ToString();

    CI_LOG_I("Renderer received message: " << name);

    if (name == "js-make-bind") {

        // Check if context pointer is null. if nil store jsfunction name to
        // bind them when the context was created.
        // CefRefPtr<CefV8Context> mV8Context =
        // CefV8Context::GetCurrentContext();

        CI_LOG_I("Context is valid: " << mV8Context.get());
        if (mV8Context.get()) {

            CI_LOG_I("arg size: " << args->GetSize());
            std::string jsFunctionName = args->GetString(0).ToString();

            CI_LOG_I("Context is valid: " << mV8Context->IsValid());

            CI_LOG_I("Context enter: " << mV8Context->Enter());

            CefRefPtr<CefV8Value> globalObject = mV8Context->GetGlobal();

            // Create the "myfunc" function.
            CefRefPtr<CefV8Value> func =
                    CefV8Value::CreateFunction(jsFunctionName, mV8Handler);

            // Add the "myfunc" function to the "window" object.
            globalObject->SetValue(
                    jsFunctionName, func, V8_PROPERTY_ATTRIBUTE_NONE);

            mV8Context->Exit();

            mV8Handler->bindJSFunctionNames.insert(jsFunctionName);

            CI_LOG_I("Created js function " << jsFunctionName);

            return true;
        }
    }

    return false;
}
