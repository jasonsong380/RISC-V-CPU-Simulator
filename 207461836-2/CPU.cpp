#include "CPU.h"
#include "inttypes.h"
#include "stdint.h"

instruction::instruction(bitset<32> fetch)
{
	// cout << fetch << endl;
	instr = fetch;
	// cout << instr << endl;
}

CPU::CPU()
{
	PC = 0;						   // set PC to 0
	for (int i = 0; i < 4096; i++) // copy instrMEM
	{
		dmemory[i] = (0);
	}

	for (int i = 0; i < 32; i++)
	{
		registers[i] = 0;
	}

	rcount=0;
	jump=0;
	branch = 0;
	memRead = 0;
	memToReg = 0;
	ALUOp = "00";
	memWrite = 0;
	aluSrc = 0;
	regWrite = 0;
	negative = 0;
	PCSrc = 0;
	rs1 = nullptr;
	rs2 = nullptr;
	rd = nullptr;
	newPC = PC;
}

// Access a particular memory element
int CPU::fetchMemory(int index)
{
	return dmemory[index];
}

// Update PC to equal new PC value computed near the end of the previous clock cycle
void CPU::updatePC()
{
	PC = newPC;
}

bitset<32> CPU::Fetch(bitset<8> *instmem)
{
	bitset<32> instr = ((((instmem[PC + 3].to_ulong()) << 24)) + ((instmem[PC + 2].to_ulong()) << 16) + ((instmem[PC + 1].to_ulong()) << 8) + (instmem[PC + 0].to_ulong())); // get 32 bit instruction
	// PC += 4;																																								 // increment PC
	return instr;
}

void CPU::setControlSignals(string opCode)
{
	if (opCode == "0110011")
	{ // R-type
		setALUOp("10");
		setAluSrc(0);
		setMemToReg(0);
		setRegWrite(1);
		setMemRead(0);
		setMemWrite(0);
		setBranch(0);
		setPCSrc(0);
		setNegative(0);
		rcount++;
	}
	else if (opCode == "0010011")
	{ // I-type
		setALUOp("11");
		setAluSrc(1);
		setMemToReg(0);
		setRegWrite(1);
		setMemRead(0);
		setMemWrite(0);
		setBranch(0);
		setPCSrc(0);
		setNegative(0);
	}
	else if (opCode == "0000011")
	{ // LW
		setALUOp("00");
		setAluSrc(1);
		setMemToReg(1);
		setRegWrite(1);
		setMemRead(1);
		setMemWrite(0);
		setBranch(0);
		setPCSrc(0);
		setNegative(0);
	}
	else if (opCode == "0100011")
	{ // SW
		setALUOp("00");
		setAluSrc(1);
		setMemToReg(0);
		setRegWrite(0);
		setMemRead(0);
		setMemWrite(1);
		setBranch(0);
		setPCSrc(0);
		setNegative(0);
	}
	else if (opCode == "1100011")
	{ // BLT
		setALUOp("01");
		setAluSrc(0);
		setMemToReg(0);
		setRegWrite(0);
		setMemRead(0);
		setMemWrite(0);
		setBranch(1);
		setPCSrc(0);
		setNegative(0);
	}
	else if (opCode == "1100111")
	{ // JALR
		setALUOp("01");
		setAluSrc(1);
		setMemToReg(0);
		setRegWrite(1);
		setMemRead(0);
		setMemWrite(0);
		setBranch(1);
		setPCSrc(1);	// to enable jump instead of going to next instruction
		setNegative(1); // to enable jump instead of going to next instruction
		jump=1;
	}
}

void CPU::setRegisters(string instr)
{
	bitset<5> RS1(instr.substr(12, 5));
	rs1 = &registers[RS1.to_ulong()];
	bitset<5> RS2(instr.substr(7, 5));
	rs2 = &registers[RS2.to_ulong()];
	bitset<5> RD(instr.substr(20, 5));
	rd = &registers[RD.to_ulong()];
}

void CPU::immediateGenerator(string instr)
{
	string opCode = instr.substr(25, 7);
	if (opCode == "0010011" || opCode == "0000011" || opCode == "1100111")
	{ // I-type or LW or JALR
		bitset<32> bits(instr.substr(0, 12));
		
		for(int i=12; i<32; i++){
			bits[i]=bits[11];
		}
		imm = static_cast<int32_t> (bits.to_ulong());
	}
	else if (opCode == "0100011")
	{ // SW
		string s = instr.substr(0, 7) + instr.substr(20, 5);
		bitset<32> bits(s);

		for(int i=12; i<32; i++){
			bits[i]=bits[11];
		}
        imm = static_cast<int32_t> (bits.to_ulong());
	}
	else if (opCode == "1100011")
	{ // BLT
		bitset<32> bits(instr.substr(0, 1) + instr.substr(24, 1) + instr.substr(1, 6) + instr.substr(20, 4));
		
		for(int i=12; i<32; i++){
			bits[i]=bits[11];
		}
        imm = static_cast<int32_t> (bits.to_ulong());
	}
}

bool CPU::Decode(instruction *curr)
{
	// cout<<curr->instr<<endl;
	str = curr->instr.to_string();
	string opCode = str.substr(25, 7);
	if (opCode == "0000000")
	{
		return false;
	}
	else
	{
		setControlSignals(opCode);
	}

	// Set registers rs1, rs2 and rd
	setRegisters(str);

	// Compute immediate based on instruction type
	immediateGenerator(str);

	return true;
}

