//  Michael Braddy
//  mi840052

#include "driver.h"

typedef struct {
	int kind; 		// const = 1, var = 2, proc = 3
	char name[10];	// name up to 11 chars
	int val; 		// number (ASCII value)
	int level; 		// L level
	int addr; 		// M address
	int mark;		// to indicate that code has been generated already for a block.
} symbol;

//  Prototypes
void error(int errorCode);
void block(int lexLevel);
token getNextToken();
void statement(int lexLevel);
int findIdent(char* name);
void condition(int lexicalLevel);
void factor(int lexLevel);
void term(int lexLevel);
void expression(int lexLevel);
void emit(int op, int l, int m);
void enter(int type, int lexicalLevel);
void varDeclaration(int lexicalLevel);
void program();
void constDeclaration(int lexicalLevel);

//  Global Variables
int cx = 0, symbolTableIndex = 0, tokenIndex = 0, addrx = 4, cxStorage;
int regPointer = 0;
int procAddress = 0;
token currentToken;
instruction code[MAX_CODE_LENGTH];
symbol table[MAX_SYMBOL_TABLE_SIZE] = { 0 };

void hw3tc() {
    FILE* outFP;
    int i = 0;

    outFP = fopen ("parserGenOutput.txt", "w");
    if (outFP == NULL){
        printf("\nError: Unable to write to output file in hw3tc\n");
        exit(EXIT_FAILURE);
    }
    //  Input is a Lexical list, Output Needs to be:
    //  "No Errors, program is syntactically correct" | Error(Grammer_Violation#) + Output Message
    //  And formatted output for pm0vm (2d 2d 2d 2d\n)

    program();
    //  If it makes it this far, no errors
    printf("\nNo Errors, Program is syntactically correct\n");

    //  Outputs code to text file
    for (i = 0; i < cx; i++) {
        fprintf(outFP, "%2d %2d %2d %2d\n", code[i].op, code[i].r, code[i].l, code[i].m);
    }

    fclose(outFP);
}

void program() {
    currentToken = getNextToken();
    block(0);
    if (currentToken.tokenID != periodsym) {
        error(9);
    }
}

void block(int lexLevel) {
    int tempTableIndex = symbolTableIndex;

    //  Max lexi level exceeded
    if (lexLevel > MAX_LEXI_LEVEL) {
        error(0);
    }

    table[symbolTableIndex].addr = cx;
    //  jump to Code corresponding to main
    emit(7, 0, 0);
    //  Goes through all Const and var and stores in symbol table
    do {
        //  Cycle through multiple declared constants, check for end of line ;
        if (currentToken.tokenID == constsym) {
            currentToken = getNextToken();
            do {
                constDeclaration(lexLevel);
                while(currentToken.tokenID == commasym) {
                    currentToken = getNextToken();
                    constDeclaration(lexLevel);
                }
                if(currentToken.tokenID == semicolonsym) {
                    currentToken = getNextToken();
                }
                else {
                    //  Missing Comma/semicolon
                    error(5);
                }
            } while (currentToken.tokenID == identsym);
        }
        //  Cycles multiple var declarations, checks end of line ;
        if (currentToken.tokenID == varsym) {
            currentToken = getNextToken();
            do {
                varDeclaration(lexLevel);
                while (currentToken.tokenID == commasym) {
                    currentToken = getNextToken();
                    varDeclaration(lexLevel);
                }
                if(currentToken.tokenID == semicolonsym) {
                    currentToken = getNextToken();
                }
                else {
                    //  Missing comma/semicolon
                    error(5);
                }
            } while(currentToken.tokenID == identsym);
        }
        if (currentToken.tokenID == procsym) {
            currentToken = getNextToken();
            if (currentToken.tokenID == identsym) {
                procAddress = cx;
                enter(3, lexLevel);
                currentToken = getNextToken();
            }
            else {
                //  Proc needs ident
                error(4);
            }
            if(currentToken.tokenID == semicolonsym) {
                currentToken = getNextToken();
            }
            else {
                //  Semicolon expected after statement
                error(5);
            }

            block(lexLevel + 1);
            code[procAddress].m = cx + 1;

            if(currentToken.tokenID == semicolonsym) {
                currentToken = getNextToken();
            }
            else {
                    error(5);
            }
        }
    }
    while((currentToken.tokenID == varsym) || (currentToken.tokenID == constsym || (currentToken.tokenID == procsym)));
    //  linking
    cxStorage = cx;
    // Code addr
    code[table[tempTableIndex].addr].m = cx;
    table[tempTableIndex].addr = cx;
    // INC instruction
    emit(6, 0, addrx);
    //  process the statement
    statement(lexLevel);
    // return instruction
    emit(2, 0, 0);
    // level -- on exit
}

