%{
/* Greyson Biggs - 2/24/2020 - CS 210 (Jeffery) */
/* Creates a parse tree with Bison */
/*  */

#include "json.tab.h"
#include "tree.h"
#include "prodrules.h"
#include <stdio.h>
%}

%union {struct node *tn;}

%token <tn> TRUE
%token <tn> FALSE
%token <tn> null
%token <tn> LCURLY
%token <tn> RCURLY
%token <tn> COMMA
%token <tn> COLON
%token <tn> LBRACKET
%token <tn> RBRACKET
%token <tn> STRINGLIT
%token <tn> NUMBER
%type <tn> root object object_center array array_center number value

//JSON grammar, according to JSON.org's universal data structures railroads.
//Some data structures' grammars are handled in json.l. Namely, whitespace, string, and number.
//Grammar is incomplete in json.l, such as in the case of hexadecimals.
%%

root
	:	object { yyroot = $1 ; printf("Setting Parse Rule: yyroot to %p\n", yyroot); }
;

object
	:	LCURLY RCURLY { $$ = nonleaf(OBJECT, OBJECT_R1, 2, $1, $2); printf("Setting Parse Rule: object $$ to %p\n", $$); }
	|	LCURLY object_center RCURLY { $$ = nonleaf(OBJECT, OBJECT_R2, 3, $1, $2, $3); printf("Setting Parse Rule: object $$ to %p\n", $$); }
;

object_center
	:	STRINGLIT COLON value { $$ = nonleaf(OBJECT_CENTER, OBJECT_CENTER_R1, 3, $1, $2, $3); printf("Setting Parse Rule: object_center $$ to %p\n", $$); }
	|	STRINGLIT COLON value COMMA object_center { $$ = nonleaf(OBJECT_CENTER, OBJECT_CENTER_R2, 5, $1, $2, $3, $4, $5); printf("Setting Parse Rule: object_center $$ to %p\n", $$); }
;

array
	:	LBRACKET array_center RBRACKET { $$ = nonleaf(ARRAY, ARRAY_R1, 3, $1, $2, $3); printf("Setting Parse Rule: array $$ to %p\n", $$); }
;

array_center
	:	value { $$ = nonleaf(ARRAY_CENTER, ARRAY_CENTER_R1, 1, $1); printf("Setting Parse Rule: array_center $$ to %p\n", $$); }
	|	value COMMA array_center { $$ = nonleaf(ARRAY_CENTER, ARRAY_CENTER_R2, 3, $1, $2, $3); printf("Setting Parse Rule: array_center $$ to %p\n", $$); }
;

number
	:	NUMBER { $$ = nonleaf(NUMBER, NUMBER_R1, 1, $1); printf("Setting Parse Rule: number $$ to %p\n", $$); }
;

value
	:	STRINGLIT { $$ = nonleaf(VALUE, VALUE_R1, 1, $1); printf("Setting Parse Rule: value $$ to %p\n", $$); }
	|	number { $$ = nonleaf(VALUE, VALUE_R2, 1, $1); printf("Setting Parse Rule: value $$ to %p\n", $$); }
	|	object { $$ = nonleaf(VALUE, VALUE_R3, 1, $1); printf("Setting Parse Rule: value $$ to %p\n", $$); }
	|	array { $$ = nonleaf(VALUE, VALUE_R4, 1, $1); printf("Setting Parse Rule: value $$ to %p\n", $$); }
	|	TRUE { $$ = nonleaf(VALUE, VALUE_R5, 1, $1); printf("Setting Parse Rule: value $$ to %p\n", $$); }
	|	FALSE { $$ = nonleaf(VALUE, VALUE_R6, 1, $1); printf("Setting Parse Rule: value $$ to %p\n", $$); }
	|	null { $$ = nonleaf(VALUE, VALUE_R7, 1, $1); printf("Setting Parse Rule: value $$ to %p\n", $$); }
;
