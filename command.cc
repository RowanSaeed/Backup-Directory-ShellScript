
/*
 * CS354: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#define _DEBUG_LOG	TRUE
#include <time.h>
#include <fstream>
#include <iostream>
#include <fcntl.h>
#include "command.h"



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#include "command.h"



#include <string.h>

#include <fcntl.h>
#include <fcntl.h> // Include this for file operations
#include <errno.h> // Include this for error handl

SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
}

void
SimpleCommand::insertArgument( char * argument )
{
	if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) {
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numberOfAvailableArguments * sizeof( char * ) );
	}
	
	_arguments[ _numberOfArguments ] = argument;

	// Add NULL argument at the end
	_arguments[ _numberOfArguments + 1] = NULL;
	
	_numberOfArguments++;
}

Command::Command()//number of simple commands
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_errFile = 0;
	_append = 0;
	_err = 0;
	
	_freeonce = 0;
	
	
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
	if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) {
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numberOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}
	
	_simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
	_numberOfSimpleCommands++;
}

void
Command:: clear()
{
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}

	if ( _outFile ) {
		free( _outFile );
	}

	if ( _inputFile ) {
		free( _inputFile );
	}

	if ( _errFile ) {
		free( _errFile );
	}

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_errFile = 0;
	_append = 0;
	_err = 0;
	
	_freeonce = 0;
}

void
Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
		}
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
		_inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO");
	printf( "\n\n" );
	
}

void
Command::execute()
{
        int pid;
	int inputfile;
	int outputfile;
	int errorfile;
	int inpfd;
	int outfd;
	int errfd;
	int defaultin;
        int defaultout;
        int defaulterr;
        int fdpipe[2]; 
	// Don't do anything if there are no simple commands
	if ( _numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}


// Print contents of Command data structure
	print();

    
    
    if (_numberOfSimpleCommands >= 1)
		{
			
			int defaultin = dup( 0 ); // Default file Descriptor for stdin
			int defaultout = dup( 1 ); // Default file Descriptor for stdout
			int defaulterr = dup( 2 ); // Default file Descriptor for stderr
			
			
			int fdpipe[2];
			if ( pipe(fdpipe) == -1) {
				perror( "error: pipe");
				exit( 2 );
			}
			if (_inputFile)
			{
				inpfd = open( _inputFile, O_RDONLY, 0666 );
				if ( inpfd < 0 )
				{
					perror( "error : create inputfile" );
					exit( 2 );
				}
			}
			/*else 
			{
				inpfd=defaultin;
			}*/		
			if (_outFile)
			{
				
				if (_append)// >>
				{
					outfd = open( _outFile, O_APPEND | O_CREAT | O_WRONLY, 0666);
				}
				else if(!_append) // >
				{
					outfd = open(_outFile, O_CREAT | O_WRONLY | O_TRUNC,0666);
				}
				
				if ( outfd < 0 ) {
					perror( "error : create outfile" );
					exit( 2 );
				}
			}
			/*else
			{
				outfd=defaultout;
			}*/
			if (_errFile)
			{
				
				if (_append)
				{
					errfd = open( _errFile, O_APPEND | O_CREAT | O_WRONLY, 7777);
				}
				else
				{
					errfd = open(_errFile, O_CREAT | O_WRONLY | O_TRUNC,7777);
				}
				
				if ( errfd < 0 ) {
					perror( "error : create outfile" );
					exit( 2 );
				}
			}
			/*else 
			{
				errfd=defaulterr;
			}*/	
			for (int i = 0 ; i < _numberOfSimpleCommands ; i++)
			{
				
				if ( i == 0 )
				{
				
							// exit
					if(strcmp(_simpleCommands[0]->_arguments[0], "exit") == 0){
						printf("Good Bye !!\n");
						exit(2);
					}
					
								// cd
					if (strcmp(_simpleCommands[0]->_arguments[0], "cd") == 0){
						if(_simpleCommands[0]->_arguments[1] != NULL){
							chdir(_simpleCommands[0]->_arguments[1]);
						}
						else{
							chdir(getenv("HOME"));
						}

						clear();
						prompt();
						return;
					}
					
					
					if (_inputFile)
					{
						
						
						//inputfile=inpfd  redirect 
						dup2(inpfd,0);
						close(inpfd);
					}
					else
					{
						dup2(defaultin,0);
					}		
				
				}
				if (i != 0 )
				{
				
				
					dup2(fdpipe[0],0);
					close(fdpipe[0]);                   
					close(fdpipe[1]);
					  if ( pipe(fdpipe) == -1) {
               					perror( "error: pipe");
               					exit( 2 );
             					}	
				}
				if ( i == _numberOfSimpleCommands - 1 )
				{
					if (_outFile)
					{
						// Create file descriptor 
						dup2(outfd,1);
						close(outfd);
					}
					else
					{
						dup2(defaultout,1);
					}
					if (_errFile)
					{
						// Create file descriptor 
						dup2(errfd,2);
						close(errfd);
					}
					if (!_errFile)
					{
						dup2(defaulterr,2);
					}
				}
				if ( i != _numberOfSimpleCommands - 1 )
				{
					dup2(fdpipe[1],1);
					
				}

			
				
				// Create new process
				pid = fork();
				if ( pid == -1 ) {
					perror( "err : fork\n");
					exit( 2 );
				}

				if (pid == 0) {
					//Child
					
					
					close(inpfd);
					close(outfd);
					close(fdpipe[0]);
					close(fdpipe[1]);
					close( defaultin );
					close( defaultout );
				        close( defaulterr );
					
					
					execvp(_simpleCommands[i]->_arguments[0] ,_simpleCommands[i]->_arguments);
					perror( "err: exec");
					exit( 2 );
				}
				if (pid > 0)
				{
					dup2( defaultin, 0 );
					dup2( defaultout, 1 );
					dup2( defaulterr, 2 );
					if (!_background )
					{
						
						// Wait for last process in the pipe line
						waitpid(pid, 0, 0 );
					}
				}
				
			}
			}
			// Restore input, output, and error

			dup2( defaultin, 0 );
			dup2( defaultout, 1 );
			dup2( defaulterr, 2 );
			
			// close file descriptors that are not needed
			close(fdpipe[0]);
			close(fdpipe[1]);
			close(inpfd);
			close(outfd);
			close(errfd);
			close( defaultin );
			close( defaultout );
			close( defaulterr );
			
			// Restore input, output, and error…
	// Clear to prepare for next command
	clear();
	
	// Print new prompt
	prompt();
}

// Shell implementation

void
Command::prompt()
{

char cwd[1024]; // Assuming a maximum path length of 1024 characters
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("myshell:%s>", cwd);
    } else {
        perror("getcwd");
    }
    fflush(stdout);
	
}


Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;

int yyparse(void);


void handler(int sig)
{
    time_t t = time(NULL);
    pid_t chpid = wait(NULL);
    FILE* log_file = fopen("data.log", "a");  // Open the log file "data.log"
    if (log_file != NULL) {
        fprintf(log_file, "Child pid %d terminated. Current date and time is: %s", chpid, ctime(&t));
        fclose(log_file);
    } else {
        perror("Error opening log file");
    }
}


void ctrlc(int){
	
	
		printf("\n");
		Command::_currentCommand.prompt();	
	
}
int 
main()
{
        signal(SIGINT,ctrlc);
	//signal(SIGINT, SIG_IGN);  //ctrl-c
	signal (SIGCHLD, handler); //log file
	Command::_currentCommand.prompt();
	yyparse();
	return 0;
}

