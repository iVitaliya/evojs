#include <libplatform/libplatform.h>
#include <uv.h>
#include "v8.h"

#include "./fs.hpp"
#include "./util.hpp"
#include "./timer.hpp"
#include "./thread.hpp"
#include "./evojs.hpp"

uv_loop_t *DEFAULT_LOOP = uv_default_loop();
v8::Isolate *isolate;
v8::Local<v8::Context> context;
std::unique_ptr<v8::Platform> *pltfrm;
v8::Isolate::CreateParams crt_prms;

void WaitForEvents()
{
    uv_run(DEFAULT_LOOP, UV_RUN_DEFAULT);
}

void ExecuteScriptAndWaitForEvents(char *filename)
{
    // Enter the context for compiling and running the hello world script.
    v8::Context::Scope context_scope(context);
    {
        // Read file from args.
        v8::Local<v8::String> source;

        if (!Fs::ReadFile(isolate, filename).ToLocal(&source))
        {
            Util::LogErrorMessage(stderr, "error", "invalid_file", "couldn't read the provided file");

            return;
        }

        // Create a string containing the JavaScript source code.
        v8::ScriptOrigin origin(isolate, Util::v8_str(filename));

        // Compile the source code.
        v8::Local<v8::Script> script = v8::Script::Compile(
            context, source, &origin).ToLocalChecked();
        
        // Run the script to get the result.
        v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

        // Convert the result to an UTF-8 string and print it.
        v8::String::Utf8Value utf8(isolate, result);

        // fprintf(stderr, "result")
        WaitForEvents();
    }
}

static void Log(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    bool first = true;

    for (int i = 0; i < args.Length(); i++)
    {
        v8::HandleScope handle_scope(args.GetIsolate());

        if (first)
        {
            first = false;
        }
        else
        {
            printf(" ");
        }

        v8::String::Utf8Value str(args.GetIsolate(), args[i]);

        printf("%s", *str);
    }

    printf("\n");
    fflush(stdout);
}

std::unique_ptr<v8::Platform> EvoJS::InitialiseV8(int argc, char *argv[])
{
    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();

    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();

    pltfrm = &platform;
    return platform;
}

void EvoJS::InitialiseVM()
{
    // Create a new isolate and make it the current one.
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    isolate = v8::Isolate::New(create_params);
    crt_prms = create_params;
}

void EvoJS::InitialiseProgram(char *filename)
{
    v8::Isolate::Scope isolate_scope(isolate);

    // Create a stack-allocated handle scope.
    v8::HandleScope handle_scope(isolate);

    // Create a template for the global object.
    v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);

    // Bind the global 'say' function to the C++ Log callback.
    global->Set(isolate, "say", v8::FunctionTemplate::New(isolate, Log));

    // Set a timer variable and initialise the loop.
    Timer timer;
    timer.Initialise(DEFAULT_LOOP);

    // Bind the global time-loop functions to 'execImmediately', 'execInterval' & 'execTimeout' to the C++ SetImmediate, SetInterval & SetTimeout callback.
    global->Set(isolate, "execImmediately", v8::FunctionTemplate::New(isolate, timer.SetImmediate));
    global->Set(isolate, "execInterval", v8::FunctionTemplate::New(isolate, timer.SetInterval));
    global->Set(isolate, "execTimeout", v8::FunctionTemplate::New(isolate, timer.SetTimeout));

    // Bind the 'execThread' to the C++ Thread callback.
    global->Set(isolate, "execThread", v8::FunctionTemplate::New(isolate, Threading::Thread));

    // Create a new context.
    context = v8::Context::New(isolate, NULL, global);

    ExecuteScriptAndWaitForEvents(filename);
}

void EvoJS::Shutdown()
{
    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::DisposePlatform();

    delete crt_prms.array_buffer_allocator;
}