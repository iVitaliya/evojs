#include <v8.h>
#include <uv.h>

#include "./util.hpp"

class Timer
{
    public:
        static void Initialise(uv_loop_t *event_loop);
        
        static void SetImmediate(const v8::FunctionCallbackInfo<v8::Value> &args);
        static void onImmediateCallback(uv_async_t *handle);
        static void onDeleteHandle(uv_handle_t *handle);

        static void SetInterval(const v8::FunctionCallbackInfo<v8::Value> &args);
        static void onIntervalCallback(uv_timer_t *handle);

        static void SetTimeout(const v8::FunctionCallbackInfo<v8::Value> &args);
        static void onTimeoutCallback(uv_timer_t *handle);
};

