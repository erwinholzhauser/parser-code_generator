/**********************************************************************************************
 * Homework #1 (P-Machine)
 *
 * Programmer: Erwin Holzhauser
 *
 * Due Date: February 2, 2014, @ 11:59 PM
 *
 * COP3402-0001, Spring 2014       Instructor: Euripides Montagne
 *
 * Description: Implementation of a virtual machine known as the P-machine.
 *              The P-machine is a stack machine with stack and code memory stores.
 *              The CPU contains 4 registers to handle stack and code segments, and
 *              16 (0 - 16) general purpose registers.
 *
 * Input: File containing PL/0 program with one instruction per line, and each line
 *        containing 4 integers representing the OP, R, L, and M fields of an instruction.
 *        Also, a separate file containing sequential user input for Operation 9 executions.
 *
 * Output: (1) Output1.txt : Print out of the program in interpreted assembly language, with
 *                           line numbers.
 *         (2) Output2.txt : Print out of the executing of the program in the virtual machine,
 *                           showing the stack, and PC, BP, and SP registers.
 *         (3) Output3.txt : Output of Operation 10 executions.
 *
 **********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

/****************/
/** Structures **/
/****************/

// Structure for instructions.
typedef struct instruction {

    int op; // Operation "Op" Code.
    int r;  // Register.
    int l;  // Lexicographical level or register in arithmetic and logic instructions.
    int m;  // Number, program or data address, or register, depending on operators.

} instruction;

/*
 * Structure for CPU.
 * Encapsultes the Register File, Instruction Register, Program Counter, Base Pointer,
 * and Stack Pointer.
 */
typedef struct CPU {

    int RF[ 16 ]; // Register file containing registers R0 - R15.

    instruction IR; // Instruction Register.

    int PC; // Program counter.
    int BP; // Base pointer;
    int SP; // Stack pointer;


} CPU;

/*************************/
/** Function Prototypes **/
/*************************/

/*
 * Input: Lexicographical level, l, and base.
 * Post-condition:  Find (and return) base l levels down.
 */
int base( int l, int base, int stack[] );

/*
 * Input: CPU, Stack, and Code data structure.
 * Post-Condition: P-Machine is initialized to initial / default values.
 */
void initPMachine( CPU *cpu, int stack[], int code[][ 4 ] );

/*
 * Input: Code data structure.
 * Pre-condition: Code DS entries initialized to zero.
 * Post-condition: Instructions from input file read into Code DS.
 */
void readInCode( int code[][ 4 ] );

// Post-condition: Loads a constant value m into register r.
void lit( int r, int m, CPU *cpu );

// Post-condition: Returns from a subroutine and restores the caller environment.
void rtn( CPU *cpu, int stack[], int bases[], int *baseCtr );

// Post-condition: Load value into selected register from the stack location at offset m from l
//                 lexicographical levels down.
void lod( int r, int l, int m, int stack[], CPU *cpu );

// Post-condition: Store value from selected register in stack location at offset m from l lexicographical
//                 levels down.
void sto( int r, int l, int m, int stack[], CPU *cpu );

// Post-condition: Call procedure at code[ m ]. Generates new activation record & program counter = m.
void cal( int l, int m, int stack[], CPU *cpu, int bases[], int *baseCtr );

// Post-condition: Allocate m locals. First three are the static link, dynamic link, and return address.
void inc( int m, CPU *cpu );

// Post-condition: Jump to instruction m.
void jmp( int m, CPU *cpu );

// Post-condition: Jump to instruction m if r = 0.
void jpc( int r, int m, CPU *cpu );

// Post-condition: Operation 9: "Write register r to screen" -- Actually, printed into "Output3.txt" file.
void writeRegToScreen( int r, FILE *f, CPU *cpu );

// Post-condition: Operation 10: "Read input from user and store it in a register r" -- Actually, input read sequentially from "Input2.txt" file.
void readInputToReg( int r, FILE *f, CPU *cpu );

// Post-condition: Register r = - (Register l).
void neg( int r, int l, CPU *cpu );

// Post-condition: Register r = Register l + Register m.
void add( int r, int l, int m, CPU *cpu );

// Post-condition: Register r = Register l - Register m.
void sub( int r, int l, int m, CPU *cpu );

