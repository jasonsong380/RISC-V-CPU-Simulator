#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
using namespace std;


class instruction {
public:
	bitset<32> instr;//instruction
	instruction(bitset<32> fetch); // constructor

};

class CPU {
private:
	int dmemory[4096]; //data memory byte addressable in little endian fashion;
	unsigned long PC; //pc
	int branch;
	int memRead;
	int memToReg;
	string ALUOp;
	int memWrite;
	int aluSrc;
	int regWrite;
	int negative;
	int PCSrc;
	int* rs1;
	int* rs2;
	int* rd;
	int imm;
	int jump;
	int aluResult;
	int addSum;
	string aluInput;
	string str; //instruction being executed
	int readData;
	int newPC;
	int registers[32];
	int rcount;

public:
	CPU();
	unsigned long readPC();
	bitset<32> Fetch(bitset<8> *instmem);
	bool Decode(instruction* instr);
	void setBranch(int state);
	void setMemRead(int state);
	void setMemToReg(int state);
	void setALUOp(string str);
	void setMemWrite(int state);
	void setAluSrc(int state);
	void setRegWrite(int state);
	void setNegative(int state);
	void setPCSrc(int state);
	void setControlSignals(string opCode);
	void setrs1(int val);
	void setrs2(int val);
	void setrd(int val);
	void setRegisters(string instr);
	void immediateGenerator(string instr);
	void execute();
	void aluControl();
	int computeALUresult(int input=2);
	void memory();
	void updatePC();
	void writeBack();
	int readRegister(int index);
	int fetchMemory(int index);
	int getALUResult();

	int getBranch();
	int getMemRead();
	int getMemToReg();
	string getALUOp();
	int getMemWrite();
	int getAluSrc();
	int getRegWrite();
	int getNegative();
	int getPCSrc();
	int getrs1();
	int getrs2();
	int getrd();
	int getrcount();
};

// add other functions and objects here

