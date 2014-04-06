/**********************************************************************************************
 * Homework #2 (Lexical Analyzer)
 *
 * Programmer: Erwin Holzhauser
 *
 * Due Date: February 16, 2014, @ 11:59 PM
 *
 * COP3402-0001, Spring 2014       Instructor: Euripides Montagne
 *
 * Description: Implementation of a lexical analyzer for the programming language PL/0.
 *              Capable of reading a PL/0 source program, identifying some errors, and
 *              producing as output: the source program, and a lexeme table and list for the
 *              source program.
 *
 * Input: File containing PL/0 program, with the following constraints:
 *        (1) Max length of identifiers is 11 characters.
 *        (2) Max length of numbers is 5 digits in length.
 *        (3) Comments and invisible characters are ignored.
 *        (4) Does not have to be grammatically valid PL/0 code.
 *
 * Output: (1) source_program.txt : Copy of input file.
 *         (2) lexeme_table.txt : Lexeme table for source program.
 *         (3) lexeme_list.txt : Lexeme list for source program.
 *
 **********************************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**CONSTANTS**/
#define MAX_ID_LEN 11
#define MAX_NUM_LEN 5

/**FUNCTION PROTOTYPES**/

//Input: String, s.
//Output: If s is a reserved word, return the numerical token value; otherwise, return -1.
int isReservedWord( char* s );

//Input: String, s.
//Output: If s is a special symbol, return 1; otherwise, return -1.
int isSpecialSymbol( char c );

//Input: Token, s.
//Output: If the token contains a lexical error, print an error message and return 1; otherwise, return 0.
int detectError( char *s );

//Input: Valid token, s.
//Output: Return the numerical token value of s.
int isOtherValid( char *s );

/**MAIN FUNCTION**/

