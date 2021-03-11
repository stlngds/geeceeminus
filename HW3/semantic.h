// Greyson Biggs
// CS 445
// semantic.h

#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include <cstdlib>
#include <cstring>
#include "scanType.h"
#include "symbolTable.h"
#include <cstring>
#include <stdlib.h>
#include "parser.tab.h"

bool areEqual(char a, char b);
char semantic(FILE*, TreeNode*);
char setIsUsed(FILE*, TreeNode*);
void passing(FILE *out, TreeNode *call, TreeNode *def, int num, TreeNode *node, TreeNode *p);
void passingUsed(FILE *out, TreeNode *call, TreeNode *def, int num, TreeNode *node, TreeNode *p);

void semerror(FILE *out, TreeNode *node, int m, const char *types, int pline, int lhs, int rhs);
void semwarning(FILE *out, TreeNode *node, int m);


#endif