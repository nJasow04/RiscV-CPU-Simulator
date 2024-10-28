#include "CPU.h"


CPU::CPU(char* instrMem)
{
	PC = 0; //set PC to 0
	this->instrMem = instrMem;

	for (int i = 0; i < 4096; i++) //copy instrMEM		
	{
		dmemory[i] = (0);
	}

	for (int i = 0; i < 32; i++){
		registers[i] = 0;
	}

}


unsigned long CPU::readPC()
{
	return PC;
}
void CPU::incPCtemp()
{
	// PC++;
	PC += 4;
}

// Add other functions here ... 

unsigned int CPU::fetch(){
	unsigned int instruction = 0;
	unsigned int pc = this->PC;

	if (pc + 3 >= 4096){
		std::cerr << "PC Out of bounds" << endl;
		return 0; 
	}

	instruction |= (static_cast<unsigned char>(instrMem[pc])	 << 0);
	instruction |= (static_cast<unsigned char>(instrMem[pc + 1]) << 8);
	instruction |= (static_cast<unsigned char>(instrMem[pc + 2]) << 16);
	instruction |= (static_cast<unsigned char>(instrMem[pc + 3]) << 24);

	return instruction;
}


DecodedInstruction CPU::decode(unsigned int instruction) {
    DecodedInstruction decoded;
    decoded.opcode = instruction & 0x7F; 			// Grabs OpCode
    decoded.rd = (instruction >> 7) & 0x1F;			// Fetches rd register number
    decoded.funct3 = (instruction >> 12) & 0x07;
    decoded.rs1 = (instruction >> 15) & 0x1F;
    decoded.rs2 = (instruction >> 20) & 0x1F;
    decoded.funct7 = (instruction >> 25) & 0x7F;

    // Handle immediate values based on instruction format
    // For example, for I-type instructions:
    if (decoded.opcode == 0x13 || decoded.opcode == 0x03) { // I-type
        decoded.immediate = ((int)instruction) >> 20; // Sign-extend
    }
    // Handle other instruction formats (S-type, B-type, U-type, J-type)
    // ...

    return decoded;
}


