	/*
	* CS354: Shell project
	*
	* Template file.
	* You will need to add more code here to execute the command table.
	*
	* NOTE: You are responsible for fixing any bugs this code may have!
	*
	*/
       #include <unistd.h>
       #include <fcntl.h>
       #include <sys/stat.h>
       #include <iostream>
       #include <cerrno> // Add this line to handle errno
#include "command.h"


	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/wait.h>
	#include <string.h>
	#include <signal.h>

	#include "command.h"
	static int backgroundCommandsCount = 0;

	SimpleCommand::SimpleCommand() {
		// Creat available space for 5 arguments
		_numberOfAvailableArguments = 5;
		_numberOfArguments = 0;
		_arguments = (char **) malloc(_numberOfAvailableArguments * sizeof(char *));
	}

	void SimpleCommand::insertArgument(char * argument) {
		if (_numberOfAvailableArguments == _numberOfArguments + 1) {
			// Double the available space
			_numberOfAvailableArguments *= 2;
			_arguments = (char **) realloc(_arguments,
										_numberOfAvailableArguments * sizeof(char *));
		}

		_arguments[_numberOfArguments] = argument;

		// Add NULL argument at the end
		_arguments[_numberOfArguments + 1] = NULL;

		_numberOfArguments++;
	}

	Command::Command() {
		// Create available space for one simple command
		_numberOfAvailableSimpleCommands = 1;
		_simpleCommands = (SimpleCommand **) malloc(_numberOfSimpleCommands * sizeof(SimpleCommand *));

		_numberOfSimpleCommands = 0;
		_outFile = 0;
		_inputFile = 0;
		_errFile = 0;
		_appendFile = 0;
		_append = 0; // Initialize _append to 0
		_background = 0;
	}

	void Command::insertSimpleCommand(SimpleCommand * simpleCommand) {
		if (_numberOfAvailableSimpleCommands == _numberOfSimpleCommands) {
			_numberOfAvailableSimpleCommands *= 2;
			_simpleCommands = (SimpleCommand **) realloc(_simpleCommands,
														_numberOfAvailableSimpleCommands * sizeof(SimpleCommand *));
		}

		_simpleCommands[_numberOfSimpleCommands] = simpleCommand;
		_numberOfSimpleCommands++;
	}

	void Command::clear() {
		for (int i = 0; i < _numberOfSimpleCommands; i++) {
			for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++) {
				free(_simpleCommands[i]->_arguments[j]);
			}

			free(_simpleCommands[i]->_arguments);
			free(_simpleCommands[i]);
		}

		if (_outFile) {
			free(_outFile);
		}

		if (_inputFile) {
			free(_inputFile);
		}

		if (_errFile) {
			free(_errFile);
		}

		if (_appendFile) {
			free(_appendFile);
		}
		_numberOfSimpleCommands = 0;
		_outFile = 0;
		_inputFile = 0;
		_errFile = 0;
		_appendFile = 0;
		_append = 0; // Reset _append
		_background = 0;
	}

	void Command::print() {
		printf("\n\n");
		printf("              COMMAND TABLE                \n");
		printf("\n");
		printf("  #   Simple Commands\n");
		printf("  --- ----------------------------------------------------------\n");

		for (int i = 0; i < _numberOfSimpleCommands; i++) {
			printf("  %-3d ", i);
			for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++) {
				printf("\"%s\" \t", _simpleCommands[i]->_arguments[j]);
			}
		}

		printf("\n\n");
		printf("  Output       Input        Error        Background\n");
		printf("  ------------ ------------ ------------ ------------\n");
		printf("  %-12s %-12s %-12s %-12s\n", _outFile ? _outFile : "default",
			_inputFile ? _inputFile : "default", _errFile ? _errFile : "default",
			_background ? "YES" : "NO");
		printf("\n\n");

	}
	

void Command::execute() {
    // Don't do anything if there are no simple commands
    if (_numberOfSimpleCommands == 0) {
        prompt();
        return;
    }

    // Print contents of the Command data structure
    print();
    if (_background) {
        backgroundCommandsCount++;
    }

    int prevPipe[2]; // File descriptors for the previous pipe
    int currPipe[2]; // File descriptors for the current pipe
    int prevRead = -1; // File descriptor for the previous read end of the pipe
 
    for (int i = 0; i < _numberOfSimpleCommands; ++i) {
        // Create a new pipe
        if (pipe(currPipe) < 0) {
            std::cerr << "Failed to create pipe." << std::endl;
            exit(1);
        }

        pid_t pid = fork(); // Create a child process
        if (pid < 0) {
            std::cerr << "Failed to fork." << std::endl;
            exit(1);
        }

        if (pid == 0) { // Child process
            // Set up input redirection from the previous command's pipe
            if (prevRead != -1) {
                if (dup2(prevRead, STDIN_FILENO) < 0) {
                    std::cerr << "Failed to redirect input." << std::endl;
                    exit(1);
                }
                close(prevRead);
            }

            // Set up output redirection to the current command's pipe
            if (i < _numberOfSimpleCommands - 1) {
                if (dup2(currPipe[1], STDOUT_FILENO) < 0) {
                    std::cerr << "Failed to redirect output." << std::endl;
                    exit(1);
                }
                close(currPipe[1]);
            }

            // Execute the simple command using execvp
            char **args = _simpleCommands[i]->_arguments;
            execvp(args[0], args);

            // If execvp fails, print an error message
            perror("Failed to execute command");
            exit(1);
        } else { // Parent process
            // Close unnecessary file descriptors
            if (prevRead != -1) {
                close(prevRead);
            }
            close(currPipe[1]);

            // Store the current pipe's read end for the next iteration
            prevRead = currPipe[0];

            // Wait for the child process to finish
            int status;
            waitpid(pid, &status, 0);
        }
    }
    pid_t pid = fork();
 
    if (!_background) {
        int status;
        pid_t lastChild;
        while ((lastChild = waitpid(pid, &status, 0)) > 0)  {
            // Handle the termination of child processes
        }
    }


    // Clear for the next command
    clear();

    // Print new prompt
    prompt();
}
    



	// Shell implementation

	void Command::prompt() {
		printf("myshell>");
		fflush(stdout);
	}

	Command Command::_currentCommand;
	SimpleCommand * Command::_currentSimpleCommand;

	int yyparse(void);

	int 
	main()
	{
		Command::_currentCommand.prompt();
		yyparse();
		return 0;
	}
