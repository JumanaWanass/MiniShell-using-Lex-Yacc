
# Current errors:
To download the repo on your local machine directly, run the following command:
`git clone https://github.com/JumanaWanass/MiniShell-using-Lex-Yacc.git`
## Part 3:
When running the command: `cd`
It doesn't work. I tried treating it as a token rather than a built in function, and modified the lex and yacc file accordingly.. however it didn't work.

### What's left to do:
1. Fix the cd to make it work. I already modified the code to write the directory the shell is in so we can know if it works.
2. Extend lex to support any character in the arguments that is not a special character
such as "&", ">", "<", "|" etc. Also, your shell should allow no spaces between "|", ">"
etc. For example, "ls|grep a" without spaces after "ls" and before "grep" should work.
3. It’s required to create a log file that contains Logs when every child is terminated you
can use SIGCHLD signal to do so.

### Bonus part:
do the wildcarding. The wildcarding will work in the same way that it works in shells
like csh. The "*" character matches 0 or more nonspace characters. The "?" character
matches one nonspace character. The shell will expand the wildcards to the file names
that match the wildcard where each matched file name will be an argument.
