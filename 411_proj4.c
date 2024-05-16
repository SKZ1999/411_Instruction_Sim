// UMBC Fall 2023 Project 4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <stdio.h>
#include <stdlib.h>

// simulated computer memory 200 (32 bit) words
int mem[201] = { 0 };
// simulated register file -- 32 registers
// R0 always = 0;
// R1 always = 1;
int  reg_file[32] = { 0 };

int PCR = 0; // program counter register

// register-integer class
const int ADD = 0x0;
const int SUB = 0x1;
const int AND = 0x2;
const int OR = 0x3;
const int XOR = 0x4;
const int SR = 0x5;
const int SL = 0x6;
const int ASR = 0x7;
const int RR = 0x8;
const int RL = 0x9;
const int NOT = 0xA;



const int BEQ = 0x10;
const int BNE = 0x11;
const int BGT = 0x12;
const int BGE = 0x13;
const int BLT = 0x14;
const int BLE = 0x15;


const int LD = 0x1a;

const int ST = 0x1d;





int read_mem(int& last)
{
	FILE* fp_in;  // pointer to input file pointer
	// FILE* fp_out; // output file pointer

	int word = 0;
	int i = 0;
	int address = 0;

	// errno_t is a Microsoft specific file command
	// If you are using another environment, please check file open/close/error operations

	// please update file path names for you computer
	errno_t err_in = fopen_s(&fp_in, "C:/Users/CMSC 411/Documents/sora/sora_out.bin", "r");
	// errno_t err_out = fopen_s(&fp_out, "C:/Users/A&M/out.txt", "w");

	if (err_in)
		printf("In.txt File Open Error.  File was not opened. \n");

	//if (err_out)
	//	printf("Out.txt File Open Error.  File was not opened. \n");

	else
	{
		while (!feof(fp_in)) // 
		{
			fscanf_s(fp_in, "%i %x", &address, &word);

			printf(" address = %i \t intruction word =  0x%08x \n", address, word);
			mem[address] = word;
			last = address;
			i = i + 1;

		}

	}

	// fclose(fp_out);
	if (!err_in)
		fclose(fp_in);

	printf("Done Loading File. \n");
	return 0;
}

int load_mem()
{
	int i = 100;
	for (i = 100; i < 200; i++)
		mem[i] = i;
	return 0;
}

int print_mem()
{
	int i = 0;
	for (i = 0; i < 200; i++)
		printf(" [Address = %i] = %i \n", i, mem[i]);
	return 0;
}


int op_decode(int word, int& op, int& mode)
{
	mode = word & 0x80000000;
	if (mode)
		mode = 1;  // integer instruction
	else
		mode = 0;  // register-to-register		

	op = word & 0x7C000000;

	op = (op >> 26); // shift operation code to lower 5 bits.
	// printf("mode = %i \t opcode = 0x%x \n", mode, op);

	return 0;
}

int reg_decode(int word, int& rd, int& rs, int& rt)
{
	rd = (word & 0x03E00000) >> 21;
	rs = (word & 0x001F0000) >> 16;
	rt = (word & 0x0000F800) >> 11;
	printf(" ... reg_decode() -> rd = %i \t  rs = %i \t  rt = %i \n", rd, rs, rt);
	return 0;
}

int int_decode(int word, int& rd, int& rs, int& imm16)
{
	rd = (word & 0x03E00000) >> 21;
	rs = (word & 0x001F0000) >> 16;
	imm16 = word & 0xFFFF; // assume positive two's complement number
	if (imm16 & 0x8000)    // if negative number, sign extend to 32 bits
		imm16 = imm16 | 0xFFFF0000;
	printf(" ... int_decode() -> rd = %i \t  rs = %i \t  imm16 = %i \n", rd, rs, imm16);
	return 0;
}

int ri_decode(int word, int& rd, int& rs, int& rt, int& imm16)
{
	rd = (word & 0x03E00000) >> 21;
	rs = (word & 0x001F0000) >> 16;
	rt = (word & 0x0000F800) >> 11;
	// printf(" ... reg_decode() -> rd = %i \t  rs = %i \t  rt = %i \n", rd, rs, rt);

	imm16 = word & 0xFFFF; // assume positive two's complement number
	if (imm16 & 0x8000)    // if negative number, sign extend to 32 bits
		imm16 = imm16 | 0xFFFF0000;
	// printf("... int_decode() -> rd = %i \t  rs = %i \t  imm16 = %i \n", rd, rs, imm16);
	return 0;
}