token getNextToken() {
    token tempToken;
    if (tokenIndex == lexicalIndex) {
        tempToken = lexicalList[lexicalIndex - 1];
        return tempToken;
    }
    tempToken = lexicalList[tokenIndex];
    tokenIndex++;
    return tempToken;
}

void statement(int lexLevel) {
    int ctemp;
    int ctemp2;
    //  Index of Identifier in table
    int tableLocation;
    //  Identifier
    if (currentToken.tokenID == identsym) {
        tableLocation = findIdent(currentToken.name);
        //  Identifier name not found in table
        if (tableLocation == -1) {
            // identifier not in symbol table
            error(11);
        }
        else if (table[tableLocation].kind != 2) {
        //  Assignment to non var
        error(12);
        }
        currentToken = getNextToken();
        if (currentToken.tokenID == becomessym) {
            currentToken = getNextToken();
        }
        else {
            //  Assignment Op needed
            error(13);
        }
        expression(lexLevel);
        if (tableLocation != -1) {
            // STO levels down Item at addr
            emit(4, (lexLevel - table[tableLocation].level), table[tableLocation].addr);
        }
    }
    //  Begin -> end
    else if (currentToken.tokenID == beginsym) {
        currentToken = getNextToken();
        statement(lexLevel);
        while (currentToken.tokenID == semicolonsym) {
            currentToken = getNextToken();
            statement(lexLevel);
        }
        if (currentToken.tokenID != endsym) {
            if (currentToken.tokenID == elsesym) {
                code[ctemp].m = cx + 1;
                ctemp = cx;
                //emit(7, 0, 0);
                currentToken = getNextToken();
                statement(lexLevel);
            }
            else {
                error(17);
            }
        }
        currentToken = getNextToken();
    }
    //  if then statement
    else if (currentToken.tokenID == ifsym) {
        currentToken = getNextToken();
        condition(lexLevel);
        if (currentToken.tokenID != thensym) {
            error(16);
        } else {
            currentToken = getNextToken();
        }
        ctemp = cx;
        emit(8, 0, 0);
        statement(lexLevel);
        //  Jump past else
        code[ctemp].m = cx;
    // while <condition> do <statement>
    } else if (currentToken.tokenID == whilesym) {
        ctemp = cx;
        currentToken = getNextToken();
        condition(lexLevel);
        ctemp2 = cx;
        emit(8, 0, 0);
        if (currentToken.tokenID != dosym)
            error(18);
        else {
            currentToken = getNextToken();
        }
        statement(lexLevel);
        emit(7, 0, ctemp);
        code[ctemp2].m = cx;
    }
    //  write
    else if (currentToken.tokenID == writesym) {
        currentToken = getNextToken();
        expression(lexLevel);
        //  SIO 1
        emit(9, 0, 1);
    }
    //  read/store NEED WORK
    else if (currentToken.tokenID == readsym) {
        currentToken = getNextToken();
        emit(9, 0, 2);
        tableLocation = findIdent(currentToken.name);
        if (tableLocation == -1)
            error(11);
        else if (table[tableLocation].kind != 2) {
            error(12);
        }
        emit(4, lexLevel - table[tableLocation].level, table[tableLocation].addr);
        currentToken = getNextToken();
    }
    //  call
    else if (currentToken.tokenID == callsym) {
         currentToken = getNextToken();
         if (currentToken.tokenID != identsym) {
            //  call then identifier
            error(14);
         }
         else {
            tableLocation = findIdent(currentToken.name);

            if (tableLocation == -1)
                error(11);
            //  proc call
            else if (table[tableLocation].kind == 3)
                emit(5, lexLevel - table[tableLocation].level, table[tableLocation].addr);
            else
                error(15);

            currentToken = getNextToken();
         }
    }
}

int findIdent(char* name) {
    for (int k = 0; k < symbolTableIndex; k++){
        //  loop through symbol table, if find name of identifier return location index else 0
        if (strcmp(name, table[k].name) == 0) {
            return k;
        }
    }
    return -1;
}

