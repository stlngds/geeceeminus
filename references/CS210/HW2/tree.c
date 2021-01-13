//tree.c
//adapted from Dr. Jeffery's tree examples
#include "tree.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct node * treenode(int symbol){
	struct node *n = (struct node *)calloc(1, sizeof(struct node)); //allocate memory for node
	if (n == NULL) printf("Treenode constructer created NULL.\n"); //uh oh
	n->symbol = symbol;
	return n;
}

struct node * leaf(int symbol, char * token){ //used in flex file
	struct node * n = treenode(symbol);
	n->u.l.token = strdup(token);
	return n;
}

struct node * nonleaf(int symbol, int prodrule, int nkids, ...){
	int i;
	va_list mylist;
	struct node *n = treenode(symbol);
	n->u.tn.prodrule = prodrule;
	va_start(mylist, nkids);
	for (i = 0; i < nkids; i++){
		n->u.tn.children[i] = va_arg(mylist, struct node *);
		printf("set child %d to %p\n", n->u.tn.children[i]);
	}
	va_end(mylist);
	return n;
}


void print_tree(struct node *tn){
	int i;
	if (tn == NULL) {printf("Null tree pointer detected. Returning...\n"); return;} //avoid segfault
	printf("node %d\n", tn->symbol);
	if(tn->symbol < 1000){ //doublecheck if terminal symbol (if errors, check .tab.h file)
		printf("\t %s \t", tn->u.l.token);
		fflush(stdout);
	}

	else {
		for(i=0; i < maxkids; i++){if(tn->u.tn.children[i] != NULL) print_tree(tn->u.tn.children[i]);}
	}
}