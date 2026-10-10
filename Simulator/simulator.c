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
        // 0x7 : 0000 0000 0000 0000 0000 0000 0000 0111
         
        int regA = (ins >> 19  ) & 0x7;      
        int regB = (ins >> 16) & 0x7 ; 
        int dest = ins & 0x7 ; 
        int offset = convertNum(ins & 0xFFFF) ; 

        state.pc++ ; 
        count++ ; 

        switch(opc) { // 101
            // add
            case 0b000: state.reg[dest] = state.reg[regA] + state.reg[regB] ; 
            break ; 
            //nand 
            case 0b001: state.reg[dest] = ~(state.reg[regA] & state.reg[regB]) ; 
            break ; 
            //lw 
            case 0b010: state.reg[regB] = state.mem[state.reg[regA]  + offset] ; 
            break;
            //sw
            case 0b011: state.mem[state.reg[regA] + offset] = state.reg[regB];
            break ; 
            // beq 
            case 0b100: if (state.reg[regA] == state.reg[regB]) {
                state.pc += offset ; 
            }
             break;
            // jalr                                                   x1= 0 
            case 0b101: { // jalr :  regA(addr dest)  regB (pc+1 )   jalr  exit ,  exit 
                                                                // pc+1  add x1,x3 ,x8      x3 = 5 , x8 = 2
                                                                //pc _  exit: x0,x0,x5      x1= 0   result : x1=0 
                state.reg[regB] = state.pc; 
                state.pc = state.reg[regA]; 
               
            }
            break; 
            // halt 
            case 0b110:    
            printf("machine halted\n");
            printf("total of %d instructions executed\n",count);
             printf("final state of machine:\n") ; 
            printState(&state) ; 
            return 0  ; 
            // noop 
            case 0b111: 
                break; 
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
    if (num & (1 << 15)) { // เลื่อนไปทางซ้าย 15 บิตเพื่อตรวจสอบบิตที่ 15 (บิตเครื่องหมาย)
    num -= (1 << 16); // เอา num มา AND กับค่านั้น ผลที่ได้จะเหลือแค่าบิตที่ 0-15 ของ num และลบด้วย 2^16 เพื่อให้ได้ค่าลบที่ถูกต้อง
    }
    return(num); // return ค่าติดลบนั้น  // ถ้าค่าบวกจะไม่เข้าcondตั้งแต่แรกอยู่แล้ว return num ได้ค่าบวกเหมือนเดิม
}
