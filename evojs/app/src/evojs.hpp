#include <libplatform/libplatform.h>
#include <uv.h>
#include "v8.h"

#include "./fs.hpp"
#include "./util.hpp"
#include "./timer.hpp"
#include "./thread.hpp"

class EvoJS
{
    public:
        std::unique_ptr<v8::Platform> InitialiseV8(int argc, char *argv[]);        
        void InitialiseVM();
        void InitialiseProgram(char *filename);
        void Shutdown();
};