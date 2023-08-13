#include <iostream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <cctype>
#include <string>
#include "v8.h"

#include "./util.hpp"

v8::Local<v8::String> Util::v8_str(const char *x)
{
    return v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), x).ToLocalChecked();
}

void Util::ReportException(v8::Isolate *isolate, v8::TryCatch *try_catch)
{
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Message> message = try_catch->Message();

    if (!message.IsEmpty())
    {
        v8::String::Utf8Value exception(isolate, try_catch->Exception());
        v8::String::Utf8Value filename(isolate, message->GetScriptOrigin().ResourceName());
        int linenum = message->GetLineNumber(isolate->GetCurrentContext()).FromJust();

        fprintf(stderr, "An Error has occurred in \"%s\" at line: %d\nException: %s", *filename, linenum, *exception);

        v8::String::Utf8Value sourceline(isolate, message->GetSourceLine(isolate->GetCurrentContext()).ToLocalChecked());
        fprintf(stderr, "\n\n%s\n", *sourceline);

        // Print wavy underline
        int start = message->GetStartColumn(isolate->GetCurrentContext()).FromJust();

        for (int i = 0; i < start; i++)
        {
            fprintf(stderr, " ");
        }

        int end = message->GetEndColumn(isolate->GetCurrentContext()).FromJust();

        for (int i = 0; i < end; i++) {
            fprintf(stderr, "~");
        }

        fprintf(stderr, "\n");

        v8::Local<v8::Value> stack_trace_string;

        if (try_catch->StackTrace(isolate->GetCurrentContext()).ToLocal(&stack_trace_string) &&
            stack_trace_string->IsString() &&
            v8::Local<v8::String>::Cast(stack_trace_string)->Length() > 0)
        {
            v8::String::Utf8Value stack_trace(isolate, stack_trace_string);

            fprintf(stderr, "%s\n", *stack_trace);
        }
    }
}

void Util::ThrowTypeError(v8::Isolate *isolate, const char *message)
{
    v8::Local<v8::String> error_message = v8::String::NewFromUtf8(isolate, message, v8::NewStringType::kNormal).ToLocalChecked();

    isolate->ThrowException(v8::Exception::TypeError(error_message));
}

std::string Util::GetCurrentTime()
{
    // Get the current time.
    auto current_now = std::chrono::system_clock::now();
    
    // Convert the time to `t`.
    std::time_t current_now_t = std::chrono::system_clock::to_time_t(current_now);

    // Format the time.
    char formattedTime[100];
    std::strftime(formattedTime, sizeof(formattedTime), "%B the %e %Y @ %I:%M:%S %p", std::localtime(&current_now_t));

    return std::string(formattedTime);
}

void Util::LogErrorMessage(FILE *stream, const char *state, const char *error_type, const char *message)
{
    std::string time = GetCurrentTime();
    std::string *upper = &std::string(state);
    std::string *err_t = &std::string(error_type);

    std::transform(upper->begin(), upper->end(), upper->begin(), ::toupper);
    std::transform(err_t->begin(), err_t->end(), err_t->begin(), ::toupper);

    fprintf(stream, "[%s | %s] (%s): %s\n", time, upper, err_t, message);
    fflush(stream);
}

void Util::LogMessage(const char *state, const char *message)
{
    std::string time = GetCurrentTime();
    std::string *state_string = &std::string(state);

    std::transform(state_string->begin(), state_string->end(), state_string->begin(), ::toupper);

    fprintf(stdout, "[%s] (%s): %s", time, state_string, message);
    fflush(stdout);
}