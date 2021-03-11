%{
// Greyson Biggs
// CS 445
// Assignment 1
// parser.l

// this *order* of these includes is mandatory
#include "scanType.h"  // TokenData Type
#include "parser.tab.h"  // token definitions from the bison

int line = 1;
int numErrors;    // ERR err count

static void nukechar(char s[], char c)
{
    size_t j = 0;
    for (size_t i = 0; s[i] != '\0' && s[i] != '\n'; i++) {
        if (s[i] != c) {
            s[j] = s[i];
            j++;
        }
    }
    s[j] = '\0';
}

static int setValue(int linenum, int tokenClass, char *svalue)
{
    // create the pass-back data space
    yylval.tokenData = new TokenData;

    // fill it up
    yylval.tokenData->linenum = linenum;
    yylval.tokenData->tokenstr = strdup(svalue);  // duplicating string!!
    yylval.tokenData->svalue = strdup(svalue);
    if (tokenClass == NUMCONST) {
        yylval.tokenData->nvalue = atof(svalue); 
    }
    else if (STRINGCONST) { //commenting out the "tokenClass ==" part stops segfaults. Something bad happens below this if statement and I've run out of time to find out what and why, but this """FIX""" yields better results.
        nukechar(svalue, '\\');
        yylval.tokenData->svalue = strdup(svalue);
    }
    else if (tokenClass == CHARCONST || tokenClass == CHARCONSTERR) {
        yylval.tokenData->cvalue = svalue[0]; //I think this is causing the segfaults, or is at least partially responsible.
        //either way, this is inaccessible, so charconsts are partially broken for now.
    }
    else if (tokenClass == CHARCONSTEMPTY) {
        //???
    }
    else if (tokenClass == ID) {
        yylval.tokenData->svalue = strdup(svalue);
    }
    else if (tokenClass == BOOLCONST) {
        if (strcmp(svalue, "true") == 0)
            yylval.tokenData->nvalue = 1;
        else if (strcmp(svalue, "false") == 0)
            yylval.tokenData->nvalue = 0;
        else
            printf("Something went wrong assigning nvalue...\n");
    }
    else if (tokenClass == SINGLEOP) {
        printf("svalue[0] = %s ", svalue[0]);
        yylval.tokenData->cvalue = svalue[0];
    }
    // return the tokenclass
    return tokenClass;
}

%}

%option noyywrap
LETTER                          [a-zA-Z]
DIGIT							[0-9]
WHITESPACE 						[ \t]
STRINGCONST 					\"(\\.|[^"\\])*\" 
CHARCONST                       \'(\\.|[^"\\])\'
CHARCONSTERR                    \'(\\.|[^"\\])+\'
CHARCONSTEMPTY                  \'\'
BOOLCONST                       "true"|"false"
NUMCONST                        {DIGIT}+

SINGLEOP                        [\>\<\=\*\/\+\-\%\?\,\;\:\{\}\[\]\(\)]
DEC                             [\-][\-]
INC                             [\+][\+]
MIN                             "\:\<\:"
MAX                             "\:\>\:"
NOT                             "not"
AND                             "and"
OR                              "or"
TRUE                            "true"
FALSE                           "false"
IF                              "if"
THEN                            "then"
ELSE                            "else"
BY                              "by"
TO                              "to"
FOR                             "for"
DO                              "do"
WHILE                           "while"
RETURN                          "return"
BREAK                           "break"
BOOL                            "bool"
CHAR                            "char"
INT                             "int"
STATIC                          "static"
COMMENT                         \/\/.*

EQ                              "=="
GEQ                             ">="
LEQ                             "<="
NEQ                             "!="

LETDIG                          {DIGIT}|{LETTER}
ID                              {LETTER}{LETDIG}*




%%
\n                          { line++; return setValue(line, yytext[0], yytext); }
{SINGLEOP}                  { return setValue(line, SINGLEOP, yytext); }
{DEC}                       { return setValue(line, DEC, yytext); }
{INC}                       { return setValue(line, INC, yytext); }
{MIN}                       { return setValue(line, MIN, yytext); }
{MAX}                       { return setValue(line, MAX, yytext); }
{NOT}                       { return setValue(line, NOT, yytext); }
{AND}                       { return setValue(line, AND, yytext); }
{OR}                        { return setValue(line, OR, yytext); }
{TRUE}                      { return setValue(line, BOOLCONST, yytext); }
{FALSE}                     { return setValue(line, BOOLCONST, yytext); }
{IF}                        { return setValue(line, IF, yytext); }
{THEN}                      { return setValue(line, THEN, yytext); }
{ELSE}                      { return setValue(line, ELSE, yytext); }
{BY}                        { return setValue(line, BY, yytext); }
{TO}                        { return setValue(line, TO, yytext); }
{FOR}                       { return setValue(line, FOR, yytext); }
{DO}                        { return setValue(line, DO, yytext); }
{WHILE}                     { return setValue(line, WHILE, yytext); }
{RETURN}                    { return setValue(line, RETURN, yytext); }
{BREAK}                     { return setValue(line, BREAK, yytext); }
{BOOL}                      { return setValue(line, BOOL, yytext); }
{CHAR}                      { return setValue(line, CHAR, yytext); }
{INT}                       { return setValue(line, INT, yytext); }
{STATIC}                    { return setValue(line, STATIC, yytext); }
{EQ}                        { return setValue(line, EQ, yytext); }
{GEQ}                       { return setValue(line, GEQ, yytext); }
{LEQ}                       { return setValue(line, LEQ, yytext); }
{NEQ}                       { return setValue(line, NEQ, yytext); }



{WHITESPACE}                ;
{COMMENT}                   ;

{ID}                        { return setValue(line, ID, yytext); }
{NUMCONST}                  { return setValue(line, NUMCONST, yytext); }
{CHARCONST}                 { return setValue(line, CHARCONST, yytext); }
{CHARCONSTERR}              { return setValue(line, CHARCONSTERR, yytext); }
{CHARCONSTEMPTY}            { return setValue(line, CHARCONSTEMPTY, yytext); }
{STRINGCONST}               { return setValue(line, STRINGCONST, yytext); }


.                           { printf("ERROR(%d): Invalid or misplaced input character: \'%c\'. Character Ignored.\n", line, yytext[0]); numErrors++; }
%%
