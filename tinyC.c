/*
 * A C interepreter in 4 functions
 *
 * The interpreter functions would achieve the following:
 * 1. Lexical Analysis : source -> internal token stream
 * 2. Parsing : internal token stream -> AST(syntax tree)
 * 3. Code Generation: Since we are writing an interpreter(not a compiler),
 *    we would build a our own VM and assembly instructions, so we do not
 *    run on an *actual* platform/target but on the VM
 *
 *  The functions would be:
 *  1. next() -> Lexer functionality, reads the next token (ignoring spaces, tabs etc)
 *  2. program() -> entry point to parser
 *  3. expression(level) -> parse the expression at hand. 
 *  4. eval() -> code gen functionality, entry point to the VM
 */
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

/* Maintaining some global state of the system */
int token; //Current token
char *src, *old_src; //Pointer to the source code buffer
int poolsize; //Size of text/data/stack
int line; //line number

/* Memory segments for the program */
int *text;      //text segment
int *old_text;  //to dump text segment 
int *stack;     //stack
char *data;     //data segment (going to be storing string literals only)

/* VM Registers */
int *pc, *bp, *sp, ax, cycle; 

/* Instruction Set */
enum { LEA ,IMM ,JMP ,CALL ,JZ ,JNZ ,ENT ,ADJ ,LEV ,LI ,LC ,SI ,SC ,PUSH,
    OR ,XOR ,AND ,EQ ,NE ,LT ,GT ,LE ,GE ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
    OPEN ,READ ,CLOS ,PRTF ,MALC ,MSET ,MCMP ,EXIT };

/* Get the next token */
void next() {
    token = *(src++); //move to the next token on source
    return;
}

/* Parser */
void program() {
    next(); //get the next token
    while(token > 0) {
        printf("The token is: %c\n", token);
        next();
    }
}

/* Code-spitter - VM functionality */
int eval() {
    int op, *tmp;
    while(1) {
	if (op == IMM)       {ax = *pc++;}                 // load immediate value to ax
	else if (op == LC)   {ax = *(char *)ax;}           // load character to ax, address in ax
	else if (op == LI)   {ax = *(int *)ax;}            // load integer to ax, address in ax
	else if (op == SC)   {ax = *(char *)*sp++ = ax;}   // save character to address, value in ax, address on stack
	else if (op == SI)   {*(int *)*sp++ = ax;}         // save integer to address, value in ax, address on stack

    }
    return 0;
}

int main(int argc, char **argv) {
    int i, fd;

    argc--;
    argv++; //point to the input file argument

    /* Size of pool to be allocated for the source strings
     * This is a random value as of now
     */
    poolsize = 256 * 1024;
    /* Open the file and get the fd */
    if((fd = open(*argv, 0)) < 0) {
        printf("Couldn't open the input file(%s)\n", *argv);
        return -1;
    }

    /* Allocate memory for the source */
    if(!(src = old_src = malloc(poolsize))) {
        printf("Couldn't allocate(%d) for source\n", poolsize);
        return -1;
    }

    /* Read the source file */
    if((i = read(fd, src, poolsize-1)) <= 0) {
        printf("read() returned %d\n", i);
        return -1;
    }
    src[i] = 0; //End of File Character
    close(fd); //Close the file
   
    /*
     * Allocate memory for the VM
     */ 
    if (!(data = malloc(poolsize))) {
	printf("could not malloc(%d) for data area\n", poolsize);
	return -1;
    }
    if (!(stack = malloc(poolsize))) {
	printf("could not malloc(%d) for stack area\n", poolsize);
	return -1;
    }

    memset(text, 0, poolsize);
    memset(data, 0, poolsize);
    memset(stack, 0, poolsize);

    /* Register initialization */
    bp = sp = (int *)((int)stack + poolsize); //Point to the greatest address and then move down
    ax = 0;

    /* Invoke the parser
     * So we have read the entire file into a buffer
     * now we can invoke the parser to go over it and
     * parse the tokens to convert them to AST
     */ 
    program();

    /*
     * This is the code-gen functionality and is going
     * to generate code on the VM
     */
    return eval();
}