// Generates ALU control input
void CPU::aluControl()
{
	if (ALUOp == "10")
	{
		if (str.substr(17, 3) == "000")
		{
			if (str.substr(1, 1) == "0")
			{
				aluInput = "0010";
			}
			else
			{
				aluInput = "0110";
			}
		}
		else if (str.substr(17, 3) == "100")
		{
			aluInput = "1111";
		}
		else if (str.substr(17, 3) == "101")
		{
			aluInput = "1000";
		}
	}
	else if (ALUOp == "11")
	{
		if (str.substr(17, 3) == "000")
		{
			aluInput = "0010";
		}
		else if (str.substr(17, 3) == "111")
		{
			aluInput = "0000";
		}
	}
	else if (ALUOp == "00")
	{
		aluInput = "0010";
	}
	else if (ALUOp == "01")
	{
		if (str.substr(17, 3) == "100")
		{
			aluInput = "0110";
		}
		else if (str.substr(17, 3) == "000")
		{
			aluInput = "0001";
		}
	}
}

// Compute output of ALU based on four-bit ALU input control signal
int CPU::computeALUresult(int input2)
{
	if (aluInput == "0010")
	{
		return *rs1 + input2; // ADD,ADDI,LW,SW
	}
	if (aluInput == "0110")
	{
		int difference = *rs1 - input2; // SUB, BLT
		//  Set output bit control signal of ALU indicating whether rs1-input2 is negative
		if (difference < 0)
		{
			negative = 1;
		}
		else
		{
			negative = 0;
		}
		return difference;
	}
	if (aluInput == "1111")
	{
		return *rs1 ^ input2; // XOR
	}
	if (aluInput == "0000")
	{
		return *rs1 & input2; // ANDI
	}
	if (aluInput == "1000")
	{
		return *rs1 >> input2; // SRA
	}

	return *rs1 + input2; // JALR
}

void CPU::execute()
{
	int input2;

	// Multiplexer controlled by ALUSrc to select second input to ALU
	if (aluSrc == 1)
	{
		input2 = imm;
	}
	else
	{
		input2 = *rs2;
	}

	// Compute ALU control input
	aluControl();

	// Compute ALU result
	aluResult = computeALUresult(input2);

	// Compute possible new PC address for branch or jump
	if (jump == 1)
	{
		addSum = aluResult;
	}
	else
	{
		addSum = static_cast<int32_t> (PC + 2 * imm);
	}
}

int CPU::getALUResult()
{
	return aluResult;
}

void CPU::memory()
{

	if (memRead == 1)
	{
		// Create a bitset to represent the 32-bit instruction
		bitset<32> instruction;

		// Extract and set the four bytes in little-endian order
		for (int i = 0; i < 4; i++)
		{
			bitset<8> byte(dmemory[aluResult + i]);
			instruction |= (byte.to_ulong() << (i * 8));
		}

		readData = static_cast<int32_t> (instruction.to_ulong());
	}

	// Write data to memory in little-endian order
	if (memWrite == 1)
	{
		for (int i = 0; i < 4; i++)
		{
			dmemory[aluResult + i] = (*rs2 >> (8 * i)) & 0xFF;
		}
	}

	// Create next PC value based on BLT branch condition
	if (branch == 1 && negative == 1)
	{
		newPC = addSum;
	}
	else
	{
		newPC = static_cast<int32_t> (PC + 4);
	}
}

void CPU::writeBack()
{
	if (regWrite == 1)
	{
		int initVal;

		// Output of first MUX after data memory
		if (memToReg == 1)
		{
			initVal = readData;
		}
		else
		{
			initVal = aluResult;
		}

		// Select data to write to destination register
		if (PCSrc == 1)
		{
			*rd = static_cast<int32_t> (PC + 4);
		}
		else
		{
			*rd = initVal;
		}
	}
}

unsigned long CPU::readPC()
{
	return PC;
}

void CPU::setBranch(int state)
{
	branch = state;
}

void CPU::setMemRead(int state)
{
	memRead = state;
}

int CPU::readRegister(int index)
{
	return registers[index];
}

void CPU::setMemToReg(int state)
{
	memToReg = state;
}

void CPU::setALUOp(string str)
{
	ALUOp = str;
}

void CPU::setMemWrite(int state)
{
	memWrite = state;
}

void CPU::setAluSrc(int state)
{
	aluSrc = state;
}

void CPU::setRegWrite(int state)
{
	regWrite = state;
}

void CPU::setNegative(int state)
{
	negative = state;
}

void CPU::setPCSrc(int state)
{
	PCSrc = state;
}

int CPU::getBranch()
{
	return branch;
}

int CPU::getMemRead()
{
	return memRead;
}

int CPU::getMemToReg()
{
	return memToReg;
}

string CPU::getALUOp()
{
	return ALUOp;
}

int CPU::getMemWrite()
{
	return memWrite;
}

int CPU::getAluSrc()
{
	return aluSrc;
}

int CPU::getRegWrite()
{
	return regWrite;
}

int CPU::getNegative()
{
	return negative;
}

int CPU::getPCSrc()
{
	return PCSrc;
}

int CPU::getrcount(){
	return rcount;
}

// Add other functions here ...
