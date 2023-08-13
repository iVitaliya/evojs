#include "v8.h"

class Fs
{
    public:
        // Copied from https://github.com/v8/v8/blob/master/samples/shell.cc
        static v8::MaybeLocal<v8::String> ReadFile(v8::Isolate *isolate, const char *name);
};