#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
using namespace std;


// class instruction { // optional
// public:
// 	bitset<32> instr;//instruction
// 	instruction(bitset<32> fetch); // constructor

// };

class DecodedInstruction;
class ControlSignals;
class Controller;
class ALUController;

class CPU {
private:
	int dmemory[4096]; //data memory byte addressable in little endian fashion;
	unsigned long PC; //pc 
	int registers[32]; 			// Register file
	char* instrMem;				// Giving CPU object instrMem pointer

	// Controller mainController;
    // ALUController aluController;

	int extractSImmediate(unsigned int instruction);
	int extractBImmediate(unsigned int instruction);
	int extractJImmediate(unsigned int instruction);

	// int ALU(DecodedInstruction instr, unsigned int ALUControl);

public:
	CPU(char* instrMem);
	unsigned long readPC();
	void incPCtemp();
	unsigned int fetch();
	DecodedInstruction decode(unsigned int instruction);
	ControlSignals generateControlSignals(const unsigned int opcode);
	unsigned int generateALUControl(unsigned int ALUOp, unsigned int funct3, unsigned int funct7);
	int execute(const DecodedInstruction instr, const ControlSignals signals, const unsigned int ALUControl);
	int memoryAccess(const DecodedInstruction instr, const ControlSignals signals, int ALUResult);            
	void writeBack(const DecodedInstruction instr, const ControlSignals signals, int result);
    void updatePC(const DecodedInstruction instr, const ControlSignals signals, int operand1, int operand2);
};

// add other functions and objects here
class DecodedInstruction {
public:
    unsigned int opcode;
    unsigned int rd;
    unsigned int funct3;
    unsigned int rs1;
    unsigned int rs2;
    unsigned int funct7;
    int immediate;
    // Add any other fields you need
};

class ControlSignals {		// from slide 7 lecture 3
public:
	// bool PCSrc;
    bool RegWrite = false;
    bool MemRead = false;
    bool MemWrite = false;
    bool ALUSrc = false;
    bool MemtoReg = false;
    bool Branch = false;
    bool Jump = false;

	/*
	All the operations needed for ALU
		0: ADD - ADD, LB, LW, SB, SW, JAL
		1: SUB - BEQ
		2: Shift Left - LUI
		3: Arithmetic Shift right - SRAI 
		4: OR - ORI
		5: XOR - XOR

	ALUOp possibel values:
		0: Load/Store, JAL - LB, LW, SB, SW, JAL (Addition)
		1: I-type - ORI, SRAI
		2: R-type - ADD, XOR
		3: Branches - subtraction
		4: U-type - LUI
	
	*/
    unsigned int ALUOp = 0;		// Default 
};

enum ALUOperation {
    ADD = 0,          // Addition for ADD, load/store, and JAL
    SLL = 1,          // Shift left logical for LUI
    OR = 2,           // Logical OR for ORI
    XOR = 3,          // Bitwise XOR for XOR
    SRA = 4,          // Shift right arithmetic for SRAI
    SUB = 5           // Subtraction for BEQ
};