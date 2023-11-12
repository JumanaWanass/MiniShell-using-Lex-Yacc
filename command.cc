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

SimpleCommand::SimpleCommand()
{
    // Creat available space for 5 arguments
    _numberOfAvailableArguments = 5;
    _numberOfArguments = 0;
    _arguments = (char **)malloc(_numberOfAvailableArguments * sizeof(char *));
}

void SimpleCommand::insertArgument(char *argument)
{
    if (_numberOfAvailableArguments == _numberOfArguments + 1)
    {
        // Double the available space
        _numberOfAvailableArguments *= 2;
        _arguments = (char **)realloc(_arguments,
                                      _numberOfAvailableArguments * sizeof(char *));
    }

    _arguments[_numberOfArguments] = argument;

    // Add NULL argument at the end
    _arguments[_numberOfArguments + 1] = NULL;

    _numberOfArguments++;
}

Command::Command()
{
    // Create available space for one simple command
    _numberOfAvailableSimpleCommands = 1;
    _simpleCommands = (SimpleCommand **)malloc(_numberOfSimpleCommands * sizeof(SimpleCommand *));

    _numberOfSimpleCommands = 0;
    _outFile = 0;
    _inputFile = 0;
    _errFile = 0;
    _appendFile = 0;
    _append = 0; // Initialize _append to 0
    _background = 0;
    _logFile = 0;
}

bool Command::isExitCommand()
{
    if (_numberOfSimpleCommands == 1 && _simpleCommands[0]->_numberOfArguments == 1 &&
        strcmp(_simpleCommands[0]->_arguments[0], "exit") == 0)
    {
        _currentCommand.shouldExit = true;
        return true;
    }
    return false;
}

// Initialize shouldExit in command.cc
bool Command::shouldExit = false;

void Command::insertSimpleCommand(SimpleCommand *simpleCommand)
{
    if (_numberOfAvailableSimpleCommands == _numberOfSimpleCommands)
    {
        _numberOfAvailableSimpleCommands *= 2;
        _simpleCommands = (SimpleCommand **)realloc(_simpleCommands,
                                                    _numberOfAvailableSimpleCommands * sizeof(SimpleCommand *));
    }

    _simpleCommands[_numberOfSimpleCommands] = simpleCommand;
    _numberOfSimpleCommands++;
}

