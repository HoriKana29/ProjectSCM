#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>

#define MAXLINELENGTH 1000
#define MAXSYMBOLS 65536

typedef struct 
{
    char name[MAXLINELENGTH];
    int address;
} Symbol;

Symbol symbolTable[MAXSYMBOLS];


int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int findLabelAddress(const char *, const Symbol[], int);
int convertNumber(const char *, long *);
int convertRegister(const char *);
int32_t resolveFill(const char *,const Symbol table[], int count);
int convertOffset(const char *, const char *, int, const Symbol[], int);

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
        lineNumber++;
        if (label[0] == '\0' && opcode[0] == '\0') {
            continue;
        }

        if(addressCountNumber >= 65536){
            printf("errpr: program exceeds memory size\n");
            exit(1);
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
                if(strcmp(symbolTable[i].name, label) == 0){
                    printf("error: duplicate label %s\n", label);
                    exit(1);
                }
            }

            if(symbolCountNumber >= MAXSYMBOLS){
                printf("error: symbol table is full\n");
                exit(1);
            }

            strcpy(symbolTable[symbolCountNumber].name, label);
            symbolTable[symbolCountNumber].address = addressCountNumber;
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

    }

    printf("Symbol Table:\n");
    for(int i = 0; i < symbolCountNumber; i++){
        printf("%s -> %d\n", symbolTable[i].name, symbolTable[i].address);
    }

    rewind(inFilePtr);

    //pass2
    addressCountNumber = 0;
    lineNumber = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2))
    {
        lineNumber++;
        if (label[0] == '\0' && opcode[0] == '\0') {
            continue;
        }

        uint32_t machineCode = 0;

        if(strcmp(opcode, ".fill") == 0){
            int32_t fillValue = resolveFill(arg0, symbolTable, symbolCountNumber);
            fprintf(outFilePtr, "%d\n", fillValue);
        }else{
            if(strcmp(opcode, "add") == 0 || strcmp(opcode, "nand") == 0){
                uint32_t opcodeNumber;
                if(strcmp(opcode, "add") == 0){
                    opcodeNumber = 0;
                }else{
                    opcodeNumber = 1;
                }
                uint32_t regA = convertRegister(arg0);
                uint32_t regB = convertRegister(arg1);
                uint32_t destReg = convertRegister(arg2);

                machineCode = (opcodeNumber << 22) | (regA << 19) | (regB << 16) | destReg; // 6 -> 110 << 22
            }else if(strcmp(opcode, "lw") == 0 || strcmp(opcode, "sw") == 0 || strcmp(opcode, "beq") == 0){
                uint32_t opcodeNumber;
                if (strcmp(opcode, "lw") == 0) {
                    opcodeNumber = 2;
                } else if (strcmp(opcode, "sw") == 0) {
                    opcodeNumber = 3;
                } else {
                    opcodeNumber = 4;
                }

                uint32_t regA = convertRegister(arg0);
                uint32_t regB = convertRegister(arg1);
                
                uint16_t offset = (convertOffset(arg2, opcode, addressCountNumber, symbolTable, symbolCountNumber));
                machineCode = (opcodeNumber << 22) | (regA << 19) | (regB << 16) | (uint32_t)offset;
            }else if(strcmp(opcode, "jalr") == 0){
                uint32_t regA = convertRegister(arg0);
                uint32_t regB = convertRegister(arg1);

                machineCode = (5 << 22) | (regA << 19) | (regB << 16);
            }else if(strcmp(opcode, "halt") == 0){
                machineCode = 6 << 22;
            }else if(strcmp(opcode, "noop") == 0){
                machineCode = 7 << 22;
            }else{
                printf("error: unknown opcode [%s] at line %d\n", opcode, lineNumber);
                exit(1);
            }
            fprintf(outFilePtr, "%u\n", machineCode);
        }
        addressCountNumber++;
    }
    

    fclose(inFilePtr);
    fclose(outFilePtr);

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

int isNumber(char *string) // 0 used
{
    /* return 1 if string is a number */
    /* return 0 ถ้า empty string หรือ string ไม่ใช่เลข(ฐาน 10)*/
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}

int findLabelAddress(const char *name, const Symbol table[], int count)
{
    for (int i = 0; i < count; i++) {
        if (strcmp(table[i].name, name) == 0) {
            return table[i].address;
        }
    }

    printf("error: undefined label %s\n", name);
    exit(1);
}

int convertNumber(const char *text, long *result)
{
    char *pointingIndex;

    errno = 0;
    long value = strtol(text, &pointingIndex, 10);

    if (pointingIndex == text || *pointingIndex != '\0') {
        return 0;
    }

    if (errno == ERANGE) {
        printf("error: number out of range: %s\n", text);
        exit(1);
    }

    *result = value;
    return 1;
}

int convertRegister(const char *text)
{
    long number;

    if (!convertNumber(text, &number)) {
        printf("error: invalid register: [%s]\n", text);
        exit(1);
    }

    if (number < 0 || number > 7) {
        printf("error: register out of range: %s\n", text);
        exit(1);
    }

    return (int)number;
}

int convertOffset(const char *text, const char *opcode, int currentAddress, const Symbol table[], int count) 
{ 
    long offset; 
 
    if (text[0] == '\0') {
        printf("error: missing offset\n"); 
        exit(1); 
    } 
 
    if (!convertNumber(text, &offset)) { 
        int targetAddress = findLabelAddress(text, table, count);
 
        if (strcmp(opcode, "beq") == 0) {
            offset = (long)targetAddress - ((long)currentAddress + (long)1); 
        } else { 
            offset = targetAddress; 
        } 
    } 
 
    if (offset < -32768 || offset > 32767) { 
        printf("error: offset out of range: %ld\n", offset); 
        exit(1); 
    } 
 
    return (int)offset; 
}

int32_t resolveFill(const char *text, const Symbol table[], int count){
    long value;

    if(text[0] == '\0'){
        printf("error: missing .fill value\n");
        exit(1);
    }

    if(!convertNumber(text, &value)){
        value = findLabelAddress(text, table, count);
    }

    if(value < INT32_MIN || value > INT32_MAX){
        printf("error: .fill value out of range: %s\n", text);
        exit(1);
    }

    return value;
}