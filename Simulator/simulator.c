// Simulator

/* instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h> // memset

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);
int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
	printf("error: usage: %s <machine-code file>\n", argv[0]);
	exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
	printf("error: can't open file %s", argv[1]);
	perror("fopen");
	exit(1);
    }

    // set ค่าตัวแปรเป็น 0 ทุกครั้งก่อนเริ่มโปรแกรม 
    memset (&state, 0 ,sizeof(state) ) ; // เข้าถึงที่อยู่ address จริงแล้วทำการกำหนดค่า ใหม่ 

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
	state.numMemory++) {
	if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
	    printf("error in reading address %d\n", state.numMemory);
	    exit(1);
	}
	printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }
   

    int count = 0  ; 
    while(1) {          // วนจนกว่าจะเจอ halt
        printState(&state) ; 
        int ins = state.mem[state.pc] ;   // fetch Ins ที่ pc ชี้อยู่ 
        int opc = (ins >> 22 ) &  0x7 ;  // decode เลื่อนบิต 22-24  (R shift ) เก็บไว้ 3 bit

        int regA = (ins >> 19  ) & 0x7; 
        int regB = (ins >> 16) & 0x7 ; 
        int dest = ins & 0x7 ; 
        int offset = convertNum(ins & 0xFFFF) ; 

        state.pc++ ; 
        count++ ; 

        if (opc == 6 ) {  // opcode halt (เจอ หยุด )
            printf("halted \n");
            printf("total of %d instructions executed\n",count);
             printf("last state :\n") ; 
            printState(&state) ; 
            return 0  ; 
        }

    }


    return(0);
}

void printState(stateType *statePtr) // print pc , mem[] , reg[0-7]
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
	for (i=0; i<statePtr->numMemory; i++) {
	    printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
	}
    printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
	    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
    printf("end state\n");
}


// 0-> 32767 (+) , 32768 -> 65535 (-)
// lw , sw , beq
int convertNum(int num) // sign extend
{
    /* convert a 16-bit number into a 32-bit integer */
    //  concept : ตัดbit ที่เหลือออกเช็คแค่ bit แรกสุดท้าย (bit 16 )ว่าเป็น 1 ไหมโดยการ AND 
    // 1 << 15      = 1000 0000 0000 0000
    if (num & (1 << 15)) {  // num >= 32768 ไหม (อยู่ในช่วงค่าลบไหม?) // shift : pointer 
    num -= (1 << 16);  // ลบด้วย 65536(จะได้ค่าติดลบ)
    }
    return(num); // return ค่าติดลบนั้น  // ถ้าค่าบวกจะไม่เข้าcondตั้งแต่แรกอยู่แล้ว return num ได้ค่าบวกเหมือนเดิม
}