void Command::clear()
{
    for (int i = 0; i < _numberOfSimpleCommands; i++)
    {
        for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++)
        {
            free(_simpleCommands[i]->_arguments[j]);
        }

        free(_simpleCommands[i]->_arguments);
        free(_simpleCommands[i]);
    }

    if (_outFile)
    {
        free(_outFile);
    }

    if (_inputFile)
    {
        free(_inputFile);
    }

    if (_errFile)
    {
        free(_errFile);
    }

    if (_appendFile)
    {
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

void handleCtrlC(int signo)
{
    if (Command::_currentCommand.isExitCommand())
    {
        printf("\nExiting the shell...\n");
        exit(0);
    }
    else
    {
        printf("\nCtrl-C is ignored in this shell!\n");
        Command::_currentCommand.prompt();
        fflush(stdout);
    }
}

void handleSIGCHLD(int signo)
{
    int status;
    pid_t childPid;

    // Reap all terminated child processes
    while ((childPid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        // Log the termination of the child process to shell.log
        FILE *logFile = fopen("shell.log", "a");
        if (logFile != nullptr)
        {
            fprintf(logFile, "Child process %d terminated with status %d\n", childPid, status);
            fclose(logFile);
        }
        else
        {
            perror("Failed to open shell.log for writing");
        }
    }
}

void setupSignalHandler()
{
    struct sigaction sa;
    sa.sa_handler = handleCtrlC;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    // Setup SIGCHLD handler
    sa.sa_handler = handleSIGCHLD;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);
}

void Command::print()
{
    printf("\n\n");
    printf("              COMMAND TABLE                \n");
    printf("\n");
    printf("  #   Simple Commands\n");
    printf("  --- ----------------------------------------------------------\n");

    for (int i = 0; i < _numberOfSimpleCommands; i++)
    {
        printf("  %-3d ", i);
        for (int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++)
        {
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

bool Command::changeDirectory(char *directory)
{
    // If no directory is specified, change to the home directory
    if (directory == nullptr || strlen(directory) == 0)
    {
        const char *homeDir = getenv("HOME");
        if (homeDir != nullptr)
        {
            return chdir(homeDir) == 0;
        }
    }
    else
    {
        // Try changing the directory in the parent process
        if (chdir(directory) == 0)
        {
            // If successful, update the prompt
            prompt();
            return true;
        }
    }

    return false; // Return false if the directory change fails
}

void Command::execute()
{
    if (isExitCommand())
    {
        printf("Exiting the shell...\n");
        exit(0);
    }

    // Don't do anything if there are no simple commands
    if (_numberOfSimpleCommands == 0)
    {
        prompt();
        return;
    }

    print();
    int defaultIn = dup(0);
    int defaultOut = dup(1);

    int ip = -1, op = -1, err = -1;
    if (_errFile)
    {
        err = open(_errFile, O_WRONLY | O_CREAT, 0777);
        dup2(err, 2);
    }
    if (_inputFile)
    {
        ip = open(_inputFile, O_RDONLY, 0777);
        if (ip == -1)
        {
            perror("Failed to open input file");
            exit(1);
        }
    }
    if (_outFile)
    {
        if (!_append)
            op = open(_outFile, O_WRONLY | O_CREAT, 0777);
        else
            op = open(_outFile, O_WRONLY | O_APPEND, 0777);
        if (op == -1)
        {
            perror("Failed to open output file");
            exit(1);
        }
    }

     if (_logFile)
    {
        if (!_append)
            op = open(_logFile, O_WRONLY | O_CREAT, 0777);
        else
            op = open(_logFile, O_WRONLY | O_APPEND, 0777);
        if (op == -1)
        {
            perror("Failed to open log file");
            exit(1);
        }
    }

    int fd[2];        // 1D array for a single pipe
    int prev_fd = 0;  // Store the previous file descriptor for piping
    int pid;         // Store the PID of the child process

    // Check if it's the cd command
    if (_numberOfSimpleCommands == 1 && _simpleCommands[0]->_numberOfArguments == 1 &&
        strcmp(_simpleCommands[0]->_arguments[0], "cd") == 0)
    {
        // If it's the cd command, change the directory
        char *directory = (_simpleCommands[0]->_numberOfArguments == 2) ? _simpleCommands[0]->_arguments[1] : nullptr;
        if (changeDirectory(directory))
        {
            // Print new prompt if the directory change is successful
            prompt();
            return;
        }
        else
        {
            // Print an error message if the directory change fails
            std::cerr << "Failed to change directory." << std::endl;
            prompt();
            return;
        }
    }

    // Print contents of the Command data structure
    print();
    if (_background)
    {
        backgroundCommandsCount++;
    }

    for (int i = 0; i < _numberOfSimpleCommands; i++)
    {       
        if (i < _numberOfSimpleCommands - 1)
        {
            // Create pipe for all commands except the last one
            if (pipe(fd) == -1)
            {
                perror("Failed to create pipe");
                exit(1);
            }
        }

        pid = fork();
        if (pid == 0)
        {
            if (_numberOfSimpleCommands > 1)
            {
                if (i == 0)
                {
                    dup2(fd[1], 1); // Redirect standard output to the input of the next command
                    close(fd[0]);
                }
                else if (i == _numberOfSimpleCommands - 1)
                {
                    dup2(prev_fd, 0); // Redirect standard input to the output of the previous command
                }
                else
                {
                    dup2(prev_fd, 0); // Redirect standard input to the output of the previous command
                    dup2(fd[1], 1);   // Redirect standard output to the input of the next command
                    close(prev_fd);
                    close(fd[0]);
                }
            }

            if (_inputFile && i == 0)
            {
                dup2(ip, 0);
                close(ip);
            }
            if (_outFile && i == _numberOfSimpleCommands - 1)
            {
                dup2(op, 1);
            }
            if (_errFile)
            {
                dup2(err, 2);
            }
     
         
            // Execute the command
            execvp(_simpleCommands[i]->_arguments[0], _simpleCommands[i]->_arguments);
            // If execvp returns, there was an error
            //perror("Failed to execute command");
            exit(1);
        }
        else if (pid < 0)
        {
            perror("Failed to fork");
            exit(1);
        }
         
         
         else{

        if (_numberOfSimpleCommands > 1)
        {   
            close(fd[1]);
            prev_fd = fd[0]; // Store the current file descriptor to be used for the next iteration
        }
        }}

    if (!_background)
    {
        int status;
        pid_t lastChild;
        while ((lastChild = waitpid(-1, &status, 0)) > 0)
        {
            FILE *logFile = fopen("shell.log", "a");
            if (logFile != nullptr)
            {
                if (WIFEXITED(status))
                {
                    fprintf(logFile, "Child process %d terminated normally with status %d\n", lastChild, WEXITSTATUS(status));
                }
                else if (WIFSIGNALED(status))
                {
                    fprintf(logFile, "Child process %d terminated by signal %d\n", lastChild, WTERMSIG(status));
                }
                else
                {
                    fprintf(logFile, "Child process %d terminated abnormally\n", lastChild);
                }

                fclose(logFile);
            }
            else
            {
                perror("Failed to open shell.log for writing");
            }
        }
    }

    // Clear for the next command
    clear();

    // Print new prompt
    prompt();
}

// Shell implementation

void Command::prompt()
{
    char currentDirectory[4096]; // adjust the size as needed
    if (getcwd(currentDirectory, sizeof(currentDirectory)) != NULL)
    {
        printf("myshell:%s>", currentDirectory);
    }
    else
    {
        perror("getcwd() error");
        printf("myshell>");
    }
    fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand *Command::_currentSimpleCommand;

int yyparse(void);

int main()
{
    // Setup signal handlers
    setupSignalHandler();

    // Start the shell
    Command::_currentCommand.prompt();
    yyparse();

    return 0;
}
