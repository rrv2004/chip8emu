#include "Chip8.h"
#include <chrono>
#include <cstdint>
#include <endian.h>
#include <fstream>
#include <random>
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