// Post-condition: Register r = Register l * Register m.
void mul( int r, int l, int m, CPU *cpu );

// Post-condition: Register r = Register l / Register m.
void DIV( int r, int l, int m, CPU *cpu );

// Post-condition: If Register r is odd, return 1 to Register r; otherwise, return 0.
void odd( int r, CPU *cpu );

// Post-condition: Register r = Register l % Register m.
void mod( int r, int l, int m, CPU *cpu );

// Post-condition: If Register l = Register m, return 1 to Register r; otherwise, return 0.
void eql( int r, int l, int m, CPU *cpu );

// Post-condition: If Register l != Register m, return 1 to Register r; otherwise, return 0.
void neq( int r, int l, int m, CPU *cpu );

// Post-condition: If Register l < Register m, return 1 to Register r; otherwise, return 0.
void lss( int r, int l, int m, CPU *cpu );

// Post-condition: If Register l <= Register m, return 1 to Register r; otherwise, return 0.
void leq( int r, int l, int m, CPU *cpu );

// Post-condition: If Register l > Register m, return 1 to Register r; otherwise, return 0.
void gtr( int r, int l, int m, CPU *cpu );

// Post-condition: If Register l >= Register m, return 1 to Register r; otherwise, return 0.
void geq( int r, int l, int m, CPU *cpu );

// Input: Op-Code n.
// Output: String containing the corresponding instruction name. (i.e. 1 returns lit, 2 returns rtn, 3 returns lod, etc.).
char *opToName( int n );

/*************************/

