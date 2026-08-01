//this header defines the chip8 class
#include <cstdint>
#include <random>

class Chip8{
private:
    uint8_t registers[16]{};
    uint8_t memory[4096]{};
    uint16_t index_register{};
    uint16_t pc{};
    uint16_t stack[16]{};
    uint8_t sp{};
    uint8_t delayTimer{};
    uint8_t soundTimer{};
    uint8_t keypad[16]{};
    uint32_t video[64*32]{};
    uint16_t opcode;
    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;
    void OP_00E0();//CLS
    void OP_00EE();//RET
    void OP_1nnn();//JP
    void OP_2nnn();//CALL
    void OP_3xkk();//SE Vx , byte
    void OP_4xkk();//SNE Vx, byte
    void OP_5xy0();//SNE Vx, Vy
    void OP_6xkk();//LD Vx,byte
    void OP_7xkk();//ADD Vx,byte
    void OP_8xy0();//LD Vx,Vy
    void OP_8xy1();//OR Vx,Vy
    void OP_8xy2();//AND Vx,Vy
    void OP_8xy3();//XOR Vx,Vy
    void OP_8xy4();//ADD Vx,Vy
    void OP_8xy5();//SUB Vx,Vy
    void OP_8xy6();//SHR Vx;
    void OP_8xy7();//SUBN Vx,Vy
    void OP_8xyE();//SHL Vx
    void OP_9xy0();//SNE Vx, Vy


public:
    Chip8(); 
    void LoadRom(char const* filename);
};

