//  Michael Braddy
//  mi840052

#include "driver.h"

int main(int argc, char** argv) {
    FILE* flagHandler;
    int input;

    if (argc < 2) {
        printf("\nProper Syntax is: ./a.out <Required: input.txt> <Optional: -a -v -l>\n");
        exit(EXIT_FAILURE);
    }

    lexical(argv[1]); //  input file for lexical, outputs lexOutput.txt
    hw3tc(); // opens lexOutput.txt, produces parserGenOutput.txt
    pm0vm(); // executes with parserGenOutput.txt, produces pm0vmOutput.txt

    //  Flag Handling (Echo of output files based on flag comparisons)
    if (argc > 2) {
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-a")) {
                flagHandler = fopen("parserGenOutput.txt", "r");
                    if (flagHandler == NULL) {
                        printf("\nError Accessing file during command -a, exiting\n");
                        exit(EXIT_FAILURE);
                    }
                printf("\n");
                input = fgetc(flagHandler);
                //  Echo hw3tc output
                while (input != EOF) {
                    fprintf(stdout, "%c", input);
                    input = fgetc(flagHandler);
                }
                printf("\n");
                fclose(flagHandler);
            } else if (strcmp(argv[i], "-v")) {
                flagHandler = fopen("pm0vmOutput.txt", "r");
                    if (flagHandler == NULL) {
                        printf("\nError Accessing file during command -v, exiting\n");
                        exit(EXIT_FAILURE);
                    }
                printf("\n");
                input = fgetc(flagHandler);
                //  Echo pm0vm output
                while (input != EOF) {
                    fprintf(stdout, "%c", input);
                    input = fgetc(flagHandler);
                }
                printf("\n");
                fclose(flagHandler);
            } else if (strcmp(argv[i], "-l")) {
                flagHandler = fopen("lexOutput.txt", "r");
                    if (flagHandler == NULL) {
                        printf("\nError Accessing file during command -l, exiting\n");
                        exit(EXIT_FAILURE);
                    }
                printf("\n");
                input = fgetc(flagHandler);
                //  Echo pm0vm output
                while (input != EOF) {
                    fprintf(stdout, "%c", input);
                    input = fgetc(flagHandler);
                }
                printf("\n");
                fclose(flagHandler);
            }
        }
    }   //  End of flag handling
    return 0;
}
