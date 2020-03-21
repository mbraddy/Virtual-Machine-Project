//  Michael Braddy
//  mi840052

#include "driver.h"

int lexicalIndex = 0;
token lexicalList[10000];

const char *reservedWords[] = {"const", "var", "procedure", "call", "begin", "end", "if", "then", "else", "while", "do", "read", "write", "odd"};

const char specialSymbols[14] = {'+', '-', '*', '/', '(', ')' ,'=' ,',', '.', '<', '>', ';', ':', '%'};

//  Prototypes
void handleReservedWord(int resOp, char identifier[12]);

void lexical(char *lexInput) {
    FILE* inFP;
    FILE* outFP;
    char identifier[12] = {0};  //  11 char max + terminator
    int input;
    int i;
    int lookAhead = 0;
    int errorFlag = 0;
    int handledFlag = 0;

    //  Init List
    for (int k = 0; k < 10000; k++) {
        lexicalList[k].tokenID = nulsym;
    }

    inFP = fopen (lexInput, "r");
    if (inFP == NULL){
        printf("\nError: Unable to Open Input file\n");
        exit(EXIT_FAILURE);
    }
    outFP = fopen ("lexOutput.txt", "w");
    if (outFP == NULL){
        printf("\nError: Unable to write to output file\n");
        exit(EXIT_FAILURE);
    }

    input = fgetc(inFP);
    while (input != EOF) {
        //  Ignore white space
        if (input == ' ' || input == '\n' || input == '\r' || input == '\t') {
            input = fgetc(inFP);
            lookAhead = 0;
            continue;
        }
        if (isalpha(input)) {
            i = 0;
            memset(identifier, 0, 12);
            identifier[i] = input;
            i++;
            lookAhead = 1;
            handledFlag = 0;

            while (isalpha(input = fgetc(inFP)) || isdigit(input)) {
                if (i == 11){
                    printf("Error: Identifier too long.\n");
                    //  Exits program, remove line 86 to cause program to dump problematic identifier and continue
                    exit(EXIT_FAILURE);
                    //  Dumps characters and trips errorFlag
                    while (isalpha(input = fgetc(inFP)) || isdigit(input)){
                        continue;
                    }
                    errorFlag = 1;
                }
                identifier[i] = input;
                i++;
            }
            //  Reset after error resolved and don't take the token
            if (errorFlag == 1){
                errorFlag = 0;
                continue;
            }
            //  Checking for Reserved Words
            for(int j = 0; j < 14; j++){
                if ((strcmp(identifier, reservedWords[j]) == 0)){
                    handleReservedWord(j, identifier);
                    handledFlag = 1;
                }
            }
            // Handles Identifiers
            if (handledFlag == 0){
                lexicalList[lexicalIndex].tokenID = identsym;
                strcpy(lexicalList[lexicalIndex].name, identifier);
            }
            lexicalIndex++;
        }   //  end isalpha
        else if (isdigit(input)) {
            // int num = (int)input; ASCII... this thing is what caused me to submit it late. Damn it.
            int num = input - '0';
            int temp;
            int digitLength = 1;

            lookAhead = 1;
            //  Error checking for incompatible numLength and combine the 5 digits
            while (isdigit(input = fgetc(inFP))) {
                if (digitLength == 5) {
                    printf("Error: Number too large.\n");
                    exit(EXIT_FAILURE);
                    //  burning through excess then skip digit sum
                    while (isdigit(input = fgetc(inFP))){
                        continue;
                    }
                    errorFlag = 1;
                    break;
                }
                temp = input - '0';
                num = 10 * num + temp;
                digitLength++;
            }
            //  Variable name error
            if (isalpha(input)) {
                printf("Error: Variable does not start with letter.\n");
                while (isalpha(input = fgetc(inFP)) || isdigit(input)){
                    continue;
                }
                errorFlag = 1;
            }
            //  Flag check for both error cases in this segment
            if (errorFlag == 1){
                errorFlag = 0;
                continue;
            }
            lexicalList[lexicalIndex].tokenID = numbersym;
            lexicalList[lexicalIndex].numberValue = num;
            sprintf(lexicalList[lexicalIndex].name, "%d", num);
            lexicalIndex++;
       }
       //   Symbols
        else {
            lookAhead = 0;
            for (int j = 0; j < 14; j++) {
                if (input == specialSymbols[j]) {
                    int commentFlag = 0;
                    switch(j) {
                    //  +
                    case 0:
                        lexicalList[lexicalIndex].tokenID = plussym;
                        strcpy(lexicalList[lexicalIndex].name, "+");
                        lexicalIndex++;
                        break;
                    //  -
                    case 1:
                        lexicalList[lexicalIndex].tokenID = minussym;
                        strcpy(lexicalList[lexicalIndex].name, "-");
                        lexicalIndex++;
                        break;
                    //  *
                    case 2:
                        lexicalList[lexicalIndex].tokenID = multsym;
                        strcpy(lexicalList[lexicalIndex].name, "*");
                        lexicalIndex++;
                        break;
                    //  Comments (just burns through characters until terminator symbol)
                    case 3:
                        input = fgetc(inFP);
                        lookAhead = 1;
                        if (input == '*') {
                            commentFlag = 1;
                            lookAhead == 0;
                            input = fgetc(inFP);
                            while (commentFlag == 1) {
                                if (input == '*') {
                                    input = fgetc(inFP);
                                    if (input == '/') {
                                        commentFlag = 0;
                                        continue;
                                    }
                                }
                                input = fgetc(inFP);
                            }
                        }
                        else  {
                            lexicalList[lexicalIndex].tokenID = slashsym;
                            strcpy(lexicalList[lexicalIndex].name, "/");
                            lexicalIndex++;
                        }
                        break;
                    //  (
                    case 4:
                        lexicalList[lexicalIndex].tokenID = lparentsym;
                        strcpy(lexicalList[lexicalIndex].name, "(");
                        lexicalIndex++;
                        break;
                    //  )
                    case 5:
                        lexicalList[lexicalIndex].tokenID = rparentsym;
                        strcpy(lexicalList[lexicalIndex].name, ")");
                        lexicalIndex++;
                        break;
                    //  =
                    case 6:
                        lexicalList[lexicalIndex].tokenID = eqsym;
                        strcpy(lexicalList[lexicalIndex].name, "=");
                        lexicalIndex++;
                        break;
                    //  ,
                    case 7:
                        lexicalList[lexicalIndex].tokenID = commasym;
                        strcpy(lexicalList[lexicalIndex].name, ",");
                        lexicalIndex++;
                        break;
                    //  .
                    case 8:
                        lexicalList[lexicalIndex].tokenID = periodsym;
                        strcpy(lexicalList[lexicalIndex].name, ".");
                        lexicalIndex++;
                        break;
                    //  < , <>, <=
                    case 9:
                        input = fgetc(inFP);
                        lookAhead = 1;
                        if (input == '<') {
                            strcpy(lexicalList[lexicalIndex].name, "<");
                            lexicalList[lexicalIndex].tokenID = lessym;
                        }
                        else if (input == '>') {
                            lexicalList[lexicalIndex].tokenID = neqsym;
                            strcpy(lexicalList[lexicalIndex].name, "<>");
                            lookAhead = 0;
                        }
                        else if (input == '=') {
                            lexicalList[lexicalIndex].tokenID = leqsym;
                            strcpy(lexicalList[lexicalIndex].name, "<=");
                            lookAhead = 0;
                        }
                        lexicalIndex++;
                        break;
                    //  >
                    case 10:
                        input = fgetc(inFP);
                        lookAhead = 1;
                        if (input == '=') {
                            lexicalList[lexicalIndex].tokenID = geqsym;
                            strcpy(lexicalList[lexicalIndex].name, ">=");
                            lookAhead = 0;
                        }
                        else {
                            lexicalList[lexicalIndex].tokenID = gtrsym;
                            strcpy(lexicalList[lexicalIndex].name, ">");
                        }
                        lexicalIndex++;
                        break;
                    //  ;
                    case 11:
                        lexicalList[lexicalIndex].tokenID = semicolonsym;
                        strcpy(lexicalList[lexicalIndex].name, ";");
                        lexicalIndex++;
                        break;
                    //  :=
                    case 12:
                        input = fgetc(inFP);
                        if (input == '=') {
                            lexicalList[lexicalIndex].tokenID = becomessym;
                            strcpy(lexicalList[lexicalIndex].name, ":=");
                            lexicalIndex++;
                        }
                        //  Invalid Symbol error
                        else {
                            printf("Error: Invalid Symbol\n");
                        }
                        break;
                    case 13:
                        lexicalList[lexicalIndex].tokenID = oddsym;
                        strcpy(lexicalList[lexicalIndex].name, "%");
                        lexicalIndex++;
                        break;
                    //  Invalid Symbol Error
                    default:
                        printf("Error: Invalid Symbol\n");
                        break;
                    }
                }
            }
        }
        if (lookAhead == 0) {
            input = fgetc(inFP);
        }
    }   //  end file loop
    rewind(inFP);
    input = fgetc(inFP);

    //  Output Section, Commented out is the content required of the first assignment
    /*fprintf(stdout, "Source Program:%s\n", argv[1]);
    while (input != EOF) {
        fprintf(stdout, "%c", input);
        input = fgetc(inFP);
    }
    fprintf(stdout, "\n\nLexeme Table:\n");
    fprintf(stdout, "lexeme\t\ttoken type\n");
    for (i = 0; i < lexicalIndex; i++)
        fprintf(stdout, "%s\t\t%d\n", lexicalList[i].name, lexicalList[i].tokenID);*/
    //fprintf(stdout, "\nLexeme List:\n");
    for (i = 0; i < lexicalIndex; i++){
        fprintf(outFP, "%d ", lexicalList[i].tokenID);
        if (lexicalList[i].tokenID == 2){
            fprintf(outFP, "%s ", lexicalList[i].name);
        } else if (lexicalList[i].tokenID == 3){
            fprintf(outFP, "%d ", lexicalList[i].numberValue);
        }
    }
    fprintf(outFP, "\n");
    fclose(inFP);
    fclose(outFP);
}