void condition(int lexicalLevel) {
    if (currentToken.tokenID == oddsym) {
        currentToken = getNextToken();
        expression(lexicalLevel);
        //  Odd Doesn't use L or M
        emit(15, 0, 0);
    } else {
        expression(lexicalLevel);
        if (currentToken.tokenID != eqsym && currentToken.tokenID != neqsym && currentToken.tokenID != lessym &&
            currentToken.tokenID != leqsym && currentToken.tokenID != gtrsym && currentToken.tokenID != geqsym) {
                //  Need operator
                error(20);
        }
        else {
            switch (currentToken.tokenID) {
                //  .r = .l op .m
                case 9:
                    currentToken = getNextToken();
                    //  EQL
                    emit(17, regPointer - 1, regPointer);
                    break;
                case 10:
                    currentToken = getNextToken();
                    //  NEQ
                    emit(18, regPointer - 1, regPointer);
                    break;
                case 11:
                    currentToken = getNextToken();
                    //  LSS
                    emit(19, regPointer - 1, regPointer);
                    break;
                case 12:
                    currentToken = getNextToken();
                    //  LEQ
                    emit(20, regPointer - 1, regPointer);
                    break;
                case 13:
                    currentToken = getNextToken();
                    //  GTR
                    emit(21, regPointer - 1, regPointer);
                    break;
                case 14:
                    currentToken = getNextToken();
                    //  GEQ
                    emit(22, regPointer - 1, regPointer);
                    break;
                default:
                    break;
            }
        expression(lexicalLevel);
        }
    }
}

void factor(int lexLevel) {
    int tableLocation;
    while ((currentToken.tokenID == identsym) || (currentToken.tokenID == numbersym) || (currentToken.tokenID == lparentsym)) {
        if (currentToken.tokenID == identsym) {
            tableLocation = findIdent(currentToken.name);
            if (tableLocation == -1) {
                error(11); // identifier not in symbol table
            }
            else {
                //  Constant
                if (table[tableLocation].kind == 1) {
                    //  LIT val to .r
                    emit(1, 0, table[tableLocation].val);
                }
                //  Var
                else if (table[tableLocation].kind == 2) {
                    //  LOD
                    emit(3, table[tableLocation].level, table[tableLocation].addr);
                }
                //  Expression used on proc identifier
                else {
                    error(21);
                }
            }
            currentToken = getNextToken();
        }
        //  Number
        else if (currentToken.tokenID == numbersym) {
            emit(1, 0, currentToken.numberValue);
            currentToken = getNextToken();

        }
        //  Parentheses
        else if (currentToken.tokenID == lparentsym) {
            currentToken = getNextToken();
            //  Handle expression in parentheses first
            expression(lexLevel);
            if (currentToken.tokenID != rparentsym) {
                //  rparent missing
                error(22);
            }
            else {
                currentToken = getNextToken();
            }
        }
    }
}

void term(int lexLevel) {
    int multOperator;
    factor(lexLevel);
    while (currentToken.tokenID == multsym || currentToken.tokenID == slashsym) {
        multOperator = currentToken.tokenID;
        currentToken = getNextToken();
        factor(lexLevel);
        if (multOperator == multsym) {
            emit(13, regPointer - 1, regPointer); //   multiplication
        } else {
            emit(14, regPointer - 1, regPointer); //   division
        }
    }
}

void expression(int lexLevel) {
    int addOperator;
    if (currentToken.tokenID == plussym || currentToken.tokenID == minussym) {
        addOperator = currentToken.tokenID;
        currentToken = getNextToken();
        term(lexLevel);
        if (addOperator == minussym) {
            emit(10, regPointer, 0); // negate
        }
    } else {
        term(lexLevel);
        while (currentToken.tokenID == plussym || currentToken.tokenID == minussym) {
            addOperator = currentToken.tokenID;
            currentToken = getNextToken();
            term(lexLevel);
            if (addOperator == plussym) {
                emit(11, regPointer - 1, regPointer);  //  addition
            } else {
                emit(12, regPointer - 1, regPointer);  //  subtraction
            }
        }
    }
}

