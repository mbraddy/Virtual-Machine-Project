//  Michael Braddy
//  mi840052
#ifndef DRIVER
#define DRIVER

#define MAX_SYMBOL_TABLE_SIZE 1000
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVEL 3

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h> // for isalpha() and isdigit()
#include <string.h> // for strcmp()

typedef struct instruction {
int op;
int r;
int l;
int m;
} instruction;

typedef enum {
nulsym = 1, identsym, numbersym, plussym, minussym,
multsym,  slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
periodsym, becomessym, beginsym, endsym, ifsym, thensym,
whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
readsym, elsesym } token_type;

typedef struct {
    token_type tokenID;
    int numberValue;
    char name[12];
} token;

//  Globals
extern int lexicalIndex;
extern token lexicalList[10000];

//  Functional Prototypes
void hw3tc();
void lexical(char *lexInput);
void pm0vm();
#endif // DRIVER