int main( void ){

    int i, j; //Loop counters.

    //File output stream to print out the program in interpreted assembly language with line numbers.
    FILE *fout1 = fopen( "output1.txt", "w+" );

    int code[ MAX_CODE_LENGTH ][ 4 ];   //Create "code" data structure.
    int stack[ MAX_STACK_HEIGHT ];      //Create stack.
    CPU cpu;                            //Create CPU.

    int bases[ MAX_LEXI_LEVELS ];           //Keep track of base pointers when new activation records are created (or deleted).
    for( i = 0; i < MAX_LEXI_LEVELS; i++ )  //Initialize values.
        bases[ i ] = 0;
    int baseCtr = 0;                        //Counter for how many base pointers are saved.

    //Initialize P-Machine.
    initPMachine( &cpu, stack, code );

    //Print out the program in interpreted assembly language with line numbers:
    /*------------------------------------------------------------------------------------------------------------------------------*/
                                                                                                                                    //
    fprintf( fout1, "Line  OP     R    L    M\n" ); //Print format.                                                                 //
                                                                                                                                    //
    i = 0; //Initialized for line counting.                                                                                         //
                                                                                                                                    //
    //Print until end of code.                                                                                                      //
    while( !( code[ i ][ 0 ] == 0 && code[ i ][ 1 ] == 0 && code[ i ][ 2 ] == 0 && code[ i ][ 3 ] == 0 ) ){                         //
                                                                                                                                    //
        char *instrName;                                                                                                            //
        instrName = opToName( code[ i ][ 0 ] );                                                                                     //
        fprintf( fout1, "%4d %4s %4d %4d %4d\n", i, instrName, code[ i ][ 1 ], code[ i ][ 2 ], code[ i ][ 3 ] );                    //
        i++;                                                                                                                        //
                                                                                                                                    //
    }                                                                                                                               //
                                                                                                                                    //
    //Resource management.                                                                                                          //
    fclose( fout1 );                                                                                                                //
                                                                                                                                    //
    /*------------------------------------------------------------------------------------------------------------------------------*/

    //File output stream to print out execution of the program in the virtual machine.
    fout1 = fopen( "output2.txt", "w+" );
    fprintf( fout1, "Inital Values               pc   bp   sp\n" );

    //File output stream for operation 9 (Write register to screen).
    FILE *fout2 = fopen( "output3.txt", "w+" );

    //File input stream for operation 10 (Read user input into register).
    FILE *sio_fin = fopen( "input2.txt", "r" );

    //Execute instruction cycle while P-Machine is not in end-state.
    while( !( cpu.PC == 0 && cpu.BP == 0 && cpu.SP == 0 ) ){

        // (1) Fetch Cycle:

        // (1a) Fetch instruction from code store and place in the Instruction Register.
        cpu.IR.op = code[ cpu.PC ][ 0 ];  // Op-code.
        cpu.IR.r = code[ cpu.PC ][ 1 ];   // Register Field.
        cpu.IR.l = code[ cpu.PC ][ 2 ];   // Lexicographical Field.
        cpu.IR.m = code[ cpu.PC ][ 3 ];   // "M" Field.

        //Print to file instruction fetched as part of execution of the program in the virtual machine.
        fprintf( fout1, "%4d %4s %4d %4d %4d ", cpu.PC, opToName( code[ cpu.PC ][ 0 ] ), code[ cpu.PC ][ 1 ], code[ cpu.PC ][ 2 ], code[ cpu.PC ][ 3 ] );

        // (1b) Increment program counter to next instruction.
        cpu.PC += 1;

        // (2) Execute Cycle:

        //The OP component indicates the operation to be executed.
        switch( cpu.IR.op ){

            //Then, the R, L, and M components of the instruction are used to execute the appropriate instruction.

            case 1 :  //LIT R,0,M.
                lit( cpu.IR.r, cpu.IR.m, &cpu );
                break;

            case 2 :  //RTN 0,0,0.
                rtn( &cpu, stack, bases, &baseCtr );
                break;

            case 3 :  //LOD R,L,M.
                lod( cpu.IR.r, cpu.IR.l, cpu.IR.m, stack, &cpu );
                break;

            case 4 :  //STO R,L,M.
                sto( cpu.IR.r, cpu.IR.l, cpu.IR.m, stack, &cpu );
                break;

            case 5 :  //CAL 0,L,M.
                cal( cpu.IR.l, cpu.IR.m, stack, &cpu, bases, &baseCtr );
                break;

            case 6 :  //INC 0,0,M.
                inc( cpu.IR.m, &cpu );
                break;

            case 7 :  //JMP 0,0,M.
                jmp( cpu.IR.m, &cpu );
                break;

            case 8 :  //JPC R,0,M.
                jpc( cpu.IR.r, cpu.IR.m, &cpu );
                break;

            case 9 :  //SIO R,0,1.
                writeRegToScreen( cpu.IR.r, fout2, &cpu );
                break;

            case 10 : //SIO R,0,2.
                readInputToReg( cpu.IR.r, sio_fin, &cpu );
                break;

            case 11 : //NEG R,L,0.
                neg( cpu.IR.r, cpu.IR.l, &cpu );
                break;

            case 12 : //ADD R,L,M.
                add( cpu.IR.r, cpu.IR.l, cpu.IR.m, &cpu );
                break;

            case 13 : //SUB R,L,M.
                sub( cpu.IR.r, cpu.IR.l, cpu.IR.m, &cpu );
                break;

            case 14 : //MUL R,L,M.
                mul( cpu.IR.r, cpu.IR.l, cpu.IR.m, &cpu );
                break;

            case 15 : //DIV R,L,M.
                DIV( cpu.IR.r, cpu.IR.l, cpu.IR.m, &cpu );
                break;

            case 16 : //ODD R,0,0.
                odd( cpu.IR.r, &cpu );
                break;

            case 17 : //MOD R,L,M.
                mod( cpu.IR.r, cpu.IR.l, cpu.IR.m, &cpu );
                break;

            case 18 : //EQL R,L,M.
                eql( cpu.IR.r, cpu.IR.l, cpu.IR.m, &cpu );
                break;

            case 19 : //NEQ R,L,M.
                neq( cpu.IR.r, cpu.IR.l, cpu.IR.m, &cpu );
                break;

            case 20 : //LSS R,L,M.
                lss( cpu.IR.r, cpu.IR.l, cpu.IR.m, &cpu );

            case 21 : //LEQ R,L,M.
                leq( cpu.IR.r, cpu.IR.l, cpu.IR.m, &cpu );
                break;

            case 22 : //GTR R,L,M.
                gtr( cpu.IR.r, cpu.IR.l, cpu.IR.m, &cpu );
                break;

            case 23 : //GEQ R,L,M.
                geq( cpu.IR.r, cpu.IR.l, cpu.IR.m, &cpu );
                break;

        }

        //Print to file the PC, BP, SP, and stack as part of the execution of the program in the virtual machine,
        //after the execution of the instruction.

        fprintf( fout1, "%4d %4d %4d ", cpu.PC, cpu.BP, cpu.SP ); //PC, BP, and SP.

        //Stack.
        //For each stack index:
        for( i = 1; i <= cpu.SP; i++ ){

            //Need to separate activation records in program execution output with "|".
            //Check if stack index is an AR base.
            for( j = 0; j < MAX_LEXI_LEVELS; j++ )
                if( i > 1 && i == bases[ j ] )
                    fprintf( fout1, "|", stack[i] );

            //Print stack content.
            fprintf( fout1, "%4d ", stack[i] );

        }

        //Skip line in program execution output after each instruction cycle is executed.
        fprintf( fout1, "\n" );

    }

    //Resource management: Close file pointers.
    fclose( fout1 );
    fclose( fout2 );
    fclose( sio_fin );

    return 0;

}

