#include "chip8.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>





CHIP8::CHIP8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()){
    
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

    Pc = START_ADDRESS;

    uint8_t tempFontset[FONTSET_SIZE] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
    {
        fontset[i] = tempFontset[i];
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }


    table[0x0] = &CHIP8::Table0;
    table[0x1] = &CHIP8::OP_1nnn;
    table[0x2] = &CHIP8::OP_2nnn;
    table[0x3] = &CHIP8::OP_3xkk;
    table[0x4] = &CHIP8::OP_4xkk;
    table[0x5] = &CHIP8::OP_5xy0;
    table[0x6] = &CHIP8::OP_6xkk;
    table[0x7] = &CHIP8::OP_7xkk;
    table[0x8] = &CHIP8::Table8;
    table[0x9] = &CHIP8::OP_9xy0;
    table[0xA] = &CHIP8::OP_Annn;
    table[0xB] = &CHIP8::OP_Bnnn;
    table[0xC] = &CHIP8::OP_Cxkk;
    table[0xD] = &CHIP8::OP_Dxyn;
    table[0xE] = &CHIP8::TableE;
    table[0xF] = &CHIP8::TableF;

    // avoid garbage
    for (size_t i = 0; i <= 0xE; i++)
    {
        table0[i] = &CHIP8::OP_NULL;
        table8[i] = &CHIP8::OP_NULL;
        tableE[i] = &CHIP8::OP_NULL;
    }

    table0[0x0] = &CHIP8::OP_00E0;
    table0[0xE] = &CHIP8::OP_00EE;


    table8[0x0] = &CHIP8::OP_8xy0;
    table8[0x1] = &CHIP8::OP_8xy1;
    table8[0x2] = &CHIP8::OP_8xy2;
    table8[0x3] = &CHIP8::OP_8xy3;
    table8[0x4] = &CHIP8::OP_8xy4;
    table8[0x5] = &CHIP8::OP_8xy5;
    table8[0x6] = &CHIP8::OP_8xy6;
    table8[0x7] = &CHIP8::OP_8xy7;
    table8[0xE] = &CHIP8::OP_8xyE;

    tableE[0x1] = &CHIP8::OP_ExA1;
    tableE[0xE] = &CHIP8::OP_Ex9E;

    
    // avoid garbage 
    for (size_t i = 0; i <= 0x65; i++)
    {
        tableF[i] = &CHIP8::OP_NULL;
    }

    tableF[0x07] = &CHIP8::OP_Fx07;
    tableF[0x0A] = &CHIP8::OP_Fx0A;
    tableF[0x15] = &CHIP8::OP_Fx15;
    tableF[0x18] = &CHIP8::OP_Fx18;
    tableF[0x1E] = &CHIP8::OP_Fx1E;
    tableF[0x29] = &CHIP8::OP_Fx29;
    tableF[0x33] = &CHIP8::OP_Fx33;
    tableF[0x55] = &CHIP8::OP_Fx55;
    tableF[0x65] = &CHIP8::OP_Fx65;
}

void CHIP8::Table0()
{
    ((*this).*(table0[opcode & 0x000Fu]))();
}

void CHIP8::Table8()
{
    ((*this).*(table8[opcode & 0x000Fu]))();
}

void CHIP8::TableE()
{
    ((*this).*(tableE[opcode & 0x000Fu]))();
}

void CHIP8::TableF()
{
    ((*this).*(tableF[opcode & 0x00FFu]))();
}

void CHIP8::OP_NULL()
	{}

// This function is improved better than the one in the website
bool CHIP8::loadROM(const char* filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if(file.is_open())
    {
        std::cout << "Inside file" << std::endl;
        std::streampos size = file.tellg();

        file.seekg(0, std::ios::beg);
        
        std::vector<char> buffer(size);
        if(file.read(buffer.data(), size))
        {
            for (long i = 0; i < size; i++)
            {
                memory[START_ADDRESS + i] = buffer[i];
            }
            return 1;
        }
        return 0;
    }
    return 0;
}

void CHIP8::OP_00E0(){
    std::fill(std::begin(video), std::end(video), 0);
}

void CHIP8::OP_00EE(){
    --sp;
    Pc = stack[sp];

}

void CHIP8::OP_1nnn()
{
    uint16_t address = opcode & 0x0FFFu;
    Pc = address;
}

void CHIP8::OP_2nnn() // empty stack convention
{
    uint16_t address = opcode & 0x0FFFu;

    stack[sp] = Pc;
    ++sp;
    Pc = address;
}

void CHIP8::OP_3xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = (opcode & 0x00FF);

    if(V[Vx] == byte)
    {
        Pc += 2;
    }
}

void CHIP8::OP_4xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = (opcode & 0x00FFu);

    if(V[Vx] != byte)
    {
        Pc += 2;
    }
}

void CHIP8::OP_5xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u ;
    if(V[Vx] == V[Vy]){
        Pc += 2;
    }
}

void CHIP8::OP_6xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = (opcode & 0x00FFu);

    V[Vx] = byte;
}

void CHIP8::OP_7xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = (opcode & 0x00FFu);

    V[Vx] +=byte;

}