int main( void ){

    int i; //Loop counter.
    int tempTokCtr = 0; //Count number of tokens.

    char buffer[ 10000 ]; //For reading in tokens.
    char temp[ 10000 ]; //Back-up buffer.

    //Prompt user for source program name.
    char sp[50]; //Name of input file.
    printf( "What is the file name for your source program?\n" );
    scanf( "%s", sp );

    //Open file input stream to main input (the program).
    FILE *fin = fopen( sp, "r" );

    //Open file output stream to intermediary output file.
    //In this file, unprocessed tokens will be stored for further processing.
    FILE *temp_fout = fopen( "temp_output.txt", "w" );

    //Procedure for creating intermediary output.
    /** BEGIN PROCEDURE **/
    while( fscanf( fin, "%s", buffer ) != EOF ){

        int len = (int) strlen( buffer ); //Length of read-in token.

        //For the length of the read-in token:
        for( i = 0; i < len; i++ ){

            //If we've reached the end of the buffer, and it's not a special symbol, print the token.
            //If invalid symbols are present, they're ignored and taken care of later.
            if( i == len-1 && !isSpecialSymbol( buffer[ i ] ) ){
                fprintf( temp_fout, "%s ", buffer );
                tempTokCtr++; //Increment the token counter.
            }

            //If the i-th element of the token is a special symbol.
            else if( isSpecialSymbol( buffer[ i ] ) ){

                //Brute force examine particular token cases: comment tokems, not equal, less-than-or-equal, ... , null, and odd.
                if( len > 1 ){
                    if( buffer[ i ] == '/' && buffer[ i+1 ] == '*' ){
                        strncpy( temp, buffer, i );
                        temp[ i ] = '\0';

                        fprintf( temp_fout, "%s ", temp );
                        tempTokCtr++;

                        fprintf( temp_fout, "/* " );
                        tempTokCtr++;

                        strncpy( buffer, &buffer[i]+2, len-1 );
                        buffer[ len - 1 ] = '\0';
                        len = (int) strlen( buffer );
                        i = -1;
                    }
                    else if( buffer[ i ] == '*' && buffer[ i+1 ] == '/' ){
                        strncpy( temp, buffer, i );
                        temp[ i ] = '\0';

                        fprintf( temp_fout, "%s ", temp );
                        tempTokCtr++;

                        fprintf( temp_fout, "*/ " );
                        tempTokCtr++;

                        strncpy( buffer, &buffer[i]+2, len-1 );
                        buffer[ len - 1 ] = '\0';
                        len = (int) strlen( buffer );
                        i = -1;
                    }
                    else if( buffer[ i ] == '!' && buffer[ i+1 ] == '=' ){
                        strncpy( temp, buffer, i );
                        temp[ i ] = '\0';

                        fprintf( temp_fout, "%s ", temp );
                        tempTokCtr++;

                        fprintf( temp_fout, "!= " );
                        tempTokCtr++;

                        strncpy( buffer, &buffer[i]+2, len-1 );
                        buffer[ len - 1 ] = '\0';
                        len = (int) strlen( buffer );
                        i = -1;
                    }
                    else if( buffer[ i ] == '<' && buffer[ i+1 ] == '=' ){
                        strncpy( temp, buffer, i );
                        temp[ i ] = '\0';

                        fprintf( temp_fout, "%s ", temp );
                        tempTokCtr++;

                        fprintf( temp_fout, "<= " );
                        tempTokCtr++;

                        strncpy( buffer, &buffer[i]+2, len-1 );
                        buffer[ len - 1 ] = '\0';
                        len = (int) strlen( buffer );
                        i = -1;
                    }
                    else if( buffer[ i ] == '>' && buffer[ i+1 ] == '=' ){
                        strncpy( temp, buffer, i );
                        temp[ i ] = '\0';

                        fprintf( temp_fout, "%s ", temp );
                        tempTokCtr++;

                        fprintf( temp_fout, ">= " );
                        tempTokCtr++;

                        strncpy( buffer, &buffer[i]+2, len-1 );
                        buffer[ len - 1 ] = '\0';
                        len = (int) strlen( buffer );
                        i = -1;
                    }
                    else if( buffer[ i ] == ':' && buffer[ i+1 ] == '=' ){
                        strncpy( temp, buffer, i );
                        temp[ i ] = '\0';

                        fprintf( temp_fout, "%s ", temp );
                        tempTokCtr++;

                        fprintf( temp_fout, ":= " );
                        tempTokCtr++;

                        strncpy( buffer, &buffer[i]+2, len-1 );
                        buffer[ len - 1 ] = '\0';
                        len = (int) strlen( buffer );
                        i = -1;
                    }
                    else if( len > 3 && buffer[ i ] == 'n' && buffer[ i+1 ] == 'u' && buffer[ i+2 ] == 'l' && buffer[ i+3 ] =='l' ){
                        strncpy( temp, buffer, i );
                        temp[ i ] = '\0';

                        fprintf( temp_fout, "%s ", temp );
                        tempTokCtr++;

                        fprintf( temp_fout, "null " );
                        tempTokCtr++;

                        strncpy( buffer, &buffer[i]+4, len-1 );
                        buffer[ len - 1 ] = '\0';
                        len = (int) strlen( buffer );
                        i = -1;
                    }
                    else if( len > 2 && buffer[ i ] == 'o' && buffer[ i+1 ] == 'd' && buffer[ i+2 ] == 'd' ){
                        strncpy( temp, buffer, i );
                        temp[ i ] = '\0';

                        fprintf( temp_fout, "%s ", temp );
                        tempTokCtr++;

                        fprintf( temp_fout, "odd " );
                        tempTokCtr++;

                        strncpy( buffer, &buffer[i]+3, len-1 );
                        buffer[ len - 1 ] = '\0';
                        len = (int) strlen( buffer );
                        i = -1;
                    }

                }

                //Special symbol token at the beginning.
                if( i == 0 ){
                    //Print symbol as individual token.
                    fprintf( temp_fout, "%c ", buffer[i] );
                    tempTokCtr++; //Increment token counter.

                    //Shift buffer, and update loop index.
                    //Similar procedure used for brute force token examination, and further token examination.
                    strncpy( buffer, &buffer[i]+1, len-1 );
                    buffer[ len - 1 ] = '\0';
                    len = (int) strlen( buffer );
                    i = -1;
                }

                //Valid symbol is splitting tokens within the buffer; split, print, and shift the tokens left of the valid symbol, including the symbol.
                if( i > 0 ){
                    strncpy( temp, buffer, i );
                    temp[ i ] = '\0';

                    fprintf( temp_fout, "%s ", temp );
                    tempTokCtr++;

                    fprintf( temp_fout, "%c ", buffer[i] );
                    tempTokCtr++;

                    strncpy( buffer, &buffer[i]+1, len-1 );
                    buffer[ len - 1 ] = '\0';
                    len = (int) strlen( buffer );
                    i = -1;
                }

            }

        }

    }
    /** END OF PROCEDURE **/

    //Resource management.
    fclose( temp_fout );
    fclose( fin );

    //Open file input stream to intermediary output created by the procedure above (unprocessed tokens).
    fin = fopen( "temp_output.txt", "r" );
    FILE *table = fopen( "lexeme_table.txt", "w" ); //Will hold the lexeme table.
    FILE *list = fopen( "lexeme_list.txt", "w" ); //Will hold the lexeme list.

    //Print header for lexeme table.
    fprintf( table, "Lexeme Table:\n%15s%15s\n", "lexeme", "token type");

    //Procedure to examine printed tokens.

    /** BEGIN PROCEDURE **/

    //Initialize.
    int isResWord = -1; //If currently examined token is a reserved word, != -1; otherwise, = -1.
    int len = 0; //Length of examined token.
    int isComment = 0; //If currently reading in a comment, = 1; otherwise, = 0.

    //For the length of tokens.
    while( fscanf( fin, "%s ", buffer ) != EOF ){

        //fscanf( fin, "%s", buffer ); //Read in token.
        len = (int) strlen( buffer ); //Get token string length.

        //Recognize beginning of comment block.
        if( !strcmp( buffer, "/*" ) ){
            isComment = 1;
        }

        //Recognize end of comment block.
        else if( isComment == 1 && !strcmp( buffer, "*/" ) ){
            isComment = 0;
        }

        //Only read in tokens while not examining a comment block.
        else if( !isComment ){

            //Error detection by token.
            if( detectError( buffer ) ){
                fclose( fin );
                fclose( table );
                fclose( list );
                exit( 0 );
            }

            //Is the current token a reserved word? Print to table and list appropriately.
            isResWord = isReservedWord( buffer );

            if( isResWord != -1 ){

                fprintf( table, "%15s%15d\n", buffer, isResWord );
                fprintf( list, "%d ", isResWord );

            }

            //Is the current token an identifier? Print to table and list appropriately.
            else if( isalpha( buffer[0] ) ){

                fprintf( table, "%15s%15d\n", buffer, 2 );
                fprintf( list, "2 %s ", buffer );

            }

            //Is the current token a number? Print to table and list appropriately.
            else if( isdigit( buffer[0] ) ){

                fprintf( table, "%15s%15d\n", buffer, 3 );
                fprintf( list, "3 %s ", buffer );

            }

            //Otherwise, our token is some other valid symbol (checked for errors above). Print appropriately.
            else{

                fprintf( table, "%15s%15d\n", buffer, isOtherValid( buffer ) );
                fprintf( list, "%d ", isOtherValid( buffer ) );

            }
        }


    }

    //Resource management.
    fclose( fin );
    fclose( table );
    fclose( list );

    /** END OF PROCEDURE **/

    //Procedure to copy and source program.
    /** BEGIN PROCEDURE **/

    fin = fopen( sp, "r" ); //File containing source program.
    FILE *source = fopen( "source_program.txt", "w" ); //File to copy source program to.
    char copy; //Used to copy file, character by character.

    //Copy character by character, until end of file.
    while( ( copy = fgetc( fin ) ) != EOF )
        fputc( copy, source );

    //Resource management.
    fclose( fin );
    fclose( source );
    remove( "temp_output.txt" ); //Delete intermediary output.

    /** END OF PROCEDURE **/

    return 0;

} //End of main.

