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

// ประกาศ prototype
void printState(stateType *); 
int convertNum(int num);


int main(int argc, char *argv[])// จำนวนคำสั่ง , คำสั่งจริง 
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr; // pointer ที่ชีไปที่ไฟล์นั้นๆ

    // แจ้ง error ถ้าจำนวนคำสั่งไม่เท่ากับ 2 
    if (argc != 2) {
	printf("error: usage: %s <machine-code file>\n", argv[0]);
	exit(1);
    }
    // เปิดไฟล์ .mc
    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
	printf("error: can't open file %s", argv[1]);
	perror("fopen"); // สาเหตุที่ error
	exit(1); // จบโปรแกรม
    }

    // set ค่าตัวแปรเป็น 0 ทุกครั้งก่อนเริ่มโปรแกรม   pc , mem , reg , numMemory
    memset (&state, 0 ,sizeof(state) ) ; // เข้าถึงที่อยู่ address จริงแล้วทำการกำหนดค่า ใหม่ 

    /* read in the entire machine-code file into memory */
    // อ่าน machine code ทีละ บรรทัด แปลงเป็นตัวเลข แล้วใส่ลงใน mem[]
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; // จากที่อ่านมาเป็น ข้อความ ต้องแปลงเป็นตัวเลขก่อน  /  อ่าน 1 บรรทัดจาก ไฟล์ มาเก็บใน line (ข้อความ) 
	state.numMemory++) { // ไปบรรทัดคำสั่งถัดไป
	if (sscanf(line, "%d", state.mem+state.numMemory) != 1) { // แปลงข้อความเป็น ฐาน10 แล้วเก็บไว้ที่   state.mem+state.numMemory /  sscanf :  อ่านข้อความที่เก็บไว้ใน line (variable) / เอาเฉาพะข้อความที่เป็นตัวเลข
	    printf("error in reading address %d\n", state.numMemory);
	    exit(1);
	}
	printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }
    printf("\n");

    int count = 0  ;  // count เก็บจำนวน Instruction ที่รันไปแล้ว 
    while(1) {          // วนจนกว่าจะเจอ halt
        printState(&state) ; // print result ของ State ก่อนหน้า ก่อน โปรแกรมจะเริ่มทำ State ถัดไป
        int ins = state.mem[state.pc] ;   // fetch Ins ที่ pc ชี้อยู่ 
        int opc = (ins >> 22 ) &  0x7 ;  // decode เลื่อนบิต 22-24  (R shift ) เก็บไว้ 3 bit
        // 0x7 : 0000 0000 0000 0000 0000 0000 0000 0111
                                  
        int regA = (ins >> 19  ) & 0x7; // 19-21     
        int regB = (ins >> 16) & 0x7 ; // 16-18
        int dest = ins & 0x7 ; // อยู่บิตขวาสุดอยู่แล้ว ไม่ต้อง shift 
        int offset = convertNum(ins & 0xFFFF) ; // 16 bit ขวาเป็น offset +,- ,มีผลเลยต้อง sign extend

        state.pc++ ; 
        count++ ; 

        switch(opc) { // 101
            // add : destReg = reg[A] + reg[B]
            case 0b000: state.reg[dest] = state.reg[regA] + state.reg[regB] ; 
            break ; 
            //nand : destReg = not (reg[A] & reg[B]) / AND กันทีละตัวแล้วกลับ bit
            case 0b001: state.reg[dest] = ~(state.reg[regA] & state.reg[regB]) ; 
            break ; 
            //lw  : load ค่า ใน mem มาเก็บไว้ที่ regฺ[B] / reg[B] = reg[A] + offset
            case 0b010: state.reg[regB] = state.mem[state.reg[regA]  + offset] ; 
            break;
            //sw : เก็บค่าใน reg[B] ลง memory / mem = reg[A] + offset
            case 0b011: state.mem[state.reg[regA] + offset] = state.reg[regB];
            break ; 
            // beq : ถ้า  reg[A] == reg[B]  ให้กระโดดไปที่ pc+1+offset 
            // pc + 1 ถูกทำก่อนเข้า switch แล้ว ใน case นี้ จึงบวกแต่ offset (offset ติดลบได้ เพื่อใช้กระโดด ย้อนกลับได้ )
            case 0b100: if (state.reg[regA] == state.reg[regB]) {
                state.pc += offset ; 
            }
             break;
            // jalr : เก็บ pc+1 ลง [regB]   จากนั้นกระโดดไป addr ที่อยู่ใน reg[A] 
            // ถ้า reg[A] กับ reg[B] เป็นตัวเดียวกัน จะกระโดดไปที่ pc+1                                             
            case 0b101: { // jalr :  regA(addr dest)  regB (pc+1 )  
                                                               
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
