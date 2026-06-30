//this header defines the chip8 class
#include <cstdint>

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
public:
    Chip8();
    void LoadRom(char const* filename);
};

