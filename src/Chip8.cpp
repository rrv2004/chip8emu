#include "Chip8.h"
#include <endian.h>
#include <fstream>
const unsigned int START_ADDRESS=0x200;

//initialize the pc
Chip8::Chip8(){
    pc=START_ADDRESS;
}
//this loads the contents of the ROM file into the memory
void Chip8::LoadRom(char const* filename){
    //open the file as binary streams
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