/**FUNCTION DECLARATIONS**/

int detectError( char *s ){

    int i;
    int len = (int) strlen( s );

    //Check for single colon token.
    if( len == 1 && s[0] == ':' ){
        printf( "Error: Invalid Symbol: :\n" );
        return 1;
    }

    //Check for invalid symbol.
    for( i = 0; i < len; i++ ){

        if( !isalnum(s[i]) && !isSpecialSymbol( s[i] ) ){
            printf( "Error: Invalid Symbol: %c\n", s[i] );
            return 1;
        }
    }

    //Single letter & single number accepted, so if s > 1.
    if( len > 1 ){

        //Check identifier length.
        if( isalpha( s[0] ) ){
            for( i = 1; i < len; i++ ){
                if( i > MAX_ID_LEN ){
                    printf( "Error: Variable name: %s is too long.\n", s );
                    return 1;
                }
            }
        }

        if( isdigit( s[0] ) ){

            //Check invalid identifier (starts with number).
            for( i = 1; i < len; i++ ){

                if( isalpha( s[i] ) ){
                    printf( "Error: Variable name: %s begins with a number.\n", s );
                    return 1;
                }

            }

            //Check number length.
            if( i > MAX_NUM_LEN ){
                printf( "Error: Number %s is too long.\n", s );
                return 1;
            }

        }

    }

    return 0;

} //End of detectError.

