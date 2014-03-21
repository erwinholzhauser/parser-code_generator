/**********************************************************************************************
 * Homework #3B (Compile Driver)
 *
 * Programmer: Erwin Holzhauser
 *
 * Due Date: March 24, 2014, @ 11:59 PM
 *
 * COP3402-0001, Spring 2014       Instructor: Euripides Montagne
 *
 * Description: Manages the parts of the compiler (scanner, parser, code generator, and VM).
 *              It handles the input, out, and execution
 *
 **********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Struct for tracking value of valid directives.
typedef struct{
    int l;
    int a;
    int v;
} directives;

//Global variable instantiation of directives struct.
directives d;

/*
 * Post-conditions: Check if directives are valid to the compiler,
 *                  and set appropriate flags in d.
 */
void parseDirectives( int argc, char **argv ){

    int i;

    //Check all directives, and set appropriate flags.
    //When invalid directive is given, report in standard output.
    for( i = 1; i < argc; i++ ){

        if( strcmp( argv[i], "-l" ) == 0 ){
            d.l = 1;
        }

        else if( strcmp( argv[i], "-a" ) == 0 ){
            d.a = 1;
        }

        else if( strcmp( argv[i], "-v" ) == 0 ){
            d.v = 1;
        }

        else{
            printf( "Bad directive: %s\n", argv[i] );
            exit( -1 );
        }

    }
}

int main( int argc, char **argv ){

    //Will hold return value of compiler components.
    int ret;

    //initialize directives ds.
    d.l = d.a = d.v = 0;

    //Parse directives.
    parseDirectives( argc, argv );

    /*
     * Run scanner, parser, code generator, and VM components.
     * For each:
     * If the component didn't finish successfully, delete associated files based on
     * directives, and exit.
     */

    //Scanner.
    ret = system( "./LexicalAnalyzer" );

    if( ret < 0 || WEXITSTATUS(ret) != EXIT_SUCCESS ){

        if( d.l == 0 ){
            remove( "source_program.txt" );
            remove( "lexeme_table.txt" );
            remove( "lexeme_list.txt" );
        }

        exit(EXIT_FAILURE);
    }

    //Parser / Code Generator.
    ret = system( "./Parser-Code_Generator" );

    if( ret < 0 || WEXITSTATUS(ret) != EXIT_SUCCESS ){

        if( d.a == 0 ){
            remove( "assembly.txt" );
        }

        exit(EXIT_FAILURE);

    }

    //VM.
    ret = system( "./pmachine" );

    if( ret < 0 || WEXITSTATUS(ret) != EXIT_SUCCESS ){

        if( d.v == 0 ){
            remove( "output1.txt" );
            remove( "output2.txt" );
            remove( "output3.txt" );
        }

        exit(EXIT_FAILURE);

    }

    //Handle directives.
    if( d.l == 0 ){
        remove( "source_program.txt" );
        remove( "lexeme_table.txt" );
        remove( "lexeme_list.txt" );
    }

    if( d.a == 0 )
        remove( "assembly.txt" );

    if( d.v == 0 ){
        remove( "output1.txt" );
        remove( "output2.txt" );
    }

    //Exit successfully!
    exit( EXIT_SUCCESS );

    return 0;
}
