#ifndef _MAIN_H_
# define _MAIN_H_

# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <dirent.h>
# include <sys/stat.h>

//memory size
# define MEM_SIZE 65536*16

// history of instructions
typedef struct _NODE {
	char inst[50];
	struct _NODE* next;
} NODE;

typedef struct _HISTORY {
	NODE* f;
	int count;
	NODE* r;
} HISTORY;

// opcode hash table
typedef struct _OPCODE {
	int num;
	char op[7];
	char t[5];
	struct _OPCODE* next;
} OPCODE;

typedef struct _HASH {
	int count;
	OPCODE* next;
} HASH;

// symbol table
typedef struct _SNODE {
	char label[10];
	int loc;
	struct _SNODE* next;
} SNODE;

typedef struct _SYMTAB {
	int count;
	SNODE* next;
} SYMTAB;

// external symbol table
typedef struct _ENODE {
	char symbol[10];
	int addr;
	struct _ENODE* next;
} ENODE;

typedef struct _ESTAB {
	int count;
	char section[10];
	int addr;
	int len;
	ENODE* next;
} ESTAB;

int prog;
int exeaddr;
int totlen;
int *bp;

/******************************
*	count number of token, check operator and call function
*	input	: (char*) input instruction
*	return	: information - quit or not
******************************/
int menu(char*);

/*****************************
*	print error message
*	input	: error number
*	return	: none
*****************************/
void errorMessage(int);



/* shell */

/*******************************
*	add instruction in history
*	input 	: (char*) concatenated string
*	return	: none
*******************************/
void push(char*);


/******************************
*	concatenate tokens
*	input	: (char**) instruction tokens
			  (int)    number of tokens
*	return	: (char*) concatenated string
******************************/
char* concat(char**, int);


/********************************
*	print list of instructions
*	input	: none
*	return	: none
********************************/
void help(void);


/********************************
*	print list of files in directory
*	reference - dirent.h, sys/stat.h
*	input	: 
*	return	:
********************************/
void dir(void);


/********************************
*	quit sicsim system and free memory
*	input	: none
*	return 	: none
********************************/
void quit(void);


/********************************
*	print previous instructions
*	input	: none
*	return	: none
********************************/
void history(void);


/********************************
*	print file
*	input	: filename
*	return	: none
********************************/
int typeFile(char*);



/* memory */

/*******************************
*	print memory information
*	input	: (int) start address
*			  (int) end address
*	return	: none
********************************/
void dump(int, int);


/*******************************
*	check the string form (hex or not)
*	input	: (char*) a string
*	return 	: yes or no
*******************************/
int checkHex(char*);

/*******************************
*	check the boundary for memory and call dump 
*	input	: (int)   number of token
*			  (char*) first string(hex)
*			  (char*) second string(hex)
*	return	: (int) existence of error
*******************************/
int dumpBound(int, char*, char*);


/*******************************
*	edit value of given address
*	input	: (char*) given address
*			  (char*) new value
*	return	: (int) existence of error
*******************************/
int edit(char*, char*);


/*******************************
*	edit value from start address to end address
*	input	: (char*) start address
*			  (char*) end address
*			  (char*) new value
*	return	: (int) existence of error
*******************************/
int fill(char*, char*, char*);


/*******************************
*	reset all memory to zero
*	input	: none
*	return	: none
*******************************/
void reset(void);

/*******************************
*	find and return value at given memory
*	input 	: memory address
*	return 	: value at the memory
*******************************/
int getvalue(int);



/* opcode */

/*******************************
*	make hash table
*	input	: none
*	return	: none
*******************************/
void buildHash(void);


/*******************************
*	print opcode of mnemonic
*	input	: mnemonic
*	return 	: opcode
*******************************/
int opcode(char*);


/*******************************
*	print opcode list
*	input	: none
*	return	: none
*******************************/
void opcodelist(void);



/* SIC/XE assembler */