/***************/
/** Functions **/
/***************/

int base( int l, int base, int stack[] ){

    int b1 = base;

    //Find base L levels down.
    while( l > 0 ){

        b1 = stack[ b1 + 1 ];
        l--;

    }

    return b1;

}

void initPMachine( CPU *cpu, int stack[], int code[][ 4 ] ){

    int i, j; //Loop counters.

    //Initialize CPU.

    // (2) Initialize Register File.
    for( i = 0; i < 16; i++ )
        cpu->RF[ i ] = 0;

    // (3) Initialize Instruction Register.
    cpu->IR.op  = 0;                                //Initialize op-code.
    cpu->IR.r   = 0;                                //Initialize register component.
    cpu->IR.l   = 0;                                //Initialize lexicographical level component.
    cpu->IR.m   = 0;                                //Initialize  "M" component.

    // (4) Initialize Program Counter, Base Pointer, and Stack Pointer.
    cpu->PC = 0;
    cpu->BP = 1;
    cpu->SP = 0;

    //Initialize Stack.
    for( i = 0; i < MAX_STACK_HEIGHT; i++ )
        stack[ i ] = 0;

    //Initialize "Code" entries.
    for( i = 0; i < MAX_CODE_LENGTH; i++ )
        for( j = 0; j < 4; j++ )
            code[ i ][ j ] = 0;

    //Read & store instructions to "Code".
    readInCode( code );

}

void readInCode( int code[][ 4 ] ){

    int i = 0; //Counter.

    //Open file input stream.
    FILE *fin = fopen( "input2.txt", "r" );

    //Read instructions until EOF.
    while( !feof( fin ) ){
        fscanf( fin, "%d %d %d %d", &code[i][0], &code[i][1], &code[i][2], &code[i][3] );
        i++;
    }

    //Close file input stream.
    fclose( fin );

}

void lit( int r, int m, CPU *cpu ){

    //Load literal m into Register r.
    cpu->RF[ r ] = m;

}

void rtn( CPU *cpu, int stack[], int bases[], int *baseCtr ){

    //Keep track of AR bases.
    bases[ *baseCtr ] = 0;
    *baseCtr--;

    //Restore caller environment.
    cpu->SP = cpu->BP - 1;          //Stack pointer.
    cpu->BP = stack[ cpu->SP + 3 ]; //Base Pointer.
    cpu->PC = stack[ cpu->SP + 4 ]; //Program Counter.

}

void lod( int r, int l, int m, int stack[], CPU *cpu ){

    //Load value into selected register from stack.
    cpu->RF[ r ] = stack[ base( l, cpu->BP, stack ) + m ];

}

void sto( int r, int l, int m, int stack[], CPU *cpu ){

    //Store value from selected register into stack.
    stack[ base( l, cpu->BP, stack ) + m ] = cpu->RF[ r ];

}

void cal( int l, int m, int stack[], CPU *cpu, int bases[], int *baseCtr ){

    //Generate activation record.
    stack[ cpu->SP + 1 ] = 0;                           // Space to return value.
    stack[ cpu->SP + 2 ] = base( l, cpu->BP, stack );   // Static link.
    stack[ cpu->SP + 3 ] = cpu->BP;                     // Dynamic link.
    stack[ cpu->SP + 4 ] = cpu->PC;                     // Return address.
    cpu->BP = cpu->SP + 1;
    cpu->PC = m;

    //Keep track of AR bases.
    bases[ *baseCtr ] = cpu->BP;
    *baseCtr++;

}

