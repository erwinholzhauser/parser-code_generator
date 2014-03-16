#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_SYMBOL_TABLE_SIZE 10000

//Data structure representing a token.
typedef struct{
  char type[3];   //Type of token (1 - 33).
  char value[12]; //Value of token (Identifier Name, or Number).
} token;

//Data structure representing a symbol.
typedef struct{
  int kind;      //const = 1, var = 2, proc = 3.
  char name[12]; //Name up to 11 characters.
  int val;       //Number (ASCII value).
  int level;     //L Level.
  int addr;      //M address.
} symbol;

token curr_token; //Current token to analyze.

int symbol_ctr; //Counter for symbols inserted to symbol table.
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE]; //Symbol table.

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

  }

  //Debugging Statement:
  printf("Token: Type: %s, Value: %s .\n", curr_token.type, curr_token.value);
  
  //Resource management.
  fclose(token_fin);

  //Terminate program.
  exit(0);
}

void term();

void expression(){
  
  /*
   * production rule:
   * expression ::= ["+"|"-"] term { ("+"|"-") term }.
   */

  //Follow grammar.
  if( !strcmp(curr_token.type, "4" /*plussym*/) || !strcmp(curr_token.type, "5" /*minussym*/) )
    get_token();

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

  /*
   * production rule:
   * factor ::= ident | number | "(" expression ")".
   */

  //Follow grammar.
  if( !strcmp(curr_token.type, "2") )
    get_token();
  else if( !strcmp(curr_token.type, "3") )
    get_token();
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

void term(){

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
    get_token();
    if( strcmp(curr_token.type, "20" /*becomessym*/) != 0 ) error(3);
    get_token();
    expression();
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
      get_token();
      if( strcmp(curr_token.type, "9" /*eqlsym*/) != 0 ) error(3);
      get_token();
      if( strcmp(curr_token.type, "3" /*numbersym*/) != 0 ) error(2);
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

  //Open file input stream.
  token_fin = fopen("token_file.txt", "r");

  //Initialize.
  symbol_ctr = 0;
  //get_token(); //Get first token.

  //Recursively parse tokens.
  program();

  //Resource management.
  fclose(token_fin);

  return 0;
}