int isSpecialSymbol( char c ){

    if( c == '+' )
        return 1;
    else if( c == '-' )
        return 1;
    else if( c == '*' )
        return 1;
    else if( c == '/' )
        return 1;
    else if( c == '(' )
        return 1;
    else if( c == ')' )
        return 1;
    else if( c == '=' )
        return 1;
    else if( c == ',' )
        return 1;
    else if( c == '.' )
        return 1;
    else if( c == '<' )
        return 1;
    else if( c == '>' )
        return 1;
    else if( c == ';' )
        return 1;
    else if( c == ':' )
        return 1;

    return 0;

} //end of isSpecialSymbol.


int isReservedWord( char* s ){

    if( !strcmp( s, "const" ) )
        return 28;
    else if( !strcmp( s, "int" ) )
        return 29;
    else if( !strcmp( s, "procedure" ) )
        return 30;
    else if( !strcmp( s, "call" ) )
        return 27;
    else if( !strcmp( s, "begin" ) )
        return 21;
    else if( !strcmp( s, "end" ) )
        return 22;
    else if( !strcmp( s, "if" ) )
        return 23;
    else if( !strcmp( s, "then" ) )
        return 24;
    else if( !strcmp( s, "else" ) )
        return 33;
    else if( !strcmp( s, "while" ) )
        return 25;
    else if( !strcmp( s, "do" ) )
        return 26;
    else if( !strcmp( s, "read" ) )
        return 32;
    else if( !strcmp( s, "write" ) )
        return 31;

    return -1;

} //End of isReservedWord.

int isOtherValid( char *s ){

    if( !strcmp( s, "null" ) )
        return 1;
    else if( !strcmp( s, "+" ) )
        return 4;
    else if( !strcmp( s, "-" ) )
        return 5;
    else if( !strcmp( s, "*" ) )
        return 6;
    else if( !strcmp( s, "/" ) )
        return 7;
    else if( !strcmp( s, "<=" ) )
        return 12;
    else if( !strcmp( s, ">=" ) )
        return 14;
    else if( !strcmp( s, "(" ) )
        return 15;
    else if( !strcmp( s, ")" ) )
        return 16;
    else if( !strcmp( s, "," ) )
        return 17;
    else if( !strcmp( s, ";" ) )
        return 18;
    else if( !strcmp( s, "." ) )
        return 19;
    else if( !strcmp( s, ":=" ) )
        return 20;
    else if( !strcmp( s, "odd" ) )
        return 8;
    else if( !strcmp( s, "=" ) )
        return 9;
    else if( !strcmp( s, "!=" ) )
        return 10;
    else if( !strcmp( s, "<" ) )
        return 11;
    else if( !strcmp( s, ">" ) )
        return 13;

} //End of isOtherValid.
