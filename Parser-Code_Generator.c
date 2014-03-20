#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_SYMBOL_TABLE_SIZE 10000
#define CODE_SIZE 500

//Data structure representing a token.
typedef struct{
  char type[3];   //Type of token (1 - 33).
  char value[12]; //Value of token (identifier, or number value).
} token;

/*
 * Data structure representing symbol table entries.
 * By default, declared variables are initialized to 0.
 */
typedef struct{
  int kind;      //const = 1, var = 2.
  char name[12]; //Name up to 11 characters.
  int val;       //Number (ASCII value).
  int level;     //L level. Not applicable for this homework.
  int addr;      //M address.
} symbol;

/*
 * For code generation, we are treating the register file as a stack.
 * The register pointer will point to top of "stack".
 * As calculations are performed, the register pointer will be appropriately incremented
 * and decremented. Calculation results should ultimately be stored in reg[0],
 * after which the result will be appropriately be stored back to memory.
 */
int reg_ptr;

/*
 * Stack pointer:
 * When declaring constants and variables, we will store them in memory (on the stack).
 * Need to leave space for the activation record (return value, SL, DL, and RA), so we
 * will initialize it past the space for these values.
 */
int sp;

/*
 * Data structure representing assembly language code
 * for virtual machine created in HW1.
 */
typedef struct{
  int op; //opcode.
  int r;  //register.
  int l;  //lexicographical level.
  int m;  //modifier.
} code;

token curr_token; //Current token to analyze.

int symbol_ctr;                             //Counter for symbol table entries created.
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE]; //Symbol table data structure.

int cx;                     //Code index.
code code_ds[CODE_SIZE];    //Code data structure.

//File pointer to token (internal representation) file.
FILE *token_fin;

//Input: curr_token, token_fin.
//Post-conditions: Gets the next token, and stores it in curr_token.
//                 If successful, return 1; Otherwise, return 0.
int get_token();

//Input: Error number, n.
//Post-condition: Stops parsing, and shows error message.
void error( int n );

void emit(int op, int r, int l, int m);

//Post-condition: Insert symbol with specified parameters into symbol table.
void insert_symbol( int k, char name[], int val, int addr );

void program();

void block();

void statement();

void condition();

//Post-conditions: If current token is a relational operator, return corresponding op-code; otherwise, return 0.
int rel_op();

void expression();

void term();

void factor();

int main(){

  int i; //index.

  //Open file input stream.
  token_fin = fopen("token_file.txt", "r");

  //Initialize counters.
  symbol_ctr = reg_ptr = cx = 0;
  sp = 5;

  //Initialize code data structure.
  for( i = 0; i < CODE_SIZE; i++ ){
    code_ds[i].op = 0;
    code_ds[i].r = 0;
    code_ds[i].l = 0;
    code_ds[i].m = 0;
  }

  //Recursively parse tokens.
  program();

  //Resource management.
  fclose(token_fin);

  return 0;
}

void program(){

  /*
   * production rule:
   * program ::= block ".".
   */

  //Follow grammar.
  get_token();
  block();
  if( strcmp(curr_token.type, "19" /*periodsym*/) != 0 ) error(9);
}

void block(){

  /* For insertion to symbol table:
   * When a constant or variable declaration is successful parsed,
   * store the identifier and value, to create a new symbol table entry.
   */
  char name[12];    //Symbol identifier.
  int val;          //Symbol value.

  /*
   * production rule:
   * block ::= const-declaration var-declaration statement.
   */

  //Follow grammar.

  //const-declaration ::= ["const" ident "=" number {"," ident "=" number} ";"].
  if( !strcmp(curr_token.type, "28" /*constsym*/) ){
    do{

      get_token();

      if( strcmp(curr_token.type, "2" /*identsym*/) != 0 ) error(4);

      //Keep track of symbol identifier for symbol table entry.
      strcpy(name, curr_token.value);

      get_token();

      if( strcmp(curr_token.type, "9" /*eqlsym*/) != 0 ) error(3);

      get_token();

      if( strcmp(curr_token.type, "3" /*numbersym*/) != 0 ) error(2);

      //Keep track of symbol value for symbol table entry.
      val = atoi(curr_token.value);

      //Create symbol table entry, and update stack pointer.
	  insert_symbol( 1 /*constant*/, name, val, sp );
	  sp++;

      get_token();


    } while( !strcmp(curr_token.type, "17" /*commasym*/) );

    if( strcmp(curr_token.type, "18" /*semicolonsym*/) != 0 ) error(5);

    get_token();

  }

  //var-declaration ::= ["int" ident {"," ident} ";"].
  if( !strcmp(curr_token.type, "29" /*intsym*/) ){
    do{

      get_token();

      if( strcmp(curr_token.type, "2" /*identsym*/) != 0 ) error(4);

      //Keep track of symbol identifier for symbol table.
      strcpy(name, curr_token.value);

      //Create symbol table entry, and update stack pointer.
      //Variables are initialized to 0.
      insert_symbol( 2 /*variable*/, name, 0, sp );
      sp++;

      get_token();

    }while( !strcmp(curr_token.type, "17" /*commasym*/) );

    if( strcmp(curr_token.type, "18" /*semicolonsym*/) != 0 ) error(5);
    get_token();
  }

  //statement.
  statement();

}

