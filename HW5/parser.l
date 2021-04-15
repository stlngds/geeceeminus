%{
// Greyson Biggs
// CS 445
// 2/18/2021
// parser.l

#include <stdio.h>
#include "scanType.h"  // TokenData Type
#include "parser.tab.h"  // token definitions from bison

int line = 1;
int numErrors = 0;    // ERR err count
int numWarnings = 0;
char *lastToken = NULL;
static void unescape(char s[]);
static int setValue(int linenum, int tokenClass, char *svalue);


static void unescape(char s[])
{
    int size = strlen(s);
    int i, j;
    for (i = 0; s[i] != '\0'; i++) {
        if (s[i] == '\\' && s[i+1] != '\0') {
            switch(s[i+1])
            {
                case 'n':
                    s[i] = '\n';
                    for (j = i; j < size - 1; j++) {
                        s[j+1] = s[j+2];
                    }
                    size--;
                    break;
                case '\\':
                    s[i] = '\\';
                    for (j = i; j < size - 1; j++) {
                        s[j+1] = s[j+2];
                    }
                    size--;

                    break;
                default:
                    for (j = i; j < size - 1; j++) {
                        s[j] = s[j+1];
                    }
                    size--;
                    break; //i++
            }
        }
    }
    s[size] = '\0';
}

static int setValue(int linenum, int tokenClass, char *svalue)
{
    lastToken = strdup(svalue);
    yylval.tokenData = new TokenData;

    yylval.tokenData->linenum = linenum;
    yylval.tokenData->tokenstr = strdup(yytext);  // duplicating string!!
    yylval.tokenData->tokenclass = tokenClass;

    /*
    if ( tokenClass == CHARCONST) {
        printf("Detected CHARCONST in setValue. It\'s value is %s.\n", yytext);
        yylval.tokenData->linenum = linenum;
        printf("Set linenum.\n");
        yylval.tokenData->tokenstr = strdup(yytext);  // duplicating string!!
        printf("Set tokenstr.\n");
        yylval.tokenData->tokenclass = tokenClass;
        printf("Set tokenclass.\n");
    }
    */

    return tokenClass;
}

%}

%option noyywrap
LETTER                          [a-zA-Z]
DIGIT							[0-9]
WHITESPACE 						[ \t]                      
SINGLEOP                        [\[\]\{\}\(\)\;\=\<\>\+\-\*\/\%\?\:\,]
COMMENT                         \/\/.*

LETDIG                          {DIGIT}|{LETTER}
ID                              {LETTER}{LETDIG}*




%%
\n                          { line++; }
"\+\="                      { return setValue(line, ADDASS, yytext); }
"\-\="                      { return setValue(line, SUBASS, yytext); }
[\*][\=]                    { return setValue(line, MULASS, yytext); }
[\/][\=]                    { return setValue(line, DIVASS, yytext); }
{SINGLEOP}                  { return setValue(line, yytext[0], yytext); }
"\-\-"                      { return setValue(line, DEC, yytext); }
"\+\+"                      { return setValue(line, INC, yytext); }
"\:\<\:"                    { return setValue(line, MIN, yytext); }
"\:\>\:"                    { return setValue(line, MAX, yytext); }
"not"                       { return setValue(line, NOT, yytext); }
"and"                       { return setValue(line, AND, yytext); }
"or"                        { return setValue(line, OR, yytext); }
"true"                      { setValue(line, BOOLCONST, yytext); yylval.tokenData->nvalue = 1; return BOOLCONST; }
"false"                     { setValue(line, BOOLCONST, yytext); yylval.tokenData->nvalue = 0; return BOOLCONST; }
"if"                        { return setValue(line, IF, yytext); }
"then"                      { return setValue(line, THEN, yytext); }
"else"                      { return setValue(line, ELSE, yytext); }
"by"                        { return setValue(line, BY, yytext); }
"to"                        { return setValue(line, TO, yytext); }
"for"                       { return setValue(line, FOR, yytext); }
"do"                        { return setValue(line, DO, yytext); }
"while"                     { return setValue(line, WHILE, yytext); }
"return"                    { return setValue(line, RETURN, yytext); }
"break"                     { return setValue(line, BREAK, yytext); }
"bool"                      { return setValue(line, BOOL, yytext); }
"char"                      { return setValue(line, CHAR, yytext); }
"int"                       { return setValue(line, INT, yytext); }
"static"                    { return setValue(line, STATIC, yytext); }
"\=\="                      { return setValue(line, EQ, yytext); }
"\>\="                      { return setValue(line, GEQ, yytext); }
"\<\="                      { return setValue(line, LEQ, yytext); }
"\!\="                      { return setValue(line, NEQ, yytext); }



{WHITESPACE}                {}
{COMMENT}                   {}

{ID}                        { return setValue(line, ID, yytext); }
{DIGIT}+                    { setValue(line, NUMCONST, yytext); yylval.tokenData->nvalue=atoi(yytext); return NUMCONST; }
\'\\.\'                     { setValue(line, CHARCONST, yytext); 
                              switch(yytext[2]){
                                  case '0':
                                      yylval.tokenData->cvalue='\0';
                                      break;
                                  case 'n':
                                      yylval.tokenData->cvalue='\n';
                                      break;
                                  default:
                                      yylval.tokenData->cvalue=yytext[2];
                                      break;
                              }
                              return CHARCONST;
                            }
\'[^'\n]+\'                 { setValue(line, CHARCONST, yytext); 
                              if (strlen(yytext) > 3) {
                                    printf("WARNING(%i): character is %i characters long and not a single character: \'%s\'. The first char will be used.\n", line, strlen(yytext) - 2, yytext); numWarnings++;
                                }
                              yylval.tokenData->cvalue=yytext[1]; 
                              return CHARCONST; 
                            }

\'\'                        { setValue(line, CHARCONST, yytext); 
                              yylval.tokenData->cvalue='\0'; 
                              return CHARCONST; 
                            }

\"(?:\\.|[^"\\\n])*\"       { setValue(line, STRINGCONST, yytext);
                              char *temptext = strdup(yytext);
                              unescape(temptext); 
                              yylval.tokenData->svalue = strdup(temptext);
                              free(temptext);
                              return STRINGCONST; 
                            }

.                           { printf("ERROR(%d): Invalid or misplaced input character: \'%c\'. Character Ignored.\n", line, yytext[0]); numErrors++; }
%%
