// chip8.h

#include <cstdint>
#include <chrono>
#include <random>

class CHIP8
{
    public:
    
    static constexpr unsigned int START_ADDRESS = 0x200;
    static constexpr unsigned int FONTSET_SIZE = 80;
    static constexpr unsigned int FONTSET_START_ADDRESS = 0x50;
    static constexpr unsigned int VIDEO_WIDTH = 64;
    static constexpr unsigned int VIDEO_HEIGHT =  32;

    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;
    

    uint16_t opcode;
    uint8_t memory[4096]{};
    uint8_t V[16]{}; // Registers
    uint16_t index{},Pc{};
    uint16_t stack[16]{};
    uint8_t sp{};
    uint8_t delayTimer{}, soundTimer{};
    uint8_t keypad[16]{};
    uint32_t video[64 * 32]{};

    uint8_t fontset[FONTSET_SIZE];

    
    typedef void (CHIP8::*Chip8func)();


    Chip8func table[0xF + 1]{};
    Chip8func table0[0xE + 1]{};
    Chip8func table8[0xE + 1]{};
    Chip8func tableE[0xE + 1]{};
    Chip8func tableF[0x65 +1]{};



    CHIP8();

    void Cycle();


    void loadROM(const char* filename);

    // OPCODES

    //CLS - Clear the display
    void OP_00E0();
    
    //RET - return from a subroutine
    void OP_00EE();
    
    //JP addr - Jump to location nnn
    void OP_1nnn();
    
    //CALL addr - Call subroutine at nnn
    void OP_2nnn();

    //3xkk - SE Vx, byte - Skip next instruction if Vx = kk
    void OP_3xkk();

    //4xkk - SNE Vx, byte - Skip next instruction if Vx != kk
    void OP_4xkk();

    //5xk0 - SE Vx, Vy - Skip next instruction if Vx = Vy
    void OP_5xy0();

    //6xkk - LD Vx, byte - Set Vx = kk
    void OP_6xkk();

    //7xkk - ADD Vx, byte - Set Vx = Vx + kk
    void OP_7xkk();

    //8xy0 - LD Vx, Vy - Set Vx = Vy
    void OP_8xy0();

    //8xy1 - OR Vx, Vy - Set Vx = Vx OR Vy
    void OP_8xy1();

    //8xy2 - AND Vx, Vy - Set Vx = Vx AND Vy
    void OP_8xy2();

    //8xy3 - XOR Vx, Vy - Set Vx = Vx XOR Vy
    void OP_8xy3();

    //8xy4 - ADD Vx, Vy - Set Vx = Vy, set Vf = carry
    void OP_8xy4();

    //8xy5 - SUB Vx, Vy - SET Vx = Vx - Vy, set VF = NOT borrow
    void OP_8xy5();

    //8xy6 - SHR Vx - Set Vx = Vx SHR 1
    void OP_8xy6();

    //8xy7 - SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT borrow
    void OP_8xy7();

    //8xyE - SHL Vx {, Vy} - Set Vx = Vx SHL 1
    void OP_8xyE();

    //9xy0, Skilp next Instruction if Vx != Vy
    void OP_9xy0();

    //Annn - LD I , addr Set I = nnn
    void OP_Annn();

    //Bnnn - JP V0, addr - Jump to location nnn + V0
    void OP_Bnnn();

    // Cxkk - RND Vx, byte - Set Vx = random byte AND kk
    void OP_Cxkk();

    // Dxyn - DRW Vx, Vy, nibble - Display/draw sprite
    void OP_Dxyn();


    // Ex9E, ExA1 - Skip on key press/not pressed
    void OP_Ex9E();
    void OP_ExA1();


    void OP_Fx07();
    void OP_Fx0A();
    void OP_Fx15();
    void OP_Fx18();
    void OP_Fx1E();
    void OP_Fx29();
    void OP_Fx33();
    void OP_Fx55();
    void OP_Fx65();

    // helper functions
    void Table0();
    void Table8();
    void TableE();
    void TableF();
    void OP_NULL();










};