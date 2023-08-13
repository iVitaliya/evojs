#include <v8.h>
#include <uv.h>

#include "./util.hpp"
#include "./timer.hpp"

struct timer
{
    v8::Global<v8::Function> callback;
    v8::Isolate *isolate;
};

uv_loop_t *loop;

void Timer::Initialise(uv_loop_t *event_loop)
{
    loop = event_loop;  
}

void Timer::SetImmediate(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    auto isolate = args.GetIsolate();
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Value> callback = args[0];

    if (!callback->IsFunction())
    {
        Util::ThrowTypeError(isolate, "Invalid execImmediate() callback function provided");

        return;
    }

    timer *timerWrap = new timer();

    timerWrap->callback.Reset(isolate, callback.As<v8::Function>());
    timerWrap->isolate = isolate;

    uv_async_t *async = new uv_async_t;
    async->data = (void *)timerWrap;

    uv_async_init(loop, async, onImmediateCallback);
    uv_async_send(async);
}

void Timer::onImmediateCallback(uv_async_t *handle)
{
    timer *timerWrap = (timer *)handle->data;

    if (!timerWrap)
    {
        return;
    }

    v8::Isolate *isolate = timerWrap->isolate;

    if (isolate->IsDead())
    {
        // Don't proceed if isolate is dead.
        delete timerWrap;

        return;
    }

    v8::TryCatch try_catch(isolate);
    v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, timerWrap->callback);
    v8::Local<v8::Value> result;

    if (!callback->Call(isolate->GetCurrentContext(), v8::Undefined(isolate), 0, nullptr).ToLocal(&result))
    {
        Util::ReportException(isolate, &try_catch);
    }

    delete timerWrap;
    uv_close((uv_handle_t *)handle, onDeleteHandle);
}

void Timer::onDeleteHandle(uv_handle_t *handle)
{
    delete (timer *)handle->data;
    delete handle;
}

void Timer::SetTimeout(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    auto isolate = args.GetIsolate();
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Value> callback = args[0];
    int64_t delay = args[1]->IntegerValue(context).ToChecked();

    if (!callback->IsFunction())
    {
        Util::ThrowTypeError(isolate, "Invalid execTimeout() callback function provided");

        return;
    }

    timer *timerWrap = new timer();

    timerWrap->callback.Reset(isolate, callback.As<v8::Function>());
    timerWrap->isolate = isolate;

    uv_timer_t *timer = new uv_timer_t;
    timer->data = (void *)timerWrap;

    uv_timer_init(loop, timer);
    uv_timer_start(timer, onTimeoutCallback, delay, 0);
}

void Timer::onTimeoutCallback(uv_timer_t *handle)
{
    timer *timerWrap = (timer *)handle->data;

    if (!timerWrap)
    {
        return;
    }

    v8::Isolate *isolate = timerWrap->isolate;

    if (isolate->IsDead())
    {
        // Don't proceed if isolate is dead.
        delete timerWrap;

        return;
    }

    v8::TryCatch try_catch(isolate);
    v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, timerWrap->callback);
    v8::Local<v8::Value> result;

    if (!callback->Call(isolate->GetCurrentContext(), v8::Undefined(isolate), 0, nullptr).ToLocal(&result))
    {
        Util::ReportException(isolate, &try_catch);
    }

    delete timerWrap;
}

void Timer::SetInterval(const v8::FunctionCallbackInfo<v8::Value> &args)
{
    auto isolate = args.GetIsolate();
    auto context = isolate->GetCurrentContext();
    v8::Local<v8::Value> callback = args[0];
    int64_t interval = args[1]->IntegerValue(context).ToChecked();

    if (!callback->IsFunction())
    {
        Util::ThrowTypeError(isolate, "Invalid execInterval() callback function provided");

        return;
    }

    timer *timerWrap = new timer();

    timerWrap->callback.Reset(isolate, callback.As<v8::Function>());
    timerWrap->isolate = isolate;

    uv_timer_t *timer = new uv_timer_t;
    timer->data = (void *)timerWrap;

    uv_timer_init(loop, timer);
    uv_timer_start(timer, onIntervalCallback, interval, interval);
}

void Timer::onIntervalCallback(uv_timer_t *handle)
{
    timer *timerWrap = (timer *)handle->data;

    if (!timerWrap)
    {
        return;
    }

    v8::Isolate *isolate = timerWrap->isolate;

    if (isolate->IsDead())
    {
        // Don't proceed if isolate is dead.
        delete timerWrap;

        return;
    }

    v8::TryCatch try_catch(isolate);
    v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate, timerWrap->callback);
    v8::Local<v8::Value> result;

    if (!callback->Call(isolate->GetCurrentContext(), v8::Undefined(isolate), 0, nullptr).ToLocal(&result))
    {
        Util::ReportException(isolate, &try_catch);
    }
}