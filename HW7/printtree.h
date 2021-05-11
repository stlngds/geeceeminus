// Greyson Biggs
// CS 445
// printtree.h
#ifndef _PRINTTREE_H_
#define _PRINTTREE_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "scanType.h"
#include "parser.tab.h"

//Recursively display the AST
void printTree(FILE *out, TreeNode *node, int level, int sibling);

void printTreeTypes(FILE *out, TreeNode *node, int level, int sibling, int allowType = 1);

void printTreeOffsets(FILE *out, TreeNode *node, int level, int sibling);


#endif