int exe_code(int& PCR)
{
	int op = 0;
	int mode = 0;
	int rd = 0, rs = 0, rt = 0;
	int imm16 = 0;
	int sign_bit = 0;
	int bit_mask = 0;
	int rotate = 0, bit0 = 0, bit31 = 0;
	int branch_field_code = 0, destination_address = 0;
	bool test = 0;
	int upper_16bits = 0, lower_16bits = 0;
	int mem_address = 0;
	int memory = 0;
	int address = 0;

	int temp = 0;
	int word = 0;

	word = mem[PCR];

	op_decode(word, op, mode);

	printf("\nExe_code() -> Address = 0x%x ... \n", PCR);
	printf(" ... mode = %i \t opcode = %i \n", mode, op);

	/*	if (mode) // if mode == 1, integer addressing mode
		{
			int_decode(word, rd, rs, imm16);
			temp = imm16;
		}

		else // mode == 0, register-to-register
		{
			reg_decode(word, rd, rs, rt);
			imm16 = 0;
			temp = reg_file[rt];
		}
	*/
	ri_decode(word, rd, rs, rt, imm16);
	if (mode)
		temp = imm16;
	else
		temp = reg_file[rt];

	switch (op)
	{


	case BEQ:
		printf(" ... Instructrion = BEQ, MODE = %i {BEQ-R | BEQ-I}, reg_file[rd] = %i reg_file[rs] = %i  \n", mode, reg_file[rd], reg_file[rs]);
		if (reg_file[rd] == reg_file[rs])  // BEQ test case
		{
			destination_address = PCR + 1 + temp;
			PCR = destination_address;
			printf("BEQ condition is TRUE.  Mode = %i  Destination Address = 0x%x \n\n", mode, PCR);
		}
		else // branch condition is false then goto next instruction
		{
			PCR = PCR + 1;
			printf("BEQ condition is FALSE.  Mode = %i  Next Instruction Address = 0x%x \n\n", mode, PCR);
		}
		break;

	case BNE:
		printf(" ... Instructrion = BNE, MODE = %i {BNE-R | BNE-I}, reg_file[rd] = %i reg_file[rs] = %i  \n", mode, reg_file[rd], reg_file[rs]);
		if (reg_file[rd] != reg_file[rs])
		{
			destination_address = PCR + 1 + temp;
			PCR = destination_address;
			printf("BNE condition is TRUE. Mode = %i, Destination Address = 0x%x \n\n", mode, PCR);
		}
		else // branch condition is false then goto next instruction
		{
			PCR = PCR + 1;
			printf("BNE condition is FALSE. Mode = %i, Next Instruction Address = 0x%x \n\n", mode, PCR);
		}
		break;

	case BGT:
		printf(" ... Instructrion = BGT, MODE = %i {BGT-R | BGT-I}, reg_file[rd] = %i reg_file[rs] = %i  \n", mode, reg_file[rd], reg_file[rs]);
		if (reg_file[rd] > reg_file[rs])
		{
			destination_address = PCR + 1 + temp;
			PCR = destination_address;
			printf("BGT condition is TRUE. Mode = %i, Destination Address = 0x%x \n\n", mode, PCR);
		}
		else // branch condition is false then goto next instruction
		{
			PCR = PCR + 1;
			printf("BGT condition is FALSE. Mode = %i, Next Instruction Address = 0x%x \n\n", mode, PCR);
		}
		break;

	case BGE:
		printf(" ... Instructrion = BGE, MODE = %i {BGE-R | BGE-I}, reg_file[rd] = %i reg_file[rs] = %i  \n", mode, reg_file[rd], reg_file[rs]);
		if (reg_file[rd] >= reg_file[rs])
		{
			destination_address = PCR + 1 + temp;
			PCR = destination_address;
			printf("BGE condition is TRUE. Mode = %i, Destination Address = 0x%x \n\n", mode, PCR);
		}
		else // branch condition is false then goto next instruction
		{
			PCR = PCR + 1;
			printf("BGE condition is FALSE. Mode = %i, Next Instruction Address = 0x%x \n\n", mode, PCR);
		}
		break;

	case BLT:
		printf(" ... Instructrion = BLT, MODE = %i {BLT-R | BLT-I}, reg_file[rd] = %i reg_file[rs] = %i  \n", mode, reg_file[rd], reg_file[rs]);
		if (reg_file[rd] < reg_file[rs])
		{
			destination_address = PCR + 1 + temp;
			PCR = destination_address;
			printf("BLT condition is TRUE. Mode = %i, Destination Address = 0x%x \n\n", mode, PCR);
		}
		else // branch condition is false then goto next instruction
		{
			PCR = PCR + 1;
			printf("BLT condition is FALSE. Mode = %i, Next Instruction Address = 0x%x \n\n", mode, PCR);
		}
		break;

	case BLE:
		printf(" ... Instructrion = BLE, MODE = %i {BLE-R | BLE-I}, reg_file[rd] = %i reg_file[rs] = %i  \n", mode, reg_file[rd], reg_file[rs]);
		if (reg_file[rd] <= reg_file[rs])
		{
			destination_address = PCR + 1 + temp;
			PCR = destination_address;
			printf("BLE condition is TRUE. Mode = %i, Destination Address = 0x%x \n\n", mode, PCR);
		}
		else // branch condition is false then goto next instruction
		{
			PCR = PCR + 1;
			printf("BLE condition is FALSE. Mode = %i, Next Instruction Address = 0x%x \n\n", mode, PCR);
		}
		break;

	case LD:
		printf(" ... Instructrion = LD MODE = %i {LD-R | LD-I} \n", mode);

		mem_address = reg_file[rs] + temp;//regfile RS + regfile RT or const16bit
		reg_file[rd] = mem[mem_address];
		printf("... mode = %i mem_address = %i, reg_file[%i] = %i \n", mode, mem_address, rd, reg_file[rd]);
		PCR = PCR + 1;
		break;

	case ST:
		printf(" ... Instructrion = ST MODE = %i {ST-R | ST-I} \n", mode);

		mem_address = reg_file[rs] + temp;//regfile RS + regfile RT or const16bit
		mem[mem_address] = reg_file[rd];
		printf("... mode = %i mem_address = %i, reg_file[%i] = %i \n", mode, mem_address, rd, reg_file[rd]);
		PCR = PCR + 1;
		break;

	default:
		printf("bad opcode \n");
		break;

	}
	// make sure R0 = 0
	reg_file[0] = 0;
	// make sure R1 = 1
	reg_file[1] = 1;
	printf(" ... PCR = %i \n", PCR);
	printf("... End Exe_code() \n");
	return 0;
}

