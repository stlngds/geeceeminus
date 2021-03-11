// Greyson Biggs
// CS 445
// printtree.h

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "scanType.h"
#include "parser.tab.h"

//Recursively display the AST
void printTree(FILE *out, TreeNode *node, int level, int sibling);