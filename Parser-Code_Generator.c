#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//Struct representing token.
typedef struct{
  char type[3]; //Type of token (1 - 33).
  char value[12]; //Value of token (Identifier Name, or Number).
} token;

token curr_token; //Current token to analyze.

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

    //Successful token fetch.
    return 1;
  }
  //No more tokens.
  else return 0;

}

int main(){
  
  //Open file input stream.
  token_fin = fopen("token_file.txt", "r");



  //Resource management.
  fclose(token_fin);

  return 0;
}


