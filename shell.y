/*
 * CS-413 Spring 98
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%token	<string_val> WORD
%token 	NOTOKEN GREAT NEWLINE AMPERSAND APPEND REDIRECT PIPE ERR


%union	{
		char   *string_val;
	}

%{
extern "C" 
{
	int yylex();
	void yyerror (char const *s);
}
#define yylex yylex
#include <stdio.h>
#include "command.h"
%}

%%

goal:	
	commands
	;

commands: 
	command
	| commands command 
	;

command: simple_command
        ;


simple_command:	
	command_and_args tokens_list   NEWLINE {
		printf("   Yacc: Execute command\n");
		Command::_currentCommand.execute();
	}
	
	| command_and_args PIPE
	| command_and_args tokens_list PIPE
	| NEWLINE
	| error NEWLINE { yyerrok; }
	;
	
command_and_args:
	command_word arg_list {
		Command::_currentCommand.
			insertSimpleCommand( Command::_currentSimpleCommand );
	}
	;

arg_list:
	arg_list argument
	| /* can be empty */
	;

argument:
	WORD {
               printf("   Yacc: insert argument \"%s\"\n", $1);

	       Command::_currentSimpleCommand->insertArgument( $1 );\
	}
	;

command_word:
	WORD {
               printf("   Yacc: insert command \"%s\"\n", $1);
	       
	       Command::_currentSimpleCommand = new SimpleCommand();
	       Command::_currentSimpleCommand->insertArgument( $1 );
	}
	;


	
	
	
tokens:



         GREAT WORD {
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		
	}
	| APPEND WORD {
		printf("   Yacc: insert output \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentCommand._append = 1;
				
	}
	| REDIRECT WORD {
		printf("   Yacc: insert input \"%s\"\n", $2);
		Command::_currentCommand._inputFile = $2;
	
	}
	| ERR WORD {
		
		printf("   Yacc: append output and error to same file \"%s\"\n", $2);
		Command::_currentCommand._outFile = $2;
		Command::_currentCommand._errFile = $2;
		Command::_currentCommand._append = 1;
		Command::_currentCommand._freeonce = 1;
		//Command::_currentCommand._err = 1;
	}
	
	
	| AMPERSAND {
		Command::_currentCommand._background = 1;
	}
	;
	
tokens_list:
	tokens_list tokens {

	}
	| 
	;



%%

void
yyerror(const char * s)
{
	fprintf(stderr,"%s", s);
}

#if 0
main()
{
	yyparse();
}
#endif
