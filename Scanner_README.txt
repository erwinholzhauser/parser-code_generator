
Description: Implementation of a lexical analyzer for the programming language PL/0. Capable of reading a PL/0 source program, identifying some errors, and producing as output: the source program, and a lexeme table and list for the source program. The source code for the lexical analyzer is contained within a single module, LexicalAnalyzer.c

Input: File containing PL/0 program, with the following constraints:
       (1) Max length of identifiers is 11 characters.
       (2) Max length of numbers is 5 digits in length.
       (3) Comments and invisible characters are ignored.
       (4) Does not have to be grammatically valid PL/0 code.
       (5) Labeled "input.txt".
       (6) The null and odd symbols are "null" and "odd", respectively.

Output: (1) source_program.txt : Copy of input file.
        (2) lexeme_table.txt : Lexeme table for source program.
        (3) lexeme_list.txt : Lexeme list for source program.
	(4) temp_output.txt : Intermediary output consisting of preprocessed tokens. It is created and deleted by the program.
	(5) Until an error is reached, output is printed to files as normal. Then, the program prints one of the appropriate error messages: "Error: Variable name <token> is too long.", "Error: Variable name <token> begins with a number.", "Error: Number <token> is too long.", or "Error: Invalid symbol: <token>", to the standard output, and terminates.

Compile & Run:

Using your Eustis account, compile "LexicalAnalyzer.c" on the UCF Eustis server, and run the corresponding executable file.

Information on Eustis and UNIX Commands can be found on the following web page: http://www.cs.ucf.edu/courses/cop3402/spr2014/assignments.html , contained in the downloadable document "Eustis Tutorial".

Ensure that all contents of the program: "input.txt", the source code "LexicalAnalyzer.c", the object code, and the executable file, are in the same directory.

The corresponding output files, detailed above, will be created in the same directory.

