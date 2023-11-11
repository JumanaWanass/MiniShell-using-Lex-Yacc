%{
#include <stdio.h>
#include <string.h>
#include "command.h"

extern "C" {
    int yylex();
    void yyerror(const char *s);
}

#define yylex yylex
%}

%token <string_val> WORD
%token NOTOKEN GREAT NEWLINE AMPERSAND PIPE APPEND SMALL AMPERSANDAPPEND LESS CD

%union {
    char *string_val;
}

%%

goal:	
    commands
    ;

commands: 
    command
    | commands command 
    ;

command: 
    complex_command
    | simple_command
    | NEWLINE
    | error NEWLINE { yyerrok; }
    ;

complex_command:
    command_and_args piped_list iomodifier_list ampersandmodifier NEWLINE {
        printf("   Yacc: Execute complex command in background\n");
        Command::_currentCommand.execute();
    }
    | command_and_args iomodifier_list ampersandmodifier NEWLINE {
        printf("   Yacc: Execute complex command in background\n");
        Command::_currentCommand.execute();
    }
    | command_and_args piped_list iomodifier_list NEWLINE {
        printf("   Yacc: Execute complex command\n");
        Command::_currentCommand.execute();
    }
    | command_and_args iomodifier_list NEWLINE {
        printf("   Yacc: Execute complex command\n");
        Command::_currentCommand.execute();
    }
    ;

simple_command:
    command_and_args iomodifier_opt NEWLINE {
        printf("   Yacc: Execute command\n");
        Command::_currentCommand.execute();
    }
    | CD iomodifier_opt NEWLINE {
        printf("   Yacc: Execute cd command\n");
        Command::_currentCommand.execute();
    }
    | NEWLINE 
    | error NEWLINE { yyerrok; }
    ;

cd_command:
    CD iomodifier_opt NEWLINE {
        printf("   Yacc: Execute cd command\n");
        Command::_currentCommand.execute();
    }
    ;




piped_list: 
    piped_list piped_command
    | /* can be empty */
    ;

piped_command:
    PIPE command_and_args
    ;

command_and_args:
    command_word arg_list {
        Command::_currentCommand.insertSimpleCommand(Command::_currentSimpleCommand);
    }
    ;

arg_list:
    arg_list argument
    | /* can be empty */
    ;

argument:
    WORD {
        printf("   Yacc: insert argument \"%s\"\n", $1);
        Command::_currentSimpleCommand->insertArgument($1);
    }
    ;

command_word:
    WORD {
        printf("   Yacc: insert command \"%s\"\n", $1);
        Command::_currentSimpleCommand = new SimpleCommand();
        Command::_currentSimpleCommand->insertArgument($1);
    }
    ;

iomodifier_list:
    iomodifier_list iomodifier
    | /* can be empty */
    ;
    
iomodifier:
    iomodifier_opt
    | ampersandappendmodifier
    | iomodifier_ipt
    | appendmodifier
    ;  
    
iomodifier_opt:
    GREAT WORD {
        printf("   Yacc: insert output \"%s\"\n", $2);
        Command::_currentCommand._outFile = $2;
    }
    | /* can be empty */ 
    ;

iomodifier_ipt:
    LESS WORD {
        printf("   Yacc: insert input \"%s\"\n", $2);
        Command::_currentCommand._inputFile = $2;
    }
    | /* can be empty */ 
    ;

ampersandappendmodifier:
    AMPERSANDAPPEND WORD{
        printf("   Yacc: insert ampersandappend \"%s\"\n", $2);
        Command::_currentCommand._appendFile = $2;
        Command::_currentCommand._errFile = $2;
    } 
    | /* can be empty */ 
    ;

appendmodifier:
    APPEND WORD {
        printf("   Yacc: insert append \"%s\"\n", $2);
        Command::_currentCommand._appendFile = $2;
    }
    | /* can be empty */ 
    ;

ampersandmodifier:
    AMPERSAND {
        printf("   Yacc: running in the background\n");
        Command::_currentCommand._background = 1;
    }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "%s", s);
}
