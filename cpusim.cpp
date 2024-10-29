#include "CPU.h"

#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
#include<fstream>
#include <sstream>
using namespace std;

/*
Add all the required standard and developed libraries here
*/

/*
Put/Define any helper function/definitions you need here
*/
int main(int argc, char* argv[])
{
	/* This is the front end of your project.
	You need to first read the instructions that are stored in a file and load them into an instruction memory.
	*/

	/* Each cell should store 1 byte. You can define the memory either dynamically, or define it as a fixed size with size 4KB (i.e., 4096 lines). Each instruction is 32 bits (i.e., 4 lines, saved in little-endian mode).
	Each line in the input file is stored as an hex and is 1 byte (each four lines are one instruction). You need to read the file line by line and store it into the memory. You may need a mechanism to convert these values to bits so that you can read opcodes, operands, etc.
	*/

	char instrMem[4096];


	if (argc < 2) {
		//cout << "No file name entered. Exiting...";
		return -1;
	}

	ifstream infile(argv[1]); //open the file
	if (!(infile.is_open() && infile.good())) {
		cout<<"error opening file\n";
		return 0; 
	}
	// string line; 
	// int i = 0;
	// while (infile) {
	// 		infile>>line;
	// 		stringstream line2(line);
	// 		char x; 
	// 		line2>>x;
	// 		instrMem[i] = x; // be careful about hex
	// 		i++;
	// 		line2>>x;
	// 		instrMem[i] = x; // be careful about hex
	// 		cout<<instrMem[i]<<endl;
	// 		i++;
	// 	}
	

	string line;
    int i = 0; // Index for instrMem
    while (getline(infile, line)) {
        if (line.empty()) continue; // Skip empty lines

        // Convert hex string to a byte value
        int byteValue = stoi(line, nullptr, 16);

        // Store in instrMem as a char
        instrMem[i] = static_cast<char>(byteValue);
        i++;
    }

    int maxPC = i; // Set maxPC as the number of bytes loaded
    // cout << "Total bytes loaded: " << maxPC << endl;

    // Test output to ensure bytes are stored correctly
    for (int j = 0; j < maxPC; j++) {
        // cout << hex << setw(2) << setfill('0') << (0xff & instrMem[j]) << " ";
        // if ((j + 1) % 4 == 0) cout << endl; // Print each 4 bytes as an instruction
    }



	// int maxPC= i; 
	// int maxPC= i/4; 

	/* Instantiate your CPU object here.  CPU class is the main class in this project that defines different components of the processor.
	CPU class also has different functions for each stage (e.g., fetching an instruction, decoding, etc.).
	*/

	CPU myCPU(instrMem);  // call the approriate constructor here to initialize the processor...  
	// make sure to create a variable for PC and resets it to zero (e.g., unsigned int PC = 0); 

	/* OPTIONAL: Instantiate your Instruction object here. */
	//Instruction myInst; 
	
	
	bool done = true;
	while (done == true) // processor's main loop. Each iteration is equal to one clock cycle.  
	{
		int result;
		// fetch
		unsigned long instr = myCPU.fetch();
		// cout << "Fetched: 0x" << instr << endl;

		// decode 
		DecodedInstruction decoded_instr = myCPU.decode(instr);
		// cout << "Decoded: Imm. : " << decoded_instr.immediate << endl;
		
		// Generating ControlSignals for execution
		ControlSignals control_signals = myCPU.generateControlSignals(decoded_instr.opcode);
		unsigned int ALUControl = myCPU.generateALUControl(control_signals.ALUOp, decoded_instr.funct3, decoded_instr.funct7);
		// cout << "Created Signals: ALU Op: " << control_signals.ALUOp << ", ALU Control Signal: " << ALUControl << endl;

		// execute
		int ALUResult = myCPU.execute(decoded_instr, control_signals, ALUControl);
		// cout << "Executed" << endl;

		// memory
		int memoryData = 0;
		if(control_signals.MemWrite || control_signals.MemRead) {	
			memoryData = myCPU.memoryAccess(decoded_instr, control_signals, ALUResult);
			// cout << "Memory Accessed" << endl;
		}

		

		if(control_signals.MemRead) result = memoryData;
		else result = ALUResult;

		// cout << "ALUResult: " << result << endl;

		myCPU.writeBack(decoded_instr, control_signals, result);

		// write back
		myCPU.updatePC(decoded_instr, control_signals, result);
		// cout << "PC updated: " << myCPU.readPC() << endl;
		// cout << endl;
		 
		if (myCPU.readPC() >= maxPC) 		// >= because we end up accessing memory we weren't supposed to
			break;
	}
	int a0;
	int a1;
	myCPU.get_result(a0, a1);

	// print the results (you should replace a0 and a1 with your own variables that point to a0 and a1)
	  cout << "(" << a0 << "," << a1 << ")" << endl;
	
	return 0;

}