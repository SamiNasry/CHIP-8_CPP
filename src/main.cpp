// Nasry Sami
#include <chrono>
#include <iostream>
#include <string>
#include <cstdlib>

#include "platform.h"
#include "chip8.h"

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << "<Scale> <Delay> <ROM>\n";
        return EXIT_FAILURE;
    }

    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
    char const* romFilename = argv[3];

    Platform platform(
        "CHIP-8 Emulator", 
        CHIP8::VIDEO_WIDTH * videoScale, 
        CHIP8::VIDEO_HEIGHT * videoScale, 
        CHIP8::VIDEO_WIDTH, 
        CHIP8::VIDEO_HEIGHT
    );

    CHIP8 chip8;
    if (!chip8.loadROM(romFilename)) {
        std::cerr << "Error: Could not load ROM " << romFilename << "\n";
        return EXIT_FAILURE;
    }

    int videoPitch = sizeof(chip8.video[0]) * CHIP8::VIDEO_WIDTH;
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;


    while (!quit)
    {
        quit = platform.ProcessInput(chip8.keypad);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (dt > cycleDelay)
        {
            lastCycleTime = currentTime;
            
            chip8.Cycle();
            platform.update(chip8.video, videoPitch);
        }
    }

    return 0;
}   