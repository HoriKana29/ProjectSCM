#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000
#define MAXSYMBOLS 65536

typedef struct 
{
    char name[MAXLINELENGTH];
    int address;
} Symbol;

Symbol symbloTable[MAXSYMBOLS];


int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);

int main(int argc, char *argv[])
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    int lineNumber = 0;
    int symbolCountNumber = 0;
    int addressCountNumber = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2))
    {
        if (label[0] == '\0' && opcode[0] == '\0') {
            continue;
        }

        if(label[0] != '\0') {

            int leng = strlen(label);
            if(leng > 6) { //Handle > 6 
                printf("error: invalid label %s Length more than 6 characters\n", label);
                exit(1);
            }else if(!isalpha(label[0])) { //Handle not start with alphabet
                printf("error: invalid label %s Not starting with alphabet\n", label);
                exit(1);
            }
            for(int i = 0; i < leng; i++){
                if(!isalnum(label[i])) { //Handle not alphanumeric
                    printf("error: invalid label %s is Not alphanumeric\n", label);
                    exit(1);
                }
            }

            for(int i = 0; i < symbolCountNumber; i++){
                if(strcmp(symbloTable[i].name, label) == 0){
                    printf("error: duplicate label %s\n", label);
                    exit(1);
                }
            }

            if(symbolCountNumber >= MAXSYMBOLS){
                printf("error: symbol table is full\n");
                exit(1);
            }

            strcpy(symbloTable[symbolCountNumber].name, label);
            symbloTable[symbolCountNumber].address = addressCountNumber;
            symbolCountNumber++;
        }
        
        printf("Line %d\n", lineNumber);
        printf("Label = [%s]\n", label);
        printf("Opcode = [%s]\n", opcode);
        printf("arg0 = [%s]\n", arg0);
        printf("arg1 = [%s]\n", arg1);
        printf("arg2 = [%s]\n", arg2);
        printf("---------------------------\n");

        addressCountNumber++;
        lineNumber++;

    }

    printf("Symbol Table:\n");
    for(int i = 0; i < symbolCountNumber; i++){
        printf("%s -> %d\n", symbloTable[i].name, symbloTable[i].address);
    }

    rewind(inFilePtr);

    fclose(inFilePtr);
    fclose(outFilePtr);
    

    /* here is an example for how to use readAndParse to read a line from
        inFilePtr */
    // if (! readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
    //     /* reached end of file */
    //     printf("End of file");
    //     return 0;
    // }

    // /* this is how to rewind the file ptr so that you start reading from the
    //     beginning of the file */
    // rewind(inFilePtr);

    // /* after doing a readAndParse, you may want to do the following to test the
    //     opcode */
    // if (!strcmp(opcode, "add")) {
    //     /* do whatever you need to do for opcode "add" */
    // }

    return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
    char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
	/* reached end of file */
        return(0);
    }

    /* check for line too long (by looking for a \n) */
    if (strchr(line, '\n') == NULL) {
        /* line too long */
    int nextChar = fgetc(inFilePtr);

    if (nextChar != EOF) {
        printf("error: line too long\n");
        exit(1);
    }
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
	/* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]",
        opcode, arg0, arg1, arg2);
    return(1);
}

int isNumber(char *string)
{
    /* return 1 if string is a number */
    /* return 0 ถ้า empty string หรือ string ไม่ใช่เลข(ฐาน 10)*/
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}