/// AmusementPark3D - Main Entry Point
/// A complete 3D amusement park simulator using OpenGL 4.6 Core Profile.

#include "core/Engine.h"

#ifdef _WIN32
#include <Windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

int main(int /*argc*/, char* /*argv*/[]) {
#ifdef _WIN32
    // Enable virtual terminal processing for colored console output
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &dwMode)) {
        SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif

    std::cout << "========================================\n";
    std::cout << "  AmusementPark3D v1.0\n";
    std::cout << "  3D Amusement Park Simulator\n";
    std::cout << "========================================\n\n";

    ap3d::Engine engine;

    if (!engine.init()) {
        std::cerr << "[FATAL] Engine initialization failed.\n";
        return 1;
    }

    engine.run();
    engine.shutdown();

    std::cout << "\n[Main] Application exited normally.\n";
    return 0;
}
