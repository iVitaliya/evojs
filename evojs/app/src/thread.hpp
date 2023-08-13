#include <v8.h>
#include <uv.h>

class Threading
{
    public:
        static void RunThread(void *arg);
        static void Thread(const v8::FunctionCallbackInfo<v8::Value> &args);
};