void statement(){

  int i; //index.

  //Boolean to check if identifier is declared. If so, = 1; else, = 0.
  int dec = 0;

  //Save symbol table index for found identifier.
  int ident_index;

  /*
   * production rule:
   * statement ::= [ ident ":=" expression
   *               | "begin" statement { ";" statement } "end"
   *		   | "if" condition "then" statement
   *		   | "while" condition "do" statement
   *		   | "read" ident
   *		   | "write" ident
   *		   | e ].
   */

  //Follow grammar.
  // ident ":=" expression
  if( !strcmp(curr_token.type, "2" /*identsym*/) ){

    //Check if identifier has been declared.
    for( i = 0; i < symbol_ctr; i++ )
        if( !strcmp( curr_token.value, symbol_table[i].name ) ){ /*found*/

            if( symbol_table[i].kind == 1 /*const*/ ) error(12);

            else if( symbol_table[i].kind == 2 /*var*/ ){
                dec = 1; //Variable, and declared!
                ident_index = i; //Save identifier index.
            }

        }

    //Undeclared identifier?
    if( !dec ) error(11);

    get_token();

    if( strcmp(curr_token.type, "20" /*becomessym*/) != 0 ) error(3);

    get_token();

    expression();

    /*
     * Store the result of the expression at the memory address assigned to the symbol at
     * the left-hand side of the assignment statement.
     */

     emit( 4 /*sto*/, reg_ptr - 1, 0, symbol_table[ ident_index ].addr /*symbol address*/ );
     reg_ptr--;

  }
  /*
   * EDIT: Removed. No call statements in HW3 grammar.
   *
   * else if( !strcmp(curr_token.type, "27" /*callsym) ){
   *   get_token();
   *   if( strcmp(curr_token.type, "2" /*identsym*) ) error(14);
   *   get_token();
   * }
   */

  else if( !strcmp(curr_token.type, "21" /*beginsym*/) ){
    get_token();
    statement();
    while( !strcmp(curr_token.type, "18" /*semicolonsym*/) ){
      get_token();
      statement();
    }
    if( strcmp(curr_token.type, "22" /*endsym*/) != 0 ) error(26);
    get_token();
  }

  // if <condition> then <statement>
  else if( !strcmp(curr_token.type, "23" /*ifsym*/) ){

    get_token();

    condition();

    if( strcmp(curr_token.type, "24" /*thensym*/) != 0 ) error(16);

    get_token();

    /*begin code generation for if-statement*/

    int ctemp = cx; //Save current code index.

    /*
     * Generate conditional jump instruction to execute or skip assembly code
     * generated for the inside of the if statement. We won't know where to jump until
     * the statement is recursively parsed, and code is generated for it. We'll use
     * ctemp to modify the jump instruction after the desired code index is calculated.
     */

     emit( 8 /*jpc*/, reg_ptr - 1, 0, 0 );
     reg_ptr--; //Done with condition result. Free up register.

    statement();

    code_ds[ ctemp ].m = cx; //Have correct cx, so update jpc.

  }

  // while <condition> do <statement>
  else if( !strcmp(curr_token.type, "25" /*whilesym*/) ){

    int cx1 = cx;

    get_token();

    condition();

    int cx2 = cx; //Code index for JPC.

    emit( 8 /*jpc*/, reg_ptr - 1, 0, 0 );

    if( strcmp(curr_token.type, "26" /*dosym*/) != 0 ) error(18);

    get_token();

    statement();

    emit( 7 /*jmp*/, 0, 0, cx1 );
    reg_ptr--;

    code_ds[ cx2 ].m = cx;

  }

}