void emit(int op, int l, int m) {
    if (cx > MAX_CODE_LENGTH)
        error(25);
    else {
        if (op == 3 || op == 1) {
            code[cx].r = regPointer;
            regPointer++;
        }
        else if (op == 10) {
            //  Neg stores result in same reg so leaves regpointer alone
            code[cx].r = regPointer;
        }
        else if (op == 4 || op == 11 || op == 12 || op == 13 ||
                 op == 14 || op == 15 || op == 16 ||op == 17 || op == 18 ||
                 op == 19 || op == 20 || op == 21 || op == 22) {
            regPointer--;
            code[cx].r = regPointer;
        }
        code[cx].op = op;
        code[cx].l = l;
        code[cx].m = m;
        cx++;
    }
}

void enter(int type, int lexicalLevel) {
    strcpy(table[symbolTableIndex].name, currentToken.name);   //  name
    table[symbolTableIndex].kind = type;    //  kind
    //  Constant
    if (type == 1) {
        table[symbolTableIndex].val = currentToken.numberValue;   //   value
    //  Var
    } else if (type == 2) {
        table[symbolTableIndex].level = lexicalLevel;   //  l
        addrx++;
        table[symbolTableIndex].addr = addrx;  //  M work
        currentToken = getNextToken();
    //  proc
    } else {
        table[symbolTableIndex].level = lexicalLevel;
    }
    symbolTableIndex++;
}

void varDeclaration(int lexicalLevel) {
    if (currentToken.tokenID == identsym) {
        enter(2, lexicalLevel);
    } else {
        //  Const/Var/Proc must be followed by identifier
        error(4);
    }
}

void constDeclaration(int lexicalLevel) {
    if (currentToken.tokenID == identsym) {
        currentToken = getNextToken();
        if (currentToken.tokenID == identsym || currentToken.tokenID == numbersym) {
            error(3);
        }
        if((currentToken.tokenID == eqsym) || (currentToken.tokenID == becomessym)) {
            if (currentToken.tokenID == becomessym) {
                //  = instead of :=
                error(1);
            }
            currentToken = getNextToken();
            if (currentToken.tokenID == numbersym) {
                enter(1, lexicalLevel);
            } else if (currentToken.tokenID == varsym) {
                error(2);
            }
        }
    }
}

void error(int errorCode) {
    switch (errorCode) {
        case 0:
            printf("Error: Lexical Level exceeded MAX_LEXI_LEVEL.\n");
            break;
        case 1:
            printf("Error 1: Use = instead of :=.\n");
            break;
        case 2:
            printf("Error 2: = must be followed by a number.\n");
            break;
        case 3:
            printf("Error 3: Identifier must be followed by =.\n");
            break;
        case 4:
            printf("Error 4: Const, var, procedure must be followed by identifier.\n");
            break;
        case 5:
            printf("Error 5: Semicolon or comma missing.\n");
            break;
        case 6:
            printf("Error 6: Incorrect symbol after procedure declaration.\n");
            break;
        case 7:
            printf("Error 7: Statement expected\n");
            break;
        case 8:
            printf("Error 8: Incorrect symbol after statement part in block.\n");
            break;
        case 9:
            printf("Error 9: Period expected.\n");
            break;
        case 10:
            printf("Error 10: Semicolon between statements missing.\n");
            break;
        case 11:
            printf("Error 11: Undeclared identifier.\n");
            break;
        case 12:
            printf("Error 12: Assignment to constant or procedure is not allowed.\n");
            break;
        case 13:
            printf("Error 13: Assignment operator expected.\n");
            break;
        case 14:
            printf("Error 14: Call must be followed by an identifier\n");
            break;
        case 15:
            printf("Error 15: Call of a constant or variable is meaningless.\n");
            break;
        case 16:
            printf("Error 16: Then expected\n");
            break;
        case 17:
            printf("Error 17: Semicolon or } expected\n");
            break;
        case 18:
            printf("Error 18: Do expected\n");
            break;
        case 19:
            printf("Error 19: Incorrect symbol following statement.\n");
            break;
        case 20:
            printf("Error 20: Relational operator expected.\n");
            break;
        case 21:
            printf("Error 21: Expression must not contain a procedure identifier.\n");
            break;
        case 22:
            printf("Error 22: Right parenthesis missing.\n");
            break;
        case 23:
            printf("Error 23: The preceding factor cannot begin with this symbol.\n");
            break;
        case 24:
            printf("Error 24: An expression cannot begin with this symbol.\n");
            break;
            //  Identifier too long
            //  Invalid Symbol
        default:
            break;
    }
    //  Exit after the Error
    exit(EXIT_FAILURE);
}
