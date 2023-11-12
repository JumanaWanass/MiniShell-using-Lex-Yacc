%{
#include <string.h>
#include "y.tab.h"
%}

%%

\n  {
    yylval.string_val = strdup(yytext);
    return NEWLINE;
}

[ \t]+  {
    /* Discard spaces and tabs */
}

"&>>" {
    return AMPERSANDAPPEND;
}

"&"    {
    return AMPERSAND;
}

"|"    {
    return PIPE;
}

">>"   {
    return APPEND;
}

">"    {
    return GREAT;
}

"<"    {
    return LESS;
}
[^ \t\n&<>|]+  {
    /* Allow any character except for special characters */
    yylval.string_val = strdup(yytext);
    return WORD;
}


.    {
    /* Invalid character in input */
    return NOTOKEN;
}


%%
int yywrap() {
    return 1;
}
