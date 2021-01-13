%{
/* Greyson Biggs - 2/24/2020 - CS 210 (Jeffery) */
/* This JSON scanner is functional but technically incomplete, such as for implementing HEX */
/* Modified for use with json.y */

#include <stdio.h>
#include "json.tab.h"
#include "tree.c"
%}

%option yylineno
%option noyywrap

DIGIT							[0-9]
HEX								[0-9a-fA-F]
WHITESPACE 						[ \t\n\r\u\f\b]+
SIGN							+|-
EXPONENT						[eE][-+]*{DIGIT}+
FRACTION						"."{DIGIT}+
NUMBER							"-"?{DIGIT}+{FRACTION}*{EXPONENT}*
STRING 							\"(\\.|[^"\\])*\"

%%

"true"									{yylval.tn = leaf(TRUE, yytext); return TRUE;}
"false"									{yylval.tn = leaf(FALSE, yytext); return FALSE;}
"null"									{yylval.tn = leaf(null, yytext); return null;}
{WHITESPACE}+							{}
{STRING}								{yylval.tn = leaf(STRINGLIT, yytext); return STRINGLIT;}
{NUMBER}								{yylval.tn = leaf(NUMBER, yytext); return NUMBER; }
"{"										{yylval.tn = leaf('{', yytext); return LCURLY; }
"}"										{yylval.tn = leaf('}', yytext); return RCURLY; }
","										{yylval.tn = leaf(',', yytext); return COMMA; }
":"										{yylval.tn = leaf(':', yytext); return COLON; }
"["										{yylval.tn = leaf('[', yytext); return LBRACKET; }
"]"										{yylval.tn = leaf(']', yytext); return RBRACKET; }
.										{ printf("lexical error on line %d\n", yylineno); }

%%
