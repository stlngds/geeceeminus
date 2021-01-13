#include "json.tab.h"
#include "tree.h"
#include "prodrules.h"
#include <stdio.h>
#include <stdlib.h>
extern FILE *yyin;
extern char *yytext;
extern int yylineno;
char *yyfilename;
void yyerror(char *s);


int main(int argc, char *argv[])
{
   int i;
   if (argc < 2) { printf("usage: iscan file.dat\n"); exit(-1); }
   yyin = fopen(argv[1],"r");
   if (yyin == NULL) { printf("can't open/read '%s'\n", argv[1]); exit(-1); }
   yyfilename = argv[1];
   if ((i=yyparse()) != 0) {
      printf("parse failed\n");
      }
   else printf("no errors\n");
   return 0;
}

void yyerror(char *s)
{
	fprintf(stderr, "%s:%d: %s before '%s' token.\n", yyfilename, yylineno, s, yytext);
}