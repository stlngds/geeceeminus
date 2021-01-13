//tree.h
//adapted from Dr. Jeffery's tree examples
#define maxkids 5

struct node {
	int symbol; //<1000 is terminal, >=1000 is non-terminal
	union {
		struct leaf {
			char *token; //copy of yytext
			int i; //copy of val
		} l;
		struct nonleaf {
			int prodrule;
			struct node *children[maxkids];
		} tn;
	} u;
};

struct node * treenode(int symbol);
struct node * leaf(int symbol, char *token);
struct node * nonleaf(int symbol, int prodrule, int nkids, ...);

void print_tree(struct node *tn);

struct node *yyroot;