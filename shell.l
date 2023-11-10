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

"cd"      { return CD; }


[^ \t\n&<>|][^ \t\n&<|]*  {
    /* Assume that file names have only alpha chars */
    yylval.string_val = strdup(yytext);
    return WORD;
}

.    {
    /* Invalid character in input */
    return NOTOKEN;
}


%%
