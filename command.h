#ifndef command_h
#define command_h
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
// Command Data Structure
struct SimpleCommand
{
    // Available space for arguments currently preallocated
    int _numberOfAvailableArguments;

    // Number of arguments
    int _numberOfArguments;
    char **_arguments;

    SimpleCommand();
    void insertArgument(char *argument);
};

struct Command
{
    int _numberOfAvailableSimpleCommands;
    int _numberOfSimpleCommands;
    SimpleCommand **_simpleCommands;
    char *_outFile;
    char *_inputFile;
    char *_errFile;
    char *_appendFile;
    int _append;
    int _background;
    char *_logFile;
    

    void prompt();
    void print();
    Command();

    void execute();
    void clear();
    bool changeDirectory(char *directory);
    bool isExitCommand();
    static bool shouldExit;  // Add this line    Command();
    void insertSimpleCommand(SimpleCommand *simpleCommand);
    void add_dir_to_path(char *directory);
    int changeCurrentDirectory();
    void ChangeDir(char* dir);


    static Command _currentCommand;
    static SimpleCommand *_currentSimpleCommand;
};

#endif
