#include "v8.h"

#include "./src/evojs.hpp"

int main(int argc, char *argv[])
{
    char *filename = argv[1];
    auto *evojs = new EvoJS();
    std::unique_ptr<v8::Platform> platform = evojs->InitialiseV8(argc, argv);

    evojs->InitialiseVM();
    evojs->InitialiseProgram(filename);
    evojs->Shutdown();

    return 0;
}