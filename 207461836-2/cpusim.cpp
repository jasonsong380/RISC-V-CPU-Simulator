#include "CPU.h"

#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

/*
Add all the required standard and developed libraries here
*/

/*
Put/Define any helper function/definitions you need here
*/
int main(int argc, char *argv[])
{
	/* This is the front end of your project.
	You need to first read the instructions that are stored in a file and load them into an instruction memory.
	*/

	/* Each cell should store 1 byte. You can define the memory either dynamically, or define it as a fixed size with size 4KB (i.e., 4096 lines). Each instruction is 32 bits (i.e., 4 lines, saved in little-endian mode).
	Each line in the input file is stored as an unsigned char and is 1 byte (each four lines are one instruction). You need to read the file line by line and store it into the memory. You may need a mechanism to convert these values to bits so that you can read opcodes, operands, etc.
	*/

	bitset<8> instMem[4096];

	if (argc < 2)
	{
		// cout << "No file name entered. Exiting...";
		return -1;
	}

	ifstream infile(argv[1]); // open the file
	if (!(infile.is_open() && infile.good()))
	{
		cout << "error opening file\n";
		return 0;
	}
	string line;
	int i = 0;
	while (infile)
	{
		infile >> line;
		stringstream line2(line);
		int x;
		line2 >> x;
		instMem[i] = bitset<8>(x);
		i++;
	}
	int maxPC = i;

	/* Instantiate your CPU object here.  CPU class is the main class in this project that defines different components of the processor.
	CPU class also has different functions for each stage (e.g., fetching an instruction, decoding, etc.).
	*/

	CPU myCPU; // call the approriate constructor here to initialize the processor...
	// make sure to create a variable for PC and resets it to zero (e.g., unsigned int PC = 0);

	unsigned int cycleCount = 0; // Initialize cycle counter variable

	/* OPTIONAL: Instantiate your Instruction object here. */
	// Instruction myInst;
	bitset<32> curr;
	instruction instr = instruction(curr);
	bool done = true;
	while (done == true) // processor's main loop. Each iteration is equal to one clock cycle.
	{
		cycleCount++; // Increase number of cycles by one
		
		// Update program counter
		myCPU.updatePC();

		// fetch
		curr = myCPU.Fetch(instMem); // fetching the instruction
		instr = instruction(curr);

		// decode
		done = myCPU.Decode(&instr);
		if (done == false) // break from loop so stats are not mistakenly updated
			break;

		myCPU.execute();

		myCPU.memory();

		myCPU.writeBack();

		// sanity check
		if (myCPU.readPC() > maxPC)
			break;
	}

	// print the results (you should replace a0 and a1 with your own variables that point to a0 and a1)
	cout << "(" << myCPU.readRegister(10) << "," << myCPU.readRegister(11) << ")" << endl;
	// cerr << "Cycle count: " << cycleCount << endl;
	// cerr << "R instruction count: " << myCPU.getrcount() << endl;

	return 0;
}