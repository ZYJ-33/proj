%option noyywrap yylineno

%{
    #include "tree.tab.h"
    #include <stdlib.h>
    
char* cut(char* text)
{
    int len = strlen(text);
    char* new_one = (char*) malloc(len-1);
    for(int i=1; i<len-1; i++)
        new_one[i-1] = text[i];
    new_one[len-2] = 0;
    return new_one;
}
    
%}

%%
"{"     {return LEFT_BRACE;}
"}"     {return RIGHT_BRACE;}
"["     {return LEFT_BRACKET;}
"]"     {return RIGHT_BRACKET;}
":"     {return COLON;}
","     {return COMMA;}
[ ]+    {}
(\n)+   {}
(\t)+   {}
[1-9][0-9]* {yylval.num = atoi(yytext); return NUM;}
0 {yylval.num = 0; return NUM;}
"\""[a-zA-Z0-9_. -]*"\"" {yylval.str = cut(yytext); return STRING;}
"true" {yylval.boolean = 1; return BOOL;}
"false" {yylval.boolean = 0; return BOOL;}
<<EOF>> {return 0;}
%%

void yyerror(char* msg)
{
    printf("%s\n", msg);
    exit(1);
}

/*
int main(int argc, char* argv[])
{
    yyin = fopen(argv[1], "r");
    int sym;
    while((sym = yylex()) != 0)
    {
        printf("%d ", sym);
    }
}
*/
