#include "v8.h"

class Util
{
    public:
        static inline v8::Local<v8::String> v8_str(const char *x);
        static void ReportException(v8::Isolate *isolate, v8::TryCatch *tryCatch);
        static void ThrowTypeError(v8::Isolate *isolate, const char *message);
        static std::string GetCurrentTime();
        static void LogErrorMessage(FILE *stream, const char *state, const char *error_type, const char *message);
        static void LogMessage(const char *state, const char *message);
};