void inc( int m, CPU *cpu ){

    cpu->SP += m;

}

void jmp( int m, CPU *cpu ){

    cpu->PC = m;

}

void jpc( int r, int m, CPU *cpu ){

    if( cpu->RF[ r ] == 0 )
        cpu->PC = m;

}

void writeRegToScreen( int r, FILE *f, CPU *cpu ){

    //"Output to screen" written to file.
    fprintf( f, "%d\n", cpu->RF[ r ] );

}

void readInputToReg( int r, FILE *f, CPU *cpu ){

    //"Input from user" read from file.
    fscanf( f, "%d", &cpu->RF[ r ] );

}

void neg( int r, int l, CPU *cpu ){

    cpu->RF[ r ] = -( cpu->RF[ l ] );

}

void add( int r, int l, int m, CPU *cpu ){

    cpu->RF[ r ] = cpu->RF[ l ] + cpu->RF[ m ];

}

void sub( int r, int l, int m, CPU *cpu ){

    cpu->RF[ r ] = cpu->RF[ l ] - cpu->RF[ m ];

}

void mul( int r, int l, int m, CPU *cpu ){

    cpu->RF[ r ] = cpu->RF[ l ] * cpu->RF[ m ];

}

void DIV( int r, int l, int m, CPU *cpu ){

    cpu->RF[ r ] = cpu->RF[ l ] / cpu->RF[ m ];

}

void odd( int r, CPU *cpu ){

    cpu->RF[ r ] = cpu->RF[ r ] % 2;

}

void mod( int r, int l, int m, CPU *cpu ){

    cpu->RF[ r ] = cpu->RF[ l ] % cpu->RF[ m ];

}

void eql( int r, int l, int m, CPU *cpu ){

    cpu->RF[ r ] = ( cpu->RF[ l ] == cpu->RF[ m ] ) ? 1 : 0;

}

void neq( int r, int l, int m, CPU *cpu ){

    cpu->RF[ r ] = ( cpu->RF[ l ] != cpu->RF[ m ] ) ? 1 : 0;

}

void lss( int r, int l, int m, CPU *cpu ){

    cpu->RF[ r ] = ( cpu->RF[ l ] < cpu->RF[ m ] ) ? 1 : 0;

}

void leq( int r, int l, int m, CPU *cpu ){

    cpu->RF[ r ] = ( cpu->RF[ l ] <= cpu->RF[ m ] ) ? 1 : 0;

}

void gtr( int r, int l, int m, CPU *cpu ){

    cpu->RF[ r ] = ( cpu->RF[ l ] > cpu->RF[ m ] ) ? 1 : 0;

}

void geq( int r, int l, int m, CPU *cpu ){

    cpu->RF[ r ] = ( cpu->RF[ l ] >= cpu->RF[ m ] ) ? 1 : 0;

}

char *opToName( int n ){

    char *temp;

    //Choose instruction name to return.
    switch( n ){

        case 1:
            temp = "lit";
            return temp;

        case 2:
            temp = "rtn";
            return temp;

        case 3:
            temp = "lod";
            return temp;

        case 4:
            temp = "sto";
            return temp;

        case 5:
            temp = "cal";
            return temp;

        case 6:
            temp = "inc";
            return temp;

        case 7:
            temp = "jmp";
            return temp;

        case 8:
            temp = "jpc";
            return temp;

        case 9:
            temp = "sio";
            return temp;

        case 10:
            temp = "sio";
            return temp;

        case 11:
            temp = "neg";
            return temp;

        case 12:
            temp = "add";
            return temp;

        case 13:
            temp = "sub";
            return temp;

        case 14:
            temp = "mul";
            return temp;

        case 15:
            temp = "div";
            return temp;

        case 16:
            temp = "odd";
            return temp;

        case 17:
            temp = "mod";
            return temp;

        case 18:
            temp = "eql";
            return temp;

        case 19:
            temp = "neq";
            return temp;

        case 20:
            temp = "lss";
            return temp;

        case 21:
            temp = "leq";
            return temp;

        case 22:
            temp = "gtr";
            return temp;

        case 23:
            temp = "geq";
            return temp;

        default:
            return NULL;
    }

}
