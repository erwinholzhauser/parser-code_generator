Table of Contents:
	I.	Description
	II.	Input
	III.	Output
	IV.	Compile & Run

I. Description:

Implementation of a compiler for the "tiny" PL/0 programming language subset. Its components are: a scanner (lexical analyzer), a recursive descent parser and interwoven code-generator, a virtual machine (p-machine) to run the generated assembly code, and a compile driver to manage these.

II. Input: 
	File containing PL/0 program, with the following constraints:
	(1) Max length of identifiers is 11 characters.
	(2) Max length of numbers is 5 digits in length.
	(3) Comments and invisible characters are ignored.
	(4) Does not have to be grammatically valid PL/0 code.
	(6) The null and odd symbols are "null" and "odd", respectively.
	(7) You will specify the name for this file. See "Compile & Run".

	Also printed to the console, prefaced by "In: <newline>".
	
	"in.txt", which will contain all the user-input integers for read operations, separated by white space. For example, if a user wants to sequentially input 1, 2, and 3 for three read operations, the file would simply contain whitespace-separated 1, 2, and 3.

III. Output:

	output3.txt, which will contain the output of write operations.
	
	Also printed to the console, prefaced by "Out: <newline>".

Based on directives:

Directive "-l" :
	source_program.txt, a copy of the input PL/0 source program.
	lexeme_table.txt, a lexeme table for the source program.
	lexeme_list.txt, a lexeme list for the source program.

	Also printed to the console, prefaced by "Source Program: <newline>", "Lexeme Table: <newline>", and "Lexeme List: <newline>", respectively..

Directive "-a" "
	assembly.txt, the generated assembly code for the P-machine.

	Also printed to the console, prefaced by "Generated Assembly Code: <newline>".

Directive "-v" :
	output1.txt, which will contain a print out of the source program in interpreted assembly language, with line numbers.
	output2.txt, which will contain the print-out of the program execution in the virtual machine, showing the stack, PC, BP, and SP.

	Also printed to the console, prefaced by "Assembly Source Program: <newline>" and 
Program Execution in VM: <newline>", respectively.

IV. Compile & Run:

Note: Information on Eustis and UNIX Commands can be found on the following web page: http://www.cs.ucf.edu/courses/cop3402/spr2014/assignments.html , contained in the downloadable document "Eustis Tutorial".

Ensure all the contents of the program: your plain text source program file and the source code files ( LexicalAnalyzer.c, Parser-Code_Generator.c, pmachine.c, and compile_driver.c ) are in the same directory.

Using your Eustis account, compile "LexicalAnalyzer.c", "Parser-Code_Generator.c", "pmachine.c", and "compile_driver.c" on the UCF Eustis server. The executable file for each component should be named the same as its source code file. For example, for "LexicalAnalyzer.c", we would do:

	gcc LexicalAnalyzer.c -o LexicalAnalyzer

Ensure that the generated object code files and executable files are in the same directory as the prior.

To run the compiler, we would execute the compile driver in the command line as follows:

	./compile_driver <directive> …

Supported directives are:
	-l , to print the scanner output.
	-a , to print the parser and code generation output.
	-v , to print VM output.

The compiler will prompt your for a "tiny" PL/0 source program, as follows:

	What is the file name for your source program?

After which, you should specify the input file name of the form:

	<filename>.txt

If the scanner and parser find no lexical or syntactical error with your source program, you will get the following message to the console:
	
	No errors, program is syntactically correct

Otherwise, you will get an error message to the console of the form:

	Error: … or *****Error number <number>, … 

Depending on the directives used, the appropriate output files will be created to the same directory as the compile driver.