void condition(){

  /*
   * production rule:
   * condition ::= "odd" expression | expression rel-op expression.
   */

  //Follow the grammar.

  // "odd" expression
  if( !strcmp(curr_token.type, "8" /*oddsym*/) ){

    get_token();

    expression();

    //Generate code to store truth value of condition in RF.
    emit( 16 /*odd*/, reg_ptr - 1, reg_ptr - 1, 0 );

  }

  // | expression rel-op expression
  else{

    expression();

    //If token is relational operator, get assembly code op-code.
    int op = rel_op();

    if( !op ) error(20);

    get_token();

    expression();

    //Generate code to store truth value of condition in RF.
    emit( op, reg_ptr - 2, reg_ptr - 2, reg_ptr - 1 );
    reg_ptr--; //Update RP.

  }

}

int rel_op(){
  /*
   * production rule:
   * rel-op ::= "="|"<>"|"<"|"<="|">"|">=".
   */

  //Follow grammar.

  //If relational operator, return appropriate assembly op-code.
  if( !strcmp( curr_token.type, "9" /*eqlsym*/ ) )
    return 18;
  else if( !strcmp( curr_token.type, "10" /*neqsym*/ ) )
    return 19;
  else if( !strcmp( curr_token.type, "11" /*lessym*/ ) )
    return 20;
  else if( !strcmp( curr_token.type, "12" /*leqsym*/ ) )
    return 21;
  else if( !strcmp( curr_token.type, "13" /*gtrsym*/ ) )
    return 22;
  else if( !strcmp( curr_token.type, "14" /*geqsym*/ ) )
    return 23;

  //Else, return 0 (false).
  else
    return 0;

}

void expression(){

    //Keep track of current operator token, for code generation.
    char addop[3];

  /*
   * production rule:
   * expression ::= ["+"|"-"] term { ("+"|"-") term }.
   */

  //Follow grammar.
  if( !strcmp(curr_token.type, "4" /*plussym*/) || !strcmp(curr_token.type, "5" /*minussym*/) ){

    //Save current operator token type.
    strcpy( addop, curr_token.type );

    get_token();

    term();

    //If term (operand) is preceded by a minus symbol, generate code to negate.
    if( !strcmp( addop, "5" /*minussym*/ ) )
        emit( 11 /*neg*/, reg_ptr - 1, reg_ptr - 1, 0 );

  }

  else term();

  while( !strcmp(curr_token.type, "4" /*plussym*/) || !strcmp(curr_token.type, "5" /*minussym*/) ){

    //Save current operator token type.
    strcpy( addop, curr_token.type );

    get_token();

    term();

    //If term (operand) is preceded by a plus symbol, generate code to add to prior operand.
    if( !strcmp( addop, "4" /*plussym*/ ) ){
        emit( 12 /*add*/, reg_ptr - 2, reg_ptr - 2, reg_ptr - 1 );
        reg_ptr--;
    }

    // " " minus symbol " " subtract " ".
    else{
        emit( 13 /*sub*/, reg_ptr - 2, reg_ptr - 2, reg_ptr - 1 );
        reg_ptr--;
    }

  }

}

void term(){

   //Will hold token type, for code generation.
   char mulop[3];

  /*
   * production rule:
   * term ::= factor {("*"|"/" factor}.
   */

  //Follow grammar.

  factor();

  while( !strcmp(curr_token.type, "6" /*multsym*/) || !strcmp(curr_token.type, "7" /*slashsym*/) ){

    //Save token type.
    strcpy( mulop, curr_token.type );

    get_token();

    factor();

    //If operator is multiplication.
    if( !strcmp( mulop, "6" /*multsym*/ ) ){

        //Generate multiplication instruction. Operands are stored in RF.
        emit( 14 /*mult*/, reg_ptr - 2, reg_ptr - 2, reg_ptr - 1 );
        reg_ptr--;

    }

    //If operator is division.
    else{

        //Generation division instruction. Operands are stored in RF.
        emit( 15 /*div*/, reg_ptr - 2, reg_ptr - 2, reg_ptr - 1 );
        reg_ptr--;

    }

  }

}