void handleReservedWord(int resOp, char identifier[12]) {
    switch(resOp) {
        //  const
        case 0:
            lexicalList[lexicalIndex].tokenID = constsym;
            strcpy(lexicalList[lexicalIndex].name, "const");
            break;
        //  var
        case 1:
            lexicalList[lexicalIndex].tokenID = varsym;
            strcpy(lexicalList[lexicalIndex].name, "var");
            break;
        //  procedure
        case 2:
            lexicalList[lexicalIndex].tokenID = procsym;
            strcpy(lexicalList[lexicalIndex].name, "procedure");
            break;
        //  call
        case 3:
            lexicalList[lexicalIndex].tokenID = callsym;
            strcpy(lexicalList[lexicalIndex].name, "call");
            break;
        //  begin
        case 4:
            lexicalList[lexicalIndex].tokenID = beginsym;
            strcpy(lexicalList[lexicalIndex].name, "begin");
            break;
        //  end
        case 5:
            lexicalList[lexicalIndex].tokenID = endsym;
            strcpy(lexicalList[lexicalIndex].name, "end");
            break;
        //  if
        case 6:
            lexicalList[lexicalIndex].tokenID = ifsym;
            strcpy(lexicalList[lexicalIndex].name, "if");
            break;
        //  then
        case 7:
            lexicalList[lexicalIndex].tokenID = thensym;
            strcpy(lexicalList[lexicalIndex].name, "then");
            break;
        //  else
        case 8:
            lexicalList[lexicalIndex].tokenID = elsesym;
            strcpy(lexicalList[lexicalIndex].name, "else");
            break;
        //  while
        case 9:
            lexicalList[lexicalIndex].tokenID = whilesym;
            strcpy(lexicalList[lexicalIndex].name, "while");
            break;
        //  do
        case 10:
            lexicalList[lexicalIndex].tokenID = dosym;
            strcpy(lexicalList[lexicalIndex].name, "do");
            break;
        //  read
        case 11:
            lexicalList[lexicalIndex].tokenID = readsym;
            strcpy(lexicalList[lexicalIndex].name, "read");
            break;
        //  write
        case 12:
            lexicalList[lexicalIndex].tokenID = writesym;
            strcpy(lexicalList[lexicalIndex].name, "write");
            break;
        default:
            break;
    }
}
