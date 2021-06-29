
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <Windows.h>

using namespace std;

// instructions
enum {
	LEA, IMM, JMP, CALL, JZ, JNZ, ENT, ADJ, LEV, LI, LC, SI, SC, PUSH,
	OR, XOR, AND, EQ, NE, LT, GT, LE, GE, SHL, SHR, ADD, SUB, MUL, DIV, MOD,
	OPEN, READ, CLOS, PRTF, MALC, MSET, MCMP, EXIT
};

int* pc, * bp, * sp, ax, cycle; // virtual machine registers


int token;            // current token
char* src, * oldSrc;  // pointer to source code string;
int poolSize;         // default size of text/data/stack
int line;             // line number

// segment
ULONG_PTR* text;
ULONG_PTR* oldText;
ULONG_PTR* stack;
char* sdata;

void next() {
	token = *src++;
	return;
}

void expression(int level) {
	// do nothing
}

void program() {
	next();                  // get next token
	while (token > 0) {
		printf("token is: %c\n", (char)token);
		next();
	}
}


int eval() { // do nothing yet
	int op, * tmp;
	while (1) {
		op = *pc++; // get next operation code
		if (op == IMM) { ax = *pc++; }                                     // load immediate value to ax
		else if (op == LC) { ax = *(char*)ax; }                               // load character to ax, address in ax
		else if (op == LI) { ax = *(int*)ax; }                                // load integer to ax, address in ax
		else if (op == SC) { ax = *(char*)*sp++ = ax; }                       // save character to address, value in ax, address on stack
		else if (op == SI) { *(int*)*sp++ = ax; }                             // save integer to address, value in ax, address on stack
		else if (op == PUSH) { *--sp = ax; }
		else if (op == JMP) { pc = (int*)*pc; }
		else if (op == JZ) { pc = ax ? pc + 1 : (int*)*pc; }                   // jump if ax is zero
		else if (op == JNZ) { pc = ax ? (int*)*pc : pc + 1; }                   // jump if ax is not zero
		else if (op == CALL) { *--sp = (int)(pc + 1); pc = (int*)*pc; }           // call subroutine
		//else if (op == RET)  {pc = (int *)*sp++;}                              // return from subroutine;
		else if (op == ENT) { *--sp = (int)bp; bp = sp; sp = sp - *pc++; }      // make new stack frame
		else if (op == ADJ) { sp = sp + *pc++; }                                // add esp, <size>
		else if (op == LEV) { sp = bp; bp = (int*)*sp++; pc = (int*)*sp++; }  // restore call frame and PC
		else if (op == LEA) { ax = (int)(bp + *pc++); }                         // load address for arguments.
		else if (op == OR)  ax = *sp++ | ax;
		else if (op == XOR) ax = *sp++ ^ ax;
		else if (op == AND) ax = *sp++ & ax;
		else if (op == EQ)  ax = *sp++ == ax;
		else if (op == NE)  ax = *sp++ != ax;
		else if (op == LT)  ax = *sp++ < ax;
		else if (op == LE)  ax = *sp++ <= ax;
		else if (op == GT)  ax = *sp++ > ax;
		else if (op == GE)  ax = *sp++ >= ax;
		else if (op == SHL) ax = *sp++ << ax;
		else if (op == SHR) ax = *sp++ >> ax;
		else if (op == ADD) ax = *sp++ + ax;
		else if (op == SUB) ax = *sp++ - ax;
		else if (op == MUL) ax = *sp++ * ax;
		else if (op == DIV) ax = *sp++ / ax;
		else if (op == MOD) ax = *sp++ % ax;
		else if (op == EXIT) { printf("exit(%d)", *sp); return *sp; }
		else if (op == OPEN) { ax = (int)fopen((char*)sp[1], (char*)sp[0]); }
		else if (op == CLOS) { ax = fclose((FILE*)(*sp)); }
		else if (op == READ) { ax = fread((void*)(sp[3]),sp[2], sp[1], (FILE*)(*sp)); }
		else if (op == PRTF) { tmp = sp + pc[1]; ax = printf((char*)tmp[-1], tmp[-2], tmp[-3], tmp[-4], tmp[-5], tmp[-6]); }
		else if (op == MALC) { ax = (int)malloc(*sp); }
		else if (op == MSET) { ax = (int)memset((char*)sp[2], sp[1], *sp); }
		else if (op == MCMP) { ax = memcmp((char*)sp[2], (char*)sp[1], *sp); }
		else {
			printf("unknown instruction:%d\n", op);
			return -1;
		}
	}

	return 0;
}

int main(int argc, char** argv)
{
	int i;
	FILE* fp;

	argc--;
	argv++;

	poolSize = 256 * 1024; // arbitrary size
	line = 1;

	if ((fp = fopen(*argv, "r+")) < 0) {
		printf("could not open(%s)\n", *argv);
		return -1;
	}

	if (!(src = oldSrc = (char*)malloc(poolSize))) {
		printf("could not malloc(%d) for source area\n", poolSize);
		return -1;
	}

	// read the source file
	if ((i = fread(src, 1, poolSize - 1, fp)) <= 0) {
		printf("read() returned %d\n", i);
		return -1;
	}
	src[i] = 0; // add EOF character
	fclose(fp);

	if (!(text = oldText = (ULONG_PTR*)malloc(poolSize)))
	{
		printf("could not malloc(%d) for text area\n", poolSize);
		return -1;
	}
	memset(text, 0, poolSize);

	if (!(sdata = (char*)malloc(poolSize)))
	{
		printf("could not malloc(%d) for sdata area\n", poolSize);
		return -1;
	}
	memset(sdata, 0, poolSize);

	if (!(stack = (ULONG_PTR*)malloc(poolSize)))
	{
		printf("could not malloc(%d) for stack area\n", poolSize);
		return -1;
	}
	memset(stack, 0, poolSize);

	bp = sp = (int*)((int)stack + poolSize);
	ax = 0;

	i = 0;
	text[i++] = IMM;
	text[i++] = 10;
	text[i++] = PUSH;
	text[i++] = IMM;
	text[i++] = 20;
	text[i++] = ADD;
	text[i++] = PUSH;
	text[i++] = EXIT;
	pc = (int*)text;

	program();
	return eval();
}

