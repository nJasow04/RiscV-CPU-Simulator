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


// Decode stage

int CPU::extractSImmediate(unsigned int instruction) {
    int imm4_0 = (instruction >> 7) & 0x1F;     // Bits 11:7
    int imm11_5 = (instruction >> 25) & 0x7F;   // Bits 31:25
    int immediate = (imm11_5 << 5) | imm4_0;    // Combine into 12-bit immediate

    // Sign-extend to 32 bits
    if (immediate & 0x800) { // Check if the 12th bit is set
        immediate |= 0xFFFFF000; // Sign-extend by setting the upper bits to 1
    }
    return immediate;
}

int CPU::extractBImmediate(unsigned int instruction) {
    int imm11 = (instruction >> 7) & 0x1;       // Bit 7
    int imm4_1 = (instruction >> 8) & 0xF;      // Bits 11:8
    int imm10_5 = (instruction >> 25) & 0x3F;   // Bits 30:25
    int imm12 = (instruction >> 31) & 0x1;      // Bit 31

    int immediate = (imm12 << 12) | (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1);

    // Sign-extend to 32 bits
    if (immediate & 0x1000) { // Check if the 13th bit is set
        immediate |= 0xFFFFE000; // Sign-extend by setting the upper bits to 1
    }
    return immediate;
}

int CPU::extractJImmediate(unsigned int instruction) {
    int imm20 = (instruction >> 31) & 0x1;      // Bit 31
    int imm10_1 = (instruction >> 21) & 0x3FF;  // Bits 30:21
    int imm11 = (instruction >> 20) & 0x1;      // Bit 20
    int imm19_12 = (instruction >> 12) & 0xFF;  // Bits 19:12

    int immediate = (imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1);

    // Sign-extend to 32 bits
    if (immediate & 0x100000) { // Check if the 21st bit is set
        immediate |= 0xFFE00000; // Sign-extend by setting the upper bits to 1
    }
    return immediate;
}

// Extract decoded instruction

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
		decoded.rs2 = -1;
		decoded.funct7 = -1;			// technically SRAI does have funct7 but doesn't matter here
		if (decoded.opcode == 0x13){
			// send signal for ORI
			if (decoded.funct3 == 6){
        		decoded.immediate = ((int)instruction) >> 20; // Sign-extend
				cout << "ORI" << endl;
			}
			else if (decoded.funct3 == 5) {
				decoded.immediate = (instruction >> 20) & 0x1F;		// Extract shamt
				cout << "SRAI" << endl;
			}
			else cerr << "invalid instruction" << endl;
		}
		else if (decoded.funct3 == 0){ 	// Case for LB funct3 == 000
			decoded.immediate = ((int)instruction) >> 20;
			cout << "LB" << endl;
		}
		else {							// Case for LW funct3 == 010
			decoded.immediate = ((int)instruction) >> 20;
			cout << "LW" << endl;
		}
    }

	else if (decoded.opcode == 0x33) { // R-type, add, xor
		decoded.immediate = -1;

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
		decoded.rs2 = -1;
		decoded.rs1 = -1;
		decoded.funct7 = -1;
		decoded.funct3 = -1;
		cout << "LUI" << endl;
	}

	else if (decoded.opcode == 0x23) { // S-type, SB, SW
		decoded.funct7 = -1;
		decoded.rd = -1;
		decoded.immediate = extractSImmediate(instruction);

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
		decoded.immediate = extractBImmediate(instruction);
		decoded.rd = -1;
		decoded.funct7 = -1;
		cout << "BEQ" << endl;
		// decoded.immediate = 
	}

	else if (decoded.opcode == 0x6f) { // J-type, JAL
		decoded.immediate = extractJImmediate(instruction);
		decoded.rs2 = -1;
		decoded.rs1 = -1;
		decoded.funct7 = -1;
		decoded.funct3 = -1;
		cout << "JAL" << endl;
	}

    return decoded;
}

// Main Controller

ControlSignals CPU::generateControlSignals( const unsigned int opcode) {
    ControlSignals signals = {}; // Initialize all signals to false/0

    switch (opcode) {
        case 0x33: // R-type instructions (e.g., ADD, SUB, XOR)
            signals.RegWrite = true;	// Writing to rd
			signals.ALUOp = 2; 			// ALUOp value for R-type

            signals.ALUSrc = false;    	// Second operand from register, immediate value is not used
            signals.MemtoReg = false;  	// Result from ALU, not memory
			signals.Branch = false;		// We choose to increment PC by 4
			signals.MemRead = false;	// We are not reading any data
			// if 
            break;

        case 0x13: // I-type instructions (e.g. ORI, SRAI)
            signals.RegWrite = true;
            signals.ALUSrc = true;     // Second operand from immediate

            signals.ALUOp = 1;         // ALU operation type for I-type
            break;

        case 0x03: // Load instructions (e.g., LB, LW)
            signals.RegWrite = true;
            signals.MemRead = true;    // Read from memory
            signals.ALUSrc = true;     // Address calculation using immediate
            signals.MemtoReg = true;   // Result from memory
            signals.ALUOp = 0;         // ALU operation for address calculation (ADD)
            break;

        case 0x23: // S-type (Store instructions, e.g., SB, SW)
            signals.MemWrite = true;   // Write to memory
            signals.ALUSrc = true;     // Address calculation using immediate
            signals.ALUOp = 0;         // ALU operation for address calculation (ADD)
            break;

        case 0x63: // B-type (Branch instructions, e.g., BEQ, BNE)
            signals.Branch = true;     // Branch operation
            signals.ALUSrc = false;    // Both operands from registers
            signals.ALUOp = 3;         // ALU operation for branch comparison (SUB)
            break;

        case 0x6F: // J-type (Jump instructions, e.g., JAL)
            signals.RegWrite = true;
            signals.Jump = true;       // Jump operation
            signals.ALUSrc = true;     // Immediate is the jump target
            signals.ALUOp = 0;         // ALU operation not needed for JAL
            break;

        case 0x37: // U-type (e.g., LUI)
            signals.RegWrite = true;
            signals.ALUSrc = true;     // Operand from immediate
            signals.MemtoReg = false;
            signals.ALUOp = 4;         // ALU operation for loading upper immediate
            break;

        default:
            cout << "Unknown opcode: " << hex << opcode << endl;
            break;
    }

    return signals;
}