void factor(){

  int i; //index.

  //Boolean to check if identifier is declared. If so, = 1; else, = 0.
  int dec = 0;

  //Save symbol table index for found identifier.
  int ident_index;

  /*
   * production rule:
   * factor ::= ident | number | "(" expression ")".
   */

  //Follow grammar.

  // ident
  if( !strcmp(curr_token.type, "2" /*identsym*/) ){

    //Check for undeclared.
    for( i = 0; i < symbol_ctr; i++ )
      if( !strcmp( curr_token.value, symbol_table[i].name ) ){
        dec = 1; //Declared!
        ident_index = i; //Save index.
      }

    //Undeclared identifier?
    if( !dec ) error(11);

    //Place identifier on Register File "stack" for operations.
    emit( 1 /*lit*/, reg_ptr, 0, symbol_table[i].val );
    reg_ptr++; //Increment RP.

    get_token();

  }

  // | number
  else if( !strcmp(curr_token.type, "3") ){

    //Place number on Register File "stack" for operations.
    emit( 1 /*lit*/, reg_ptr, 0, atoi(curr_token.value) );
    reg_ptr++; //Increment RP.

    get_token();

  }

  // | "(" expression ")".
  else if( !strcmp(curr_token.type, "15" /*left parenthesis*/) ){

    get_token();

    expression();

    if( strcmp(curr_token.type, "16" /*right parenthesis*/) != 0 ) error(22);

    get_token();

  }

  else
    error(23);

}

int get_token(){

  //Get next token type.
  if( fscanf( token_fin, "%s", curr_token.type ) != EOF ){

    //If token is identsym or numbersym, get token value.
    if( !strcmp(curr_token.type, "2") || !strcmp(curr_token.type, "3") )
      fscanf( token_fin, "%s", curr_token.value );

    //All other tokens.
    else curr_token.value[0] = '\0';

    //Debugging Statement:
    printf("Token: Type: %s, Value: %s .\n", curr_token.type, curr_token.value);

    //Successful token fetch.
    return 1;
  }

  //No more tokens.
  else{
    curr_token.type[0] = '\0'; //Set token type to null.
    curr_token.value[0] = '\0'; //Set token value to null.
    return 0;
  }

}

void error( int n ){

  printf("***** Error number ");

  switch( n ){

  case 1:
    printf("1, use = instead of :=\n");
    break;
  case 2:
    printf("2, = must be followed by a number\n");
    break;
  case 3:
    printf("3, identifier must be followed by =\n");
    break;
  case 4:
    printf("4, const, int, procedure must be followed by identifier\n");
    break;
  case 5:
    printf("5, semicolon or comma missing\n");
    break;
  case 6:
    printf("6, incorrect symbol after procedure declaration\n");
    break;
  case 7:
    printf("7, statement expected\n");
    break;
  case 8:
    printf("8, incorrect symbol after statement part in block\n");
    break;
  case 9:
    printf("9, period expected\n");
    break;
  case 10:
    printf("10, semicolon between statements missing\n");
    break;
  case 11:
    printf("11, undeclared identifier\n");
    break;
  case 12:
    printf("12, assignment to constant or procedure is not allowed\n");
    break;
  case 13:
    printf("13, assignment operator expected\n");
    break;
  case 14:
    printf("14, call must be followed by an identifier\n");
    break;
  case 15:
    printf("15, call of a constant or variable is meaningless\n");
    break;
  case 16:
    printf("16, then expected\n");
    break;
  case 17:
    printf("17, semicolon or } expected\n");
    break;
  case 18:
    printf("18, do expected\n");
    break;
  case 19:
    printf("19, incorrect symbol following statement\n");
    break;
  case 20:
    printf("20, relational operator expected\n");
    break;
  case 21:
    printf("21, expression must not contain a procedure identifier\n");
    break;
  case 22:
    printf("22, right parenthesis missing\n");
    break;
  case 23:
    printf("23, the preceding factor cannot begin with this symbol\n");
    break;
  case 24:
    printf("24, an expression cannot begin with this symbol\n");
    break;
  case 25:
    printf("25, this number is too large\n");
    break;
  case 26:
    printf("26, end expected\n");
    break;
  case 27:
    printf("27, all available registers in use\n");
    break;
  case 28:
    printf("28, variable not initialized\n");
    break;

  }

  //Resource management.
  fclose(token_fin);

  //Terminate program.
  exit(0);
}

void emit(int op, int r, int l, int m){

  if( cx > CODE_SIZE )
    error(25); //Code exceeds max size. TO-DO: Replace error type.

  else{
    code_ds[cx].op = op;    //opcode
    code_ds[cx].r = r;      //register
    code_ds[cx].l = l;      //lexicographical level
    code_ds[cx].m = m;      //modifier
    cx++;
  }

}

void insert_symbol( int k, char name[], int val, int addr ){

  //Set symbol fields.
  symbol_table[symbol_ctr].kind = k;
  strcpy( symbol_table[symbol_ctr].name, name );
  symbol_table[symbol_ctr].val = val;
  symbol_table[symbol_ctr].addr = addr;

  symbol_ctr++; //Increment counter.

}