void CHIP8::OP_8xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    V[Vx] = V[Vy];
}

void CHIP8::OP_8xy1()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    V[Vx] |= V[Vy]; // Vx = Vx OR Vy
}

void CHIP8::OP_8xy2()
{
     uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    V[Vx] &= V[Vy]; // Vx = Vx AND Vy
}

void CHIP8::OP_8xy3()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    V[Vx] ^= V[Vy]; // Vx = Vx XOR Vy
}

void CHIP8::OP_8xy4()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    uint16_t sum = V[Vx] + V[Vy];

    if(sum > 255U)
    {
        V[0xF] = 1;
    }
    else{
        V[0xF] = 0;
    }
    V[Vx]= sum & 0xFFu;
}

void CHIP8::OP_8xy5()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(V[Vx] >= V[Vy])
    {
        V[0xF] = 1;
    }
    else{
        V[0xF] = 0;
    }

    V[Vx] -= V[Vy];
}

void CHIP8::OP_8xy6()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    V[0xF] = (V[Vx] & 0x1u);

    V[Vx] >>= 1;
}

void CHIP8::OP_8xy7()
{
     uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(V[Vy] >= V[Vx])
    {
        V[0xF] = 1;
    }
    else{
        V[0xF] = 0;
    }

    V[Vx] = (V[Vy] - V[Vx]);
}

void CHIP8::OP_8xyE()
{
     uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    V[0xF] = (V[Vx] & 0x80u) >> 7u;

    V[Vx] <<= 1;
}


void CHIP8::OP_9xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(V[Vx] != V[Vy]){
        Pc += 2;
    }
}

void CHIP8::OP_Annn()
{
    uint16_t address = (opcode & 0x0FFFu);

    index = address;
}

void CHIP8::OP_Bnnn()
{
    uint16_t address = (opcode & 0x0FFFu);

    Pc = V[0] + address;
}

void CHIP8::OP_Cxkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    V[Vx] = randByte(randGen) & byte;
}

void CHIP8::OP_Dxyn()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = (opcode & 0x000Fu);

    // Initial starting positions (wrapped if they start off-screen)
    uint8_t xPos = V[Vx] % VIDEO_WIDTH;
    uint8_t yPos = V[Vy] % VIDEO_HEIGHT;

    V[0xF] = 0; // Reset collision register

    for (unsigned int row = 0; row < height; ++row)
    {
        // Safety: If the next row is off the bottom of the screen, stop drawing
        if (yPos + row >= VIDEO_HEIGHT) break;

        uint8_t spriteByte = memory[index + row];

        for (unsigned int col = 0; col < 8; ++col)
        {
            // Safety: If the next pixel is off the right side, skip this pixel
            if (xPos + col >= VIDEO_WIDTH) break;

            uint8_t spritePixel = spriteByte & (0x80u >> col);
            
            // Pointer to the specific pixel in our 1D video array
            uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

            if (spritePixel)
            {
                // Collision Detection: 
                // If the screen pixel is already ON (White), set VF to 1
                if (*screenPixel == 0xFFFFFFFF)
                {
                    V[0xF] = 1;
                }

                // XOR the pixel: 
                // This turns White to Black and Black to White
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}

void CHIP8::OP_Ex9E()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    uint8_t key = V[Vx];

    if (keypad[key])
    {
    Pc += 2;
    }
}

void CHIP8::OP_ExA1()
{
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    uint8_t key = V[Vx];

    if (!keypad[key])
    {
        Pc += 2;
    }
}

void CHIP8::OP_Fx07()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    V[Vx] = delayTimer;
}

void CHIP8::OP_Fx0A()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    bool keyPressed = false;

    for(int i = 0; i < 16; ++i)
    {
        if (keypad[i])
        {
            V[Vx] = i;
            keyPressed = true;
            break;
        }
    }
   
    if(!keyPressed)
    {
        Pc -= 2;
    }

}

void CHIP8::OP_Fx15()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    delayTimer = V[Vx];
}

void CHIP8::OP_Fx18()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    soundTimer = V[Vx];
}

void CHIP8::OP_Fx1E()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    index += V[Vx];
}

void CHIP8::OP_Fx29()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t digit = V[Vx];

    index = FONTSET_START_ADDRESS + (digit *5);
}

void CHIP8::OP_Fx33()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t value = V[Vx];

    memory[index + 2] = value % 10;
    value /= 10;

    memory[index+1] = value % 10;
    value /= 10;

    memory[index] = value%10;
}

void CHIP8::OP_Fx55()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for(uint8_t i = 0; i <= Vx; ++i)
    {
        memory[index + i] = V[i];
    }
}

void CHIP8::OP_Fx65()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for (uint8_t i = 0; i <= Vx; ++i)
    {
        V[i] = memory[index + i];
    }
}


void CHIP8::Cycle()
{
    opcode = (memory[Pc] << 8u) | memory[Pc+1];

    Pc += 2;

    ((*this).*(table[(opcode & 0xF000u) >> 12u]))  ();

    if(delayTimer)
    {
        --delayTimer;
    }

    if(soundTimer)
    {
        --soundTimer;
    }





}