// ALU Controller
unsigned int CPU::generateALUControl(unsigned int ALUOp, unsigned int funct3, unsigned int funct7) {
    switch (ALUOp) {
        case 0: // For address calculation (e.g., load/store, JAL), use ADD
            return ALUOperation::ADD;

        case 1: // For I-type instructions
            if (funct3 == 0x6) return ALUOperation::OR;   // ORI
            if (funct3 == 0x5) return ALUOperation::SRA; // SRAI 
            break;

        case 2: // For R-type instructions
            if (funct3 == 0x0) return ALUOperation::ADD; // ADD
            if (funct3 == 0x4) return ALUOperation::XOR; // XOR
            break;

        case 3: // For branch instructions, typically use SUB to compare
            return ALUOperation::SUB;

        case 4: // For LUI (load upper immediate), no ALU operation required
            return ALUOperation::SLL; // Could be treated as no-op for LUI

        // Add other cases as needed

        default:
            cout << "Unknown ALUOp: " << ALUOp << endl;
            break;
    }
    return ALUOperation::ADD; // Default to ADD operation as a fallback
}

// Execution time :)
/* 
	Returns writeback value
	Parameters:
		- Instruction: rd, rs1, rs2, immediate 
		- Control Signals: 
*/
int CPU::execute(const DecodedInstruction instr, const ControlSignals signals, unsigned int ALUControl) {
    int operand1 = registers[instr.rs1];   // Value from the source register rs1
    int operand2;

    // Determine the second operand based on ALUSrc control signal
    if (signals.ALUSrc) {
        operand2 = instr.immediate; // Use immediate for I-type instructions and similar cases
    } else {
        operand2 = registers[instr.rs2]; // Use value from rs2 for R-type instructions
    }

    int ALUResult = 0;

    // Perform ALU operation based on ALUControl
    switch (ALUControl) {
        case ALUOperation::ADD:  // For ADD, LW, SW, etc.
            ALUResult = operand1 + operand2;
            break;
        
        case ALUOperation::SUB:  // For BEQ and comparison
            ALUResult = operand1 - operand2;
            break;
        
        case ALUOperation::SLL:  // Logical shift left (for LUI)
            ALUResult = operand2 << 12;
            break;
        
        case ALUOperation::OR:   // For ORI
            ALUResult = operand1 | operand2;
            break;
        
        case ALUOperation::XOR:  // For XOR
            ALUResult = operand1 ^ operand2;
            break;
        
        case ALUOperation::SRA:  // Arithmetic shift right for SRAI
            ALUResult = operand1 >> (operand2 & 0x1F);
            break;

        default:
            cout << "Unknown ALU operation." << endl;
            break;
    }

    // (Optional) You may want to store ALUResult in a temporary register for the next stage
    return ALUResult;
}


int CPU::memoryAccess(const DecodedInstruction instr, const ControlSignals signals, int ALUResult){
	int memoryData = 0;
	if(signals.MemRead){				// Loading
		if (instr.funct3 == 0x2) {
			memoryData = dmemory[ALUResult / 4];
		}
		else if(instr.funct3 == 0x0) {
			memoryData = dmemory[ALUResult] & 0xFF;
			if (memoryData & 0x80) memoryData |= 0xFFFFFF00;
		}
	}

	if(signals.MemWrite) {				// Storing
		if (instr.funct3 == 0x2){
			dmemory[ALUResult / 4] = registers[instr.rs2];
		}
		else if (instr.funct3 == 0x0){
			dmemory[ALUResult] = registers[instr.rs2] & 0xFF;
		}
	}

	return memoryData;
}


void CPU::writeBack(const DecodedInstruction instr, const ControlSignals signals, int result) {

	if(signals.MemWrite) return;

	if(signals.Branch && result != 0) return;
	
	registers[instr.rd] = result;
}

void CPU::updatePC(const DecodedInstruction instr, const ControlSignals signals, int result){
	if (signals.Branch) {
        // Check if the branch condition is met (e.g., BEQ: operand1 == operand2)
        if (result == 0) {
            // Update PC with branch offset, shifted left by 1 (branch offsets are in multiples of 2)
            PC += instr.immediate;
            return;
        }
    } 
    
    if (signals.Jump) {
        // For JAL, PC is updated with the immediate offset (target address)
        PC += instr.immediate;
        return;
    }

    // Default: Increment PC to next instruction
    PC += 4;
}