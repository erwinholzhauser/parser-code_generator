#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_SYMBOL_TABLE_SIZE 10000
#define CODE_SIZE 10000

//Data structure representing a token.
typedef struct{
  char type[3];   //Type of token (1 - 33).
  char value[12]; //Value of token (Identifier Name, or Number).
} token;

//Data structure representing a symbol.
typedef struct{
  int kind;      //const = 1, var = 2.
  char name[12]; //Name up to 11 characters.
  int val;       //Number (ASCII value).
  /*int level;   //L level. Not applicable.*/
  int reg;       //M address.
  int init;       //Boolean; If initialized, = 1; otherwise, = 0.
} symbol;

//Keep track of registers occupied registers.
//After var/const is bound to register, increment.
int reg_ctr;

//Data structure representing assembly language code
//for virtual machine created in HW1.
typedef struct{
  int op; //opcode.
  int r;  //register.
  int l;  //lexicographical level.
  int m;  //modifier.
} code;

token curr_token; //Current token to analyze.

int symbol_ctr; //Counter for symbols inserted to symbol table.
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE]; //Symbol table.

int cx; //Code index.
code code_ds[CODE_SIZE]; //Code data structure.

//File pointer to token / internal representation file.
FILE *token_fin;

//Input: curr_token, token_fin. 
//Post-conditions: Gets the next token, and stores it in curr_token.
//                 If successful, return 1; Otherwise, return 0.
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

//Input: Error number, n.
//Post-condition: Stops parsing, and shows error message.
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
    code_ds[cx].op = op; //opcode
    code_ds[cx].r = r; //register
    code_ds[cx].l = l; //lexicographical level
    code_ds[cx].m = m; //modifier
    cx++;
  }
}

//Post-condition: Insert symbol with specified parameters into symbol table.
void insert_symbol( int k, char name[], int val, int reg, int i ){

  //Set symbol fields.
  symbol_table[symbol_ctr].kind = k;
  strcpy( symbol_table[symbol_ctr].name, name );
  symbol_table[symbol_ctr].val = val;
  symbol_table[symbol_ctr].reg = reg;
  symbol_table[symbol_ctr].init = i;
  symbol_ctr++; //Increment counter.

}

void term();

void expression(){
  
  /*
   * production rule:
   * expression ::= ["+"|"-"] term { ("+"|"-") term }.
   */

  //Follow grammar.
  if( !strcmp(curr_token.type, "4" /*plussym*/) || !strcmp(curr_token.type, "5" /*minussym*/) ){
    get_token();
  }

  term();

  while( !strcmp(curr_token.type, "4" /*plussym*/) || !strcmp(curr_token.type, "5" /*minussym*/) ){
    get_token();
    term();
  }
  
}

//Post-conditions: If current token is a relational operator, return 1; otherwise, return 0.
int rel_op(){
  /*
   * production rule:
   * rel-op ::= "="|"<>"|"<"|"<="|">"|">=".
   */

  //Follow grammar.
  if( !strcmp(curr_token.type, "=" ) || !strcmp(curr_token.type, "<>" ) || !strcmp(curr_token.type, "<" ) ||
      !strcmp(curr_token.type, "<=" ) || !strcmp(curr_token.type, ">" ) || !strcmp(curr_token.type, ">=" ) )
    return 1;
  else
    return 0;
  
}

void condition(){
  
  /*
   * production rule:
   * condition ::= "odd expression | expression rel-op expression.
   */

  //Follow the grammar.
  if( !strcmp(curr_token.type, "8" /*oddsym*/) ){
    get_token();
    expression();
  }

  else{
    expression();
    if( rel_op() != 0 ) error(20);
    get_token();
    expression();
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

  //ident
  if( !strcmp(curr_token.type, "2") ){

    //Check for undeclared.
    for( i = 0; i < symbol_ctr; i++ )
      if( !strcmp( curr_token.value, symbol_table[i].name ) ){
	dec = 1; //Declared!
	ident_index = i; //Save index.
      }
    
    //Undeclared identifier?
    if( !dec ) error(11);

    //Check for uninitialized.
    if( !symbol_table[ident_index].init ) error(28); //Uninitialized!

    get_token();

  }

  // | number
  else if( !strcmp(curr_token.type, "3") )
    get_token();

  // | "(" expression ")".
  else if( !strcmp(curr_token.type, "15") ){
    get_token();
    expression();
    if( strcmp(curr_token.type, "16") != 0 )
      error(22);
    get_token();
  }
  else
    error(23);
}

void term(){  token addop;

  /*
   * production rule:
   * term ::= factor {("*"|"/" factor}.
   */

  //Follow grammar.
  factor();
  while( !strcmp(curr_token.type, "6") || !strcmp(curr_token.type, "7") ){
    get_token();
    factor();
  }

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

    //Identifier has been initialized. Update symbol table.
    symbol_table[ident_index].init = 1;

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
  else if( !strcmp(curr_token.type, "23" /*ifsym*/) ){
    get_token();
    condition();
    if( strcmp(curr_token.type, "24" /*thensym*/) != 0 ) error(16);
    get_token();
    statement();
  }
  else if( !strcmp(curr_token.type, "25" /*whilesym*/) ){
    get_token();
    condition();
    if( strcmp(curr_token.type, "26" /*dosym*/) != 0 ) error(18);
    get_token();
    statement();
  }
}

void block(){
  
  //Used for insertion to symbol table.
  //Whenever a constant or variable is successfully parsed, store appropriate
  //values into these variables, and insert to symbol table.
  char name[12];
  int val;
  int reg;
  
  
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
      
      //Keep track of symbol identifier for symbol table.
      strcpy(name, curr_token.value);

      get_token();

      if( strcmp(curr_token.type, "9" /*eqlsym*/) != 0 ) error(3);
      get_token();

      if( strcmp(curr_token.type, "3" /*numbersym*/) != 0 ) error(2);
      
      //Keep track of symbol value for symbol table.
      val = atoi(curr_token.value);

      //Store constant to symbol table, if registers permit.
      if( reg_ctr <= 15 ){
	insert_symbol( 1 /*constant*/, name, val, reg_ctr, 1 );
	reg_ctr++;
      }
      else error(27); //All registers in use!
      
      get_token();


    }while( !strcmp(curr_token.type, "17" /*commasym*/) );
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

      //Store constant to symbol table, if registers permit.
      if( reg_ctr <= 15 ){
	insert_symbol( 2 /*constant*/, name, 0, reg_ctr, 0 ); //Set undeclared flag.
	reg_ctr++;
      }
      else error(27); //All registers in use!
      
      get_token();

    }while( !strcmp(curr_token.type, "17" /*commasym*/) );

    if( strcmp(curr_token.type, "18" /*semicolonsym*/) != 0 ) error(5);
    get_token();
  }
  //statement.
  statement();

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

int main(){

  int i; //index.

  //Open file input stream.
  token_fin = fopen("token_file.txt", "r");

  //Initialize counters.
  symbol_ctr = 0;
  cx = 0;
  reg_ctr = 0;

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
