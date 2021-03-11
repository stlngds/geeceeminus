%{ 
// Greyson Biggs
// CS 445
// Assignment 2
// parser.y
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "scanType.h"

extern int yylex();
extern FILE *yyin;
extern int yydebug;
extern int line;
extern int numErrors;

#define YYERROR_VERBOSE
void yyerror(const char *msg)
{
    printf("ERROR(%d): %s\n", line, msg);
    numErrors++;
}
%}

%union {
    TokenData *tokenData;
    double value;
}
%token <tokenData> DEC INC MIN MAX NOT AND OR TRUE FALSE IF THEN ELSE BY TO FOR DO WHILE RETURN BREAK BOOL CHAR INT STATIC EQ GEQ LEQ NEQ SINGLEOP ID NUMCONST CHARCONST STRINGCONST BOOLCONST ADDASS SUBASS MULASS DIVASS
%type  <value> tokenlist token

%%
tokenlist     : tokenlist token
              | token 
              ;

token :     
                '\n'            { }
            |   ADDASS          { printf("Line %i Token: ADDASS\n", line); }
            |   SUBASS          { printf("Line %i Token: SUBASS\n", line); }
            |   MULASS          { printf("Line %i Token: MULASS\n", line); }
            |   DIVASS          { printf("Line %i Token: DIVASS\n", line); }
            |   DEC             { printf("Line %i Token: DEC\n", line); }
            |   INC             { printf("Line %i Token: INC\n", line); }
            |   MIN             { printf("Line %i Token: MIN\n", line); }
            |   MAX             { printf("Line %i Token: MAX\n", line); }
            |   NOT             { printf("Line %i Token: NOT\n", line); }
            |   AND             { printf("Line %i Token: AND\n", line); }
            |   OR              { printf("Line %i Token: OR\n", line); }
            |   IF              { printf("Line %i Token: IF\n", line); }
            |   THEN            { printf("Line %i Token: THEN\n", line); }
            |   ELSE            { printf("Line %i Token: ELSE\n", line); }
            |   BY              { printf("Line %i Token: BY\n", line); }
            |   TO              { printf("Line %i Token: TO\n", line); }
            |   FOR             { printf("Line %i Token: FOR\n", line); }
            |   DO              { printf("Line %i Token: DO\n", line); }
            |   WHILE           { printf("Line %i Token: WHILE\n", line); }
            |   RETURN          { printf("Line %i Token: RETURN\n", line); }
            |   BREAK           { printf("Line %i Token: BREAK\n", line); }
            |   BOOL            { printf("Line %i Token: BOOL\n", line); }
            |   CHAR            { printf("Line %i Token: CHAR\n", line); }
            |   INT             { printf("Line %i Token: INT\n", line); }
            |   STATIC          { printf("Line %i Token: STATIC\n", line); }
            |   EQ              { printf("Line %i Token: EQ\n", line); }
            |   GEQ             { printf("Line %i Token: GEQ\n", line); }
            |   LEQ             { printf("Line %i Token: LEQ\n", line); }
            |   NEQ             { printf("Line %i Token: NEQ\n", line); }
            |   SINGLEOP        { printf("Line %i Token: %s\n", line, $1->tokenstr); }
            |   ID              { printf("Line %i Token: ID Value: %s\n", line, $1->tokenstr); }
            |   NUMCONST        { printf("Line %i Token: NUMCONST Value: %i  Input: %s\n", line, $1->nvalue, $1->tokenstr); }
            |   CHARCONST       { printf("Line %i Token: CHARCONST Value: \'%c\'  Input: %s\n", $1->linenum, $1->cvalue, $1->tokenstr); }
            |   STRINGCONST     { printf("Line %i Token: STRINGCONST Value: %s  Len: %i  Input: %s\n", line, $1->svalue, strlen($1->svalue) - 2, $1->tokenstr); }
            |   BOOLCONST       { printf("Line %i Token: BOOLCONST Value: %i Input: %s\n", line, $1->nvalue, $1->tokenstr); }
%%

int main(int argc, char *argv[]) 
{
    if (argc > 1) {
        if ((yyin = fopen(argv[1], "r"))) {
            // file open successful
        }
        else {
            // failed to open file
            printf("ERROR: failed to open \'%s\'\n", argv[1]);
            exit(1);
        }
    }

    yyparse();

    return 0;
}