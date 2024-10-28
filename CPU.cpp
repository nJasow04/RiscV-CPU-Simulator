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
    if (decoded.opcode == 0x13 || decoded.opcode == 0x03) { // I-type, ORI, SRAI, LB, LW
        decoded.immediate = ((int)instruction) >> 20; // Sign-extend
		// cout << "I-type, immediate: " << decoded.immediate << ", opcode: " << decoded.opcode << endl;
		if (decoded.opcode == 0x13){
			// send signal for ORI
			if (decoded.funct3 == 6){
				cout << "ORI" << endl;
			}
			else if (decoded.funct3 == 5) {
				cout << "SRAI" << endl;
			}
			else cerr << "invalid instruction" << endl;
		}
		else if (decoded.funct3 == 0){ 	// Case for LB funct3 == 000
			cout << "LB" << endl;
		}
		else {							// Case for LW funct3 == 010
			cout << "LW" << endl;
		}
    }

	else if (decoded.opcode == 0x33) { // R-type, add, xor
		if (decoded.funct3 == 0){
			cout << "Add" << endl;
		}
		else if (decoded.funct3 == 4){
			cout << "XOR" << endl;
		}
		else {
			cerr << "invalid instruction" << endl;
		}
	}

	else if (decoded.opcode == 0x37) { // U-type, LUI
		decoded.immediate = ((int)instruction) >> 12;
		cout << "LUI" << endl;
	}

	else if (decoded.opcode == 0x23) { // S-type, SB, SW
		if (decoded.funct3 == 0){
			cout << "SB" << endl;
		}
		else if (decoded.funct3 == 2){
			cout << "SW" << endl;
		}
		else {
			cerr << "invalid instruction" << endl;
		}
	}

	else if (decoded.opcode == 0x63) { // B-type, BEQ
		cout << "BEQ" << endl;
		// decoded.immediate = 
	}

	else if (decoded.opcode == 0x6f) { // J-type, JAL
		cout << "JAL" << endl;
	}

    return decoded;
}


