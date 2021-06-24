
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <Windows.h>

using namespace std;

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



    program();
    return eval();
}

