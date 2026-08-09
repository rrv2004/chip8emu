#include "Chip8.h"
#include <chrono>
#include <cstdint>
#include <endian.h>
#include <fstream>
#include <random>
#include <string.h>
const unsigned int START_ADDRESS=0x200;
const unsigned int FONTSET_SIZE=80;
uint8_t fontset[FONTSET_SIZE]={
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
const unsigned int FONTSET_START_ADDRESS=0x50;

//initialize the chip8 constructor
Chip8::Chip8()
    :randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    //initialize the pc
    pc=START_ADDRESS;
    //loading the fonts onto memory
    for(unsigned int i=0;i<FONTSET_SIZE;i++){
        memory[FONTSET_START_ADDRESS+i]=fontset[i];
    }
    //initialize rng
    randByte=std::uniform_int_distribution<uint8_t>(0,255U);
    
}
//this loads the contents of the ROM file into the memory
void Chip8::LoadRom(char const* filename){
    //open the file as binary streams and move the file pointer to the end
    std::ifstream file(filename, std::ios::binary|std::ios::ate);
    if(file.is_open()){
        //get the size of the file
        std::streampos size=file.tellg();
        char* buffer=new char[size];
        //read the file into the buffer
        file.seekg(0,std::ios::beg);
        file.read(buffer, size);
        file.close();
        //load the buffer into the chip-8 memory
        // Note: chip 8 starts instructions at 0x200
        for(long i=0;i<size;i++){
            memory[START_ADDRESS+i]=buffer[i];
        }
        //free the buffer
        delete [] buffer;
        
    }
    
}
void  Chip8::OP_00E0(){//CLS- clears the screen
    memset(video,0,sizeof(video));
}
void Chip8::OP_00EE(){//RET-return from a subroutine
    --sp;
    pc=stack[sp];
}
void Chip8::OP_1nnn(){//JP- jump to location nnn
    uint16_t address= opcode&0x0FFFu;//select the last three digits of the opcode
    pc= address;
}
void Chip8::OP_2nnn(){//CALL subroutine at nnn
    uint16_t address= opcode&0x0FFFu;
    stack[sp]=pc;
    ++sp;
    pc=address;
}
void Chip8::OP_3xkk(){//SE Vx, byte (skip next instruction if Vx=kk)
    uint8_t Vx=(opcode&0x0F00u)>>8u;
    uint8_t byte=opcode&0x00FFu;
    if(registers[Vx]==byte){
        pc+=2;
    }
}
void Chip8::OP_4xkk(){// SNE Vx, byte (skip next instruction if Vx!=kk)
    uint8_t Vx=(opcode&0x0F00u)>>8u;
    uint8_t byte=opcode&0x00FFu;
    if(registers[Vx]!=byte){
        pc+=2;
    }
}
void Chip8::OP_5xy0(){//SE Vx, Vy(skip next instruction if Vx==Vy)
    uint8_t Vx=(opcode&0x0F00u)>>8u;
    uint8_t Vy=(opcode&0x00F0u)>>4u;
    if(registers[Vx]==registers[Vy]){
        pc+=2;
    }
}
void Chip8::OP_6xkk(){//LD Vx,byte (set Vx=kk)
    uint8_t Vx=(opcode&0x0F00u)>>8u;
    uint8_t byte=(opcode&0x00u);
    registers[Vx]=byte;
}
void Chip8::OP_7xkk(){//ADD Vx, byte (Vx+=kk)
    uint8_t Vx=(opcode&0x0F00u)>>8u;;
    uint8_t byte=opcode&0x00FFu;
    registers[Vx]+=byte;
}
void Chip8::OP_8xy0(){//LD Vx,Vy (set Vx=Vy)
    uint8_t Vx=(opcode&0x0F00u)>>8u;
    uint8_t Vy=(opcode&0x00F0u)>>4u;
    registers[Vx]=registers[Vy];
}
void Chip8::OP_8xy1(){//OR Vx,Vy (set Vx=Vx OR Vy)
    uint8_t Vx=(opcode&0x0F00u)>>8u;
    uint8_t Vy=(opcode&0x00F0u)>>4u;
    registers[Vx]|=registers[Vy];
}
void Chip8::OP_8xy2(){//AND Vx,Vy (set Vx=Vx AND Vy)
    uint8_t Vx=(opcode&0x0F00u)>>8u;
    uint8_t Vy=(opcode&0x00F0u)>>4u;
    registers[Vx]&=registers[Vy];
}
void Chip8::OP_8xy3(){//XOR Vx,Vy (set Vx=Vx XOR Vy)
    uint8_t Vx=(opcode&0x0F00u)>>8u;
    uint8_t Vy=(opcode&0x00F0u)>>4u;
    registers[Vx]^=registers[Vy];
}
void Chip8::OP_8xy4(){//ADD Vx,Vy (set Vx=Vx+Vy, VF=carry)
    uint8_t Vx=(opcode&0x0F00u)>>8u;
    uint8_t Vy=(opcode&0x00F0u)>>4u;
    uint16_t sum=registers[Vx]+registers[Vy];
    registers[0xF]=sum>0x00FFu?1:0;
    registers[Vx]=sum&0x00FFu;
}
void Chip8::OP_8xy5(){//SUB Vx,Vy (set Vx=Vx-Vy, VF=NOT borrow)
    uint8_t Vx=(opcode&0x0F00u)>>8u;
    uint8_t Vy=(opcode&0x00F0u)>>4u;
    registers[0xF]=registers[Vx]>registers[Vy]?1:0;
    registers[Vx]-=registers[Vy];
}
void Chip8::OP_8xy6(){//SHR Vx (set VF=least significant bit of Vx, Vx>>=1)
    uint8_t Vx=(opcode&0x0F00u)>>8u;
    registers[0xF]=registers[Vx]&0x1u;
    registers[Vx]>>=1;
}
void Chip8::OP_8xy7(){//SUBN Vx,Vy (set Vx=Vy-Vx, VF=NOT borrow)
    uint8_t Vx=(opcode&0x0F00u)>>8u;
    uint8_t Vy=(opcode&0x00F0u)>>4u;
    registers[0xF]=registers[Vy]>registers[Vx]?1:0;
    registers[Vx]=registers[Vy]-registers[Vx];
}
void Chip8::OP_8xyE(){//SHL Vx (set VF=most significant bit of Vx, Vx<<=1)
    uint8_t Vx=(opcode&0x0F00u)>>8u;
    registers[0xF]=(registers[Vx]&0x80u)>>7u;
    registers[Vx]<<=1;
}
void Chip8::OP_9xy0(){//SNE Vx,Vy (skip next instruction if Vx!=Vy)
    uint8_t Vx=(opcode&0x0F00u)>>8u;
    uint8_t Vy=(opcode&0x00F0u)>>4u;
    if(registers[Vx]!=registers[Vy]){
        pc+=2;
    }
}
void Chip8::OP_Annn(){//LD I, addr (set I=nnn)
    uint16_t address=opcode&0x0FFFu;
    index_register=address;
}
void Chip8::OP_Bnnn(){//JP V0, addr (jump to location nnn+V0)
    uint16_t address=opcode&0x0FFFu;
    pc=registers[0]+address;
}