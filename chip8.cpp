#include "chip8.h"
#include <fstream>
#include <vector>
#include <algorithm>




const unsigned int START_ADDRESS = 0x200;

CHIP8::CHIP8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()){
    
    randbyte = std::uniform_int_distribution<uint8_t>(0, 255U);

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
 
}



// This function is improved better than the one in the website
void CHIP8::loadROM(const char* filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if(file.is_open())
    {
        std::streampos size = file.tellg();
        
        std::vector<char> buffer(size);
        if(file.read(buffer.data(), size))
        {
            for (long i = 0; i < size; i++)
            {
                memory[START_ADDRESS + i] = buffer[i];
            }
        }
    }
}

void CHIP8::OP_00E0(){
    std::fill(std::begin(video), std::end(video), 0);
}

void CHIP8::OP_00EE(){
    --sp;
    Pc = stakc[sp];

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

void CHIP::OP_8xyE()
{
     uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    V[0xF] = (V[Vx] & 0x80u) >> 7u;

    V[Vx] <<= 1;
}


void CHIP::OP_9xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if(V[Vx] != V[Vy]){
        Pc += 2;
    }
}

void CHIP8::Annn()
{
    uint16_t address = (opcode & 0x0FFFu);

    index = address;
}

void CHIP8::Bnnn()
{
    uint16_t address = (opcode & 0x0FFFu);

    pc = V[0] + address;
}

void CHIP8::OP_Cxkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    V[Vx] = randByte(randgen) & byte;
}

void CHIP8::OP_Dxyn()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = (opcde & 0x000Fu);

    // Wrap
    uint8_t cPos = V[Vx] % VIDEO_WIDTH;
    uint8_t yPos = V[Vy] % VIDEO_HEIGHT;

    V[0xF] = 0;

    for(unsigned int row = 0; row < heaight; ++row
    {
        uint8_t spriteByte = memory[index + row];

        for(unsigned int col = 0; col < 8; ++col)
        {
            uint8_t spritePixel = spriteByte & (0x8u >> col);
            uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

            if (spritePixel)
            {
                if (*screenPixel == 0xFFFFFFFF)
                {
                    V[0xF] = 1;
                }

                *screenPixel ^= 0xFFFFFFFF;
            }
        }

    })
}

void CHIP8::Ex9E()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    uint8_t key = V[Vx];

    if (keypad[key])
    {
        pc += 2;
    }
}

void CHIP8::ExA1()
{
        uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    uint8_t key = V[Vx];

    if (!keypad[key])
    {
        pc += 2;
    }
}

void CHIP8::Fx07()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    V[Vx] = delayTimer;
}

void CHIP8::Fx0A()
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
        pc -= 2;
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

void CHIP8::Fx1E()
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
    value = V[Vx];

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