/*******************************
*	assemble source code
*	input 	: (char*) file name
*	return	: none
*******************************/
int assemble(char*);


/******************************
*	copy file objt to obj and lstt to lst, and then remove temp files
*	input	: (int) copy and remove : 1, just remove : 0
*	return 	: none
******************************/
void cpyFile(int);


/******************************
*	pass1 part of assemble
*	input 	: (char*)filename
*	return 	: (int) existence of error
*******************************/
int pass1(char*);


/******************************
*	pass2 part of assemble
*	input	: (char*) filename
*	return	: (int) existence of error
*******************************/
int pass2(char*);


/******************************
*	check register
*	input	: (char*) string
*	return	: (int) it is register : 1, else 0
*******************************/
int isReg(char*);


/******************************
*	build Symbol table
*	input	: none
*	return 	: none
******************************/
void buildStab(void);


/******************************
*	search Opcode in table
*	input	: (char*) operator
*	return	: (char*) format of operator
******************************/
char* searchOtab(char*);


/*****************************
*	search Symbol in table
*	input	: (char*) symbol
*	return	: (char*) found : locctr of symbol, else -1
******************************/
int searchStab(char*);


/******************************
*	push Symbol in table
*	input	: (char*) symbol
*			  (int)   locctr
*	return 	: none
*******************************/
void pushStab(char*, int);


/*******************************
*	print symbol table
*	input	: none
*	return	: none
*******************************/
void symbol(void);


/*******************************
*	free Symbol table
*	input	: (int) option
*	return 	: none
*******************************/
void rmStab(int);



/* linking loader */

/******************************
*	set progaddr
*	input	: (char*) address 
*	return	: none
******************************/
void progaddr(char*);


/*****************************
*	call load1 and load2 for linking load, print load map
*	input	: (int) number of token
*			  (char*) name of file1
*			  (char*) name of file2
*			  (char*) name of file3
*	return 	: none
*****************************/
void loader(int, char*, char*, char*);


/****************************
*	pass1 part of linking load
*	input	: (int) number of current file
*			  (char*) name of current file
*	return	: none
****************************/
void load1(int,char*);


/****************************
*	pass2 part of linking load
*	input	: (int) number of current file
*			  (char*) name of current file
*	return 	: none
****************************/
void load2(int, char*);


/****************************
*	find address of external refereced label
*	input	: (int) number of current file
*			  (char*) label
*	return 	: address of label
****************************/
int findRef(int, char*);


/***************************
*	save information of referenced label on estab
*	input	: (int) number of current file
*			  (char*) label
*			  (int) address
*	return 	: none
***************************/
void pushEstab(int, char*, int);


/***************************
*	print load map
*	input 	: (int) number of current file
*	return 	: none
***************************/
void printMap(int);


/***************************
*	free all node of estab
*	input 	: none
*	return 	: none
***************************/
void freeEstab(void);


/***************************
*	return pointer of current opcode node
*	input	: (int) opcode number
*	return 	: (OPCODE*) pointer of node
***************************/
OPCODE* searchCode(int);



/* run */

/**************************
*	initialize all register
*	input 	: none
*	return 	: none
**************************/
void initReg(void);


/**************************
*	run the loaded program
*	input 	: none
*	return 	: none
**************************/
void run(void);


/*************************
*	get value (m .. m+2)
*	input 	: (int) start address m
*	return 	: value of (m .. m+2)
*************************/
int getmem(int);

/*************************
*	save value on (m .. m+2)
*	input 	: (int) start address m
*			  (int) value
*	return 	: none
*************************/
void setmem(int, int);


/************************
*	set break point
*	input	: (char*) address
*	return 	: none
************************/
void set_bp(char*);


/************************
*	clear all break point
*	input 	: none
*	return 	: none
************************/
void bpClear(void);


/************************
*	print all break point
*	input 	: none
*	return 	: none
************************/
void print_bp(void);

#endif