int init_reg_file()
{
	int i = 0;
	for (i = 0; i < 32; i++)
		reg_file[i] = 0;
	reg_file[1] = 1;
	return 0;
}

int print_reg_file()
{
	int i = 0;
	for (i = 0; i < 32; i++)
		printf(" reg_file[%i] = %i \n", i, reg_file[i]);
	return 0;
}

int soma_sim()

{
	int done = 0;

	while (!done)
	{


	}

	return 0;
}

int main(void)
{
	int last = 0;
	int word = 0;
	int i = 0;

	init_reg_file();

	mem[0] = 0x00432000;
	mem[1] = 0x80640065;
	mem[2] = 0x04432000;
	mem[3] = 0x84640067;
	mem[4] = 0x08432000;
	mem[5] = 0x88640069;
	mem[6] = 0x0c432000;
	mem[7] = 0x8c64006b;
	mem[8] = 0x10432000;
	mem[9] = 0x9064006d;
	mem[10] = 0x14432000;
	mem[11] = 0x9464006f;
	mem[12] = 0x18432000;
	mem[13] = 0x98640071;
	mem[14] = 0x1c432000;
	mem[15] = 0x9c640073;
	mem[16] = 0x20432000;
	mem[17] = 0xa0640075;
	mem[18] = 0x24432000;
	mem[19] = 0xa4640077;
	mem[20] = 0x28432000;
	mem[21] = 0xa8640079;



	mem[22] = 0x2c432000;
	mem[23] = 0xac64007b;
	mem[24] = 0x30432000;
	mem[25] = 0xb064007d;
	mem[26] = 0x34432000;
	mem[27] = 0xb464007f;
	mem[28] = 0x38432000;

	mem[29] = 0xb8640081;
	mem[30] = 0x3c432000;
	mem[31] = 0xbc640083;
	mem[32] = 0x40432000;
	mem[33] = 0xc0640085;

	mem[34] = 0x44432000;
	mem[35] = 0xc4640087;
	mem[36] = 0x48432000;
	mem[37] = 0xc8640089;
	mem[38] = 0x4c432000;
	mem[39] = 0xcc64008b;
	mem[40] = 0x50432000;
	mem[41] = 0xd064008d;
	mem[42] = 0x54432000;
	mem[43] = 0xd464008f;


	mem[44] = 0x68432000;
	mem[45] = 0xe8640099;

	mem[46] = 0x74432000;
	mem[47] = 0xf464009f;



	// print_reg_file();

	//read_mem(last);
	//load_mem();
	//print_mem();
	last = 47;

	printf("last = %i \n", last);

	int PCR = 0;
	for (i = 32; i <= last; i++)
	{
		PCR = i;

		exe_code(PCR);


	}

	/*

	exe_code(PCR); // ADD
	exe_code(PCR);

	exe_code(PCR); // SUB
	exe_code(PCR);

	exe_code(PCR); // AND
	exe_code(PCR);

	exe_code(PCR); // OR
	exe_code(PCR);

	exe_code(PCR); // XOR
	exe_code(PCR);

	exe_code(PCR); // SR
	exe_code(PCR);

	exe_code(PCR); // SL
	exe_code(PCR);

	exe_code(PCR); // ASR
	exe_code(PCR);

	exe_code(PCR); // RR
	exe_code(PCR);

	exe_code(PCR); // RL
	exe_code(PCR);

	exe_code(PCR); // NOT
	exe_code(PCR);


	*/


	// print_reg_file();

}



