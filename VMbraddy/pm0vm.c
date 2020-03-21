//  Michael Braddy
//  mi840052

#include "driver.h"

//  Prototypes
void execute(instruction *instructionList, int *stack, int *reg, int *sp, int *bp, int instructionCount);
void printStack(int sp, int bp, int* stack, int lex, FILE** outputFP);
int findBase(int l, int bp, int *stack);

void pm0vm() {
    int stack [MAX_STACK_HEIGHT] = { 0 };
    int reg[8] = { 0 };
    int sp = 0;
    int bp = 1;
    int i = 0;
    instruction instructionList[MAX_CODE_LENGTH];
    FILE *inputFP;

    if ((inputFP = fopen("parserGenOutput.txt", "r")) == NULL){
        printf("\nUnable to open parserGenOutput.txt in pm0vm.c\n");
        exit(EXIT_FAILURE);
    } else {
        //  Fetch and parse the instructions
        while (fscanf(inputFP, "%d %d %d %d", &instructionList[i].op, &instructionList[i].r,
                      &instructionList[i].l, &instructionList[i].m) != EOF ){
            i++;
        }
        execute(instructionList, stack, reg, &sp, &bp, i);
    }
    fclose(inputFP);
}

void execute(instruction *instructionList, int *stack, int *reg, int *sp, int *bp, int instructionCount) {
    int pc = 0;
    char opStr[4] = "ERR";
    FILE* outputFP;

    outputFP = fopen("pm0vmOutput.txt", "w");
    if (outputFP == NULL) {
        printf("\nError writing to output file pm0vmOutput.txt in pm0vm.c\n");
        exit(EXIT_FAILURE);
    }

    printf("OP   Rg Lx Vl[ PC BP SP ]\n");
        for (int j = 0; j < instructionCount; j++){
                pc++;
            switch(instructionList[j].op) {
            case 1: //  LIT Check
                strcpy(opStr, "LIT");
                reg[(instructionList[j].r)] = instructionList[j].m;
                break;
            case 2: //  RTN Check
                strcpy(opStr, "RTN");
                *sp = *bp - 1;
                pc = stack[*sp + 4];
                j = pc + 1;
                *bp = stack[*bp + 2];
                break;
            case 3: //  Lod Check
                strcpy(opStr, "LOD");
                reg[instructionList[j].r] = stack[findBase(instructionList[j].l, *bp, stack) + instructionList[j].m - 2];
                break;
            case 4: //  Sto Check
                strcpy(opStr, "STO");
                stack[findBase(instructionList[j].l, *bp, stack) + instructionList[j].m - 2] = reg[instructionList[j].r];
                break;
            case 5: //  Cal Check
                strcpy(opStr, "CAL");
                stack[*sp + 1] = 0; //  Space to return value
                stack[*sp + 2] = findBase(instructionList[j].l, *bp, stack);   //  Static Link
                stack[*sp + 3] = *bp;   //  Dynamic Link
                stack[*sp + 4] = pc;   //  Return Address
                *bp = *sp + 1;
                *sp = *sp + 4;
                pc = instructionList[j].m;
                break;
            case 6: //  Inc Check
                strcpy(opStr, "INC");
                *sp = *sp + instructionList[j].m;
                break;
            case 7: //  Jmp Check
                strcpy(opStr, "JMP");
                pc = instructionList[j].m;
                j = instructionList[j].m - 1;
                break;
            case 8: //  JPC Check
                strcpy(opStr, "JPC");
                if (reg[instructionList[j].r] == 0) {
                    pc = instructionList[j].m;
                    j = instructionList[j].m - 1;
                }
                break;
            case 9: //  SI0 1, 2, 3
                strcpy(opStr, "SIO");
                switch (instructionList[j].m){
                case 1: //  Write a register to the screen
                    printf("%d\n", reg[instructionList[j].r]);
                    break;
                case 2: //  Read input from user and store in register
                    scanf("%d", &reg[instructionList[j].r]);
                    break;
                case 3: //  End program
                    exit(EXIT_SUCCESS);
                    break;
                default:
                    printf("Error: Invalid L passed with SIO command");
                    break;
                }
                break;
            case 10:    //  NEG
                strcpy(opStr, "NEG");
                reg[instructionList[j].r] = -reg[instructionList[j].l];
                break;
            case 11:    //  ADD
                strcpy(opStr, "ADD");
                reg[instructionList[j].r] = reg[instructionList[j].l] + reg[instructionList[j].m];
                break;
            case 12:    //  Sub
                strcpy(opStr, "SUB");
                reg[instructionList[j].r] = reg[instructionList[j].l] - reg[instructionList[j].m];
                break;
            case 13:    //  MUL
                strcpy(opStr, "MUL");
                reg[instructionList[j].r] = reg[instructionList[j].l] * reg[instructionList[j].m];
                break;
            case 14:    //  DIV
                strcpy(opStr, "DIV");
                reg[instructionList[j].r] = reg[instructionList[j].l] / reg[instructionList[j].m];
                break;
            case 15:    //  ODD
                strcpy(opStr, "ODD");
                reg[instructionList[j].r] = reg[instructionList[j].r] % 2;
                break;
            case 16:    //  Mod
                strcpy(opStr, "MOD");
                reg[instructionList[j].r] = (reg[instructionList[j].l] % reg[instructionList[j].m]);
                break;
            case 17:    //  EQL
                strcpy(opStr, "EQL");
                reg[instructionList[j].r] = (reg[instructionList[j].l] == reg[instructionList[j].m]);
                break;
            case 18:    //  NEQ
                strcpy(opStr, "NEQ");
                reg[instructionList[j].r] = (reg[instructionList[j].l] != reg[instructionList[j].m]);
            case 19:    //  LSS
                strcpy(opStr, "LSS");
                reg[instructionList[j].r] = (reg[instructionList[j].l] < reg[instructionList[j].m]);
                break;
            case 20:    //  LEQ
                strcpy(opStr, "LEQ");
                reg[instructionList[j].r] = (reg[instructionList[j].l] <= reg[instructionList[j].m]);
                break;
            case 21:    //  GTR
                strcpy(opStr, "GTR");
                reg[instructionList[j].r] = (reg[instructionList[j].l] > reg[instructionList[j].m]);
                break;
            case 22:    //  GEQ
                strcpy(opStr, "GEQ");
                reg[instructionList[j].r] = (reg[instructionList[j].l] >= reg[instructionList[j].m]);
                break;
            default:
                printf("\nReading invalid OP\n");
                break;
            }   //  Switch end

            fprintf(outputFP, "%s\t%d %d %d [ %d %d %d ] \t", opStr, instructionList[j].r, instructionList[j].l,
                   instructionList[j].m, pc, *bp, *sp);
            printStack(*sp, *bp, stack, instructionList[j].l, &outputFP);
            fprintf(outputFP, "\n\tRegisters: [%3d%3d%3d%3d%3d%3d%3d%3d]\n", reg[0], reg[1], reg[2],
                    reg[3], reg[4], reg[5], reg[6], reg[7]);

        }   //  for end
    }

//  Finding base L levels down
int findBase(int l, int base, int* stack) {
    int tempBase = base;
    while (l > 0) {
        tempBase = stack[tempBase + l];
        l--;
    }
    return tempBase;
}

void printStack(int sp, int bp, int* stack, int lex, FILE** outputFP){
    int i = 0;
    if (bp == 0) {
        return;
    }
    else if (bp == 1) {
        for (i = 0; i < sp; i++)
            fprintf(*outputFP, "%3d", stack[i]);
        return;
    }
    else {
        printStack(bp - 1, stack[bp + 2], stack, lex, outputFP);
        //  Call formatting
        if (sp < bp) {
            fprintf(*outputFP, "|");
            //  New stack parts created by call
            for (i = 0; i < 4; i++)
                fprintf(*outputFP, "%3d", stack[bp+i]);
        } else {
            fprintf(*outputFP, "|");
            for (i = bp; i <= sp; i++)
                fprintf(*outputFP, "%3d", stack[i]);
        }
        return;
    }
}
