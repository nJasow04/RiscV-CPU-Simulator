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

class CPU {
private:
	int dmemory[4096]; //data memory byte addressable in little endian fashion;
	unsigned long PC; //pc 
	int registers[32]; 			// Register file
	char* instrMem;				// Giving CPU object instrMem pointer

public:
	CPU(char* instrMem);
	unsigned long readPC();
	void incPCtemp();
	unsigned int fetch();
	DecodedInstruction decode(unsigned int instruction);
	// ControlSignals generateControlSignals(const DecodedInstruction& instr);
	// int execute(const DecodedInstruction instr, const ControlSignals signals, int rsd, int rs1, int rs2); 
	// int memoryAccess(const DecodedInstruction& instr, const ControlSignals& signals, int ALUResult);            
	// void writeBack(const DecodedInstruction& instr, const ControlSignals& signals, int result);
    // void updatePC(const DecodedInstruction& instr, const ControlSignals& signals, int operand1, int operand2);
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

class ControlSignals {
public:
	bool PCSrc;
    bool RegWrite;
    bool MemRead;
    bool MemWrite;
    bool ALUSrc;
    bool MemtoReg;
    bool Branch;
    bool Jump;
    unsigned int ALUOp;
};