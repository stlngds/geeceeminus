// Greyson Biggs
// CS 445
// 3/9/2021
// semantic.cpp

#include "semantic.h"

extern int numWarnings, numErrors;

SymbolTable declarations;
SymbolTable temp;
bool justInFunc = false; //used to help determine scope

int scopedepth = 0;
int sibcheck = 1; //have semantic() go down the sibling list or not
int hitfor = 0; //used to determine For loop scoping


void semerror(FILE *out, TreeNode *node, int m, const char *types = NULL, int pline = 0, int lhs = 0, int rhs = 0) {
    switch(m){
        char *lhss, *rhss; //used in case 1
        case 0:
            fprintf(out, "ERROR(%d): '%s' is a simple variable and cannot be called.\n", node->lineno, node->token->tokenstr);
            break;
        case 1:
            if (lhs == 1) {
                lhss = strdup("");
            }
            else {
                lhss = strdup(" not");
            }
            if (rhs == 1) {
                rhss = strdup("");
            }
            else {
                rhss = strdup(" not");
            }
            fprintf(out, "ERROR(%d): '%s' requires both operands be arrays or not but lhs is%s an array and rhs is%s an array.\n", node->lineno, node->token->tokenstr, lhss, rhss);
            break;
        case 2:
            fprintf(out, "ERROR(%d): '%s' requires operands of type %s but lhs is of type %s.\n", node->lineno, node->token->tokenstr, types, node->children[0]->expstr);
            break;
        case 3:
            fprintf(out, "ERROR(%d): '%s' requires operands of type %s but rhs is of type %s.\n", node->lineno, node->token->tokenstr, types, node->children[1]->expstr);
            break;
        case 4:
            fprintf(out, "ERROR(%d): '%s' requires operands of the same type but lhs is type %s and rhs is type %s.\n", node->lineno, node->token->tokenstr, node->children[0]->expstr, node->children[1]->expstr);
            break;
        case 5:
            fprintf(out, "ERROR(%d): Array '%s' should be indexed by type int but got type %s.\n", node->lineno, node->children[0]->token->tokenstr, node->children[1]->expstr);
            break;
        case 6:
            fprintf(out, "ERROR(%d): Array index is the unindexed array '%s'.\n", node->lineno, node->children[1]->token->tokenstr);
            break;
        case 7:
            fprintf(out, "ERROR(%d): Cannot index nonarray '%s'.\n", node->lineno, node->children[0]->token->tokenstr);
            break;
        case 8:
            fprintf(out, "ERROR(%d): Cannot return an array.\n", node->lineno);
            break;
        case 9:
            fprintf(out, "ERROR(%d): Cannot use function '%s' as a variable.\n", node->lineno, node->token->tokenstr);
            break;
        case 10:
            fprintf(out, "ERROR(%d): Symbol '%s' is already declared at line %d.\n", node->lineno, node->token->tokenstr, pline);
            break;
        case 11:
            fprintf(out, "ERROR(%d): Symbol '%s' is not declared.\n", node->lineno, node->token->tokenstr);
            break;
        case 12:
            fprintf(out, "ERROR(%d): The operation '%s' does not work with arrays.\n", node->lineno, node->token->tokenstr);
            break;
        case 13:
            fprintf(out, "ERROR(%d): The operation '%s' only works with arrays.\n", node->lineno, node->token->tokenstr);
            break;
        case 14:
            fprintf(out, "ERROR(%d): Unary '%s' requires an operand of type %s but was given type %s.\n", node->lineno, node->token->tokenstr, types, node->children[0]->expstr);
            break;
    }
    numErrors++;
}

void semwarning(FILE *out, TreeNode *node, int m) {
    switch(m) {
        case 0:
            fprintf(out, "WARNING(%d): The variable '%s' seems not to be used.\n", node->lineno, node->token->tokenstr);
            break;
        case 1: //unused
            fprintf(out, "WARNING(%d): Variable '%s' may be uninitialized when used here.\n", node->lineno, node->children[1]->token->tokenstr);
            break;
        case 2: //unused
            fprintf(out, "WARNING(%d): Variable '%s' may be uninitialized when used here.\n", node->lineno, node->children[0]->token->tokenstr);
            break;
        case 3:
            fprintf(out, "WARNING(%d): Variable '%s' may be uninitialized when used here.\n", node->lineno, node->token->tokenstr);
            break;
    }
    numWarnings++;
}

bool areEqual(char a, char b) { //are equal, regardless of capitalization
    return a == 'u' || b == 'u' || a == b || a == b - 32 || a == b + 32;
}

//recursive sibling traversal, used when analyzing variables passed in function calls
void passing(FILE *out, TreeNode *call, TreeNode *def, int num, TreeNode *node, TreeNode *p) {
    //more fun errors go here in the future
    if (call == NULL && def == NULL) { //reached the last passed variable
        return;
    }
    if (call == NULL) {
        //too few arguments
        //errors go here in the future
        return;
    }
    if (def == NULL) {
        //too many args
        //errors go here in the future
        int sbak = sibcheck;
        sibcheck = 1;
        semantic(out, call);
        sibcheck = sbak;
        return;
    }

    int sbak = sibcheck;
    sibcheck = 0; //do this to not check siblings more than once
    char r = semantic(out, call);
    sibcheck = sbak;
    passing(out, call->sibling, def->sibling, num+1, node, p);
}


// Recursively traverse the AST - similar to printtree - searching for semantic errors
// Also defines scopes with symbolTable
// Returns a char code indicating the type of the tree node - used for all sorts of error/warning handling and type info proliferation
// i: int
// b: bool
// c: char
// v: void
// u: undefined
// n: null //don't think this ever happens but...?
// Capitals indicate arrays.
char semantic(FILE *out, TreeNode *node) {
    if (node == NULL) {
        return 'n';
    }
    char r = 'u'; //return type of treenode
    int c[] = {1, 1, 1}; //indicates checked children with semantic(), so we don't check them again later
    int sib = sibcheck; //both used in passing() to not check siblings more than once
    char tmp;
    TreeNode *p, *g; //temporary nodes
    bool scoped = false; //entered scope
    bool looped = false; //entered loop
    int b = 1;
    int i = 0;

    //node should have a token, which should have a tokenclass, each kind of which should be handled here. Should never reach default case.
    switch(node->token->tokenclass) {
        case ID:
            switch(node->nodekind) {
                case DeclK:
                    switch(node->subkind.decl) {
                        case ParamK:
                            node->isInitialized = true;
                        case VarK:
                            //if var is initialized
                            if (node->children[0] != NULL) {
                                //c[0] = 0;
                                //semantic(out, node->children[0]);
                            }
                            r = node->expstr[0];
                            //if already declared
                            if (!declarations.insert(node->token->tokenstr, node) && node->isForIdDecl != true) {
                                semerror(out, node, 10, "ERROR", ((TreeNode*)(declarations.lookup(node->token->tokenstr)))->lineno);
                            }
                            else if (node->isUsed != true) {
                                semwarning(out, node, 0);
                            }
                            //fprintf(out, "VarK or ParamK %s has r %c with expstr %s\n", node->token->tokenstr, r, node->expstr);
                            
                            break;

                        case FuncK:
                            if (node->token == NULL || node->expType == UndefinedType) {
                                r = 'v';
                            }
                            else {
                                r = node->expstr[0];
                            }
                            if ((node->children[1] != NULL) && (node->children[1]->token->tokenclass == '{')) {
                                justInFunc = true;
                            }
                            //already declared
                            if (!declarations.insertGlobal(node->token->tokenstr, node)) {
                                semerror(out, node, 10, "ERROR", ((TreeNode*)(declarations.lookupGlobal(node->token->tokenstr)))->lineno);
                            }
                            //main can't have arguments
                            if (node->children[0] != NULL)
                            if (strcmp(node->token->tokenstr, "main") == 0 && strcmp(node->children[0]->token->tokenstr, "{") != 0) {
                                printf("ERROR(LINKER): A function named 'main()' must be defined.\n");
                                numErrors++;
                            }
                            declarations.enter(node->token->tokenstr);
                            scoped = true;
                            //fprintf(out, "FuncK %s has r %c\n", node->token->tokenstr, r);
                            break;

                        default:
                            fprintf(out, "Reached DeclK with no nodekind!\n");
                            break;
                    }
                    break;
                case ExpK:
                    switch(node->subkind.exp) {
                        //Reminder: IdK is for nodes used as variables (e.g. "wewlad += 1;" has wewlad as IdK. But "int wewlad;" has wewlad as VarK.)
                        case IdK:
                            p = (TreeNode*)declarations.lookup(node->token->tokenstr);
                            g = (TreeNode*)declarations.lookupGlobal(node->token->tokenstr);
                            //ID undeclared
                            if (p == NULL) {
                                semerror(out, node, 11);
                            }
                            else {
                                //used function name as though it were a variable
                                if (p->subkind.decl == FuncK) {
                                    semerror(out, node, 9);
                                }
                                else {
                                    r = p->expstr[0];
                                    if (p->isArray) {
                                        r -= 32;
                                    }
                                    if (p != g) //global scoped vars may be initialized elsewhere in the code, so we can't make any hard and fast claims about them
                                    if (p->isInitialized != true && p->isForIdDecl != true && b == 1) {
                                            //variable may be uninitialized when used here
                                            semwarning(out, node, 3);
                                            node->isInitialized = true; //prevent further warnings about the same variable later on
                                            p->isInitialized = true; //?
                                        }
                                }
                                //node->isUsed = true; //but this does
                            }
                            break;

                        case CallK:
                            p = (TreeNode*)declarations.lookup(node->token->tokenstr); //FuncK

                            if (p == NULL) {
                                //function not declared
                                semerror(out, node, 11);
                            }
                            else {
                                //tried to call a variable
                                if (p->subkind.decl == VarK) {
                                    semerror(out, node, 0);
                                }
                                else { //continue
                                    c[0] = 0;
                                    //check passed variables
                                    passing(out, node->children[0], p->children[0], 1, node, p);
                                    r = p->expstr[0];
                                    if (p->isArray) {
                                        r -= 32;
                                    }
                                }
                            }
                            break;

                        default:
                            break;
                    }
                    break;
                case StmtK:
                    switch(node->subkind.stmt) {
                        case ForK: //never reached, trim later
                            fprintf(out, "reached ForK\n");
                            break;

                        case RangeK: //never reached
                            fprintf(out, "reach RangeK\n");
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
            break;
        
        case '{': //entering a new scope
            if (hitfor) { //immediately follows a "do", so don't enter new scope as one was already previously entered at "for" - that's how for scopes work!
                hitfor = 0;
                break;
            }
            if (!justInFunc){
                declarations.enter("compound");
                scoped = true;
            }
            else {
                justInFunc = false;
            }
            break;

        case FOR:
            node->children[0]->isForIdDecl = true; //deprecated flag?
            if (!justInFunc){
                declarations.enter("for");
                scoped = true;
            }
            else {
                justInFunc = false;
            }
            c[0] = 0;
            semantic(out, node->children[0]);
            c[1] = 0;
            semantic(out, node->children[1]);
            if (strcmp(node->children[2]->token->tokenstr, "{") == 0) {
                hitfor = 1;
            }
            c[2] = 0;
            semantic(out, node->children[2]); //could be compound
            break;

        case TO:
            //?
            break;

        case BY:
            //???
            break;

        case WHILE:
            scopedepth++;
            looped = true;
            //don't break, go to IF stuff
        case IF:
            c[0] = 0;
            r = semantic(out, node->children[0]);
            r = 'v';
            break;

        case BREAK: //need to check how deep we are in scope
            if (scopedepth == 0) {
                //error goes here in the future
            }
            r = 'v';
            break;

        case RETURN:
            //returned array
            //fprintf(out, "return node is %s\n", node->children[0]->token->tokenstr);
            c[0] = 0;
            r = semantic(out, node->children[0]);
            if (r < 97) {
                semerror(out, node, 8);
            }
            //TODO in Assignment 4:
            //returned wrong type
            //returned nothing and function wasn't void
            //returned a type and function was void
            r = 'v';
            break;

        case '=':
            //order's a bit important here
            c[1] = 0;
            tmp = semantic(out, node->children[1]);

            //mark stuff as initialized
            if (strcmp(node->children[0]->token->tokenstr, "[") == 0) { //array case
                node->children[0]->children[0]->isInitialized = true;
                //node->children[0]->children[0]->isArray = true;
                TreeNode *j;
                j = (TreeNode*)declarations.lookup(node->children[0]->children[0]->token->tokenstr);
                if (j != NULL) {
                    j->isInitialized = true;
                    //j->isArray = true;
                    //fprintf(out, "marked %s and %s as isInitialized\n", node->children[0]->children[0]->token->tokenstr, j->token->tokenstr);
                }
                //else fprintf(out, "could not lookup array token %s\n", node->children[0]->children[0]->token->tokenstr);
            }
            else { //not array
                node->children[0]->isInitialized = true; //doesn't seem to do anything but keeping just in case
                TreeNode *j;
                j = (TreeNode*)declarations.lookup(node->children[0]->token->tokenstr);
                if (j != NULL) {
                    j->isInitialized = true;
                    //fprintf(out, "marked %s and %s as isInitialized\n", node->children[0]->token->tokenstr, j->token->tokenstr);
                }
                //else fprintf(out, "could not lookup nonarray token %s\n", node->children[0]->token->tokenstr);
            }

            c[0] = 0;
            r = semantic(out, node->children[0]);


            if (!areEqual(r, tmp)) {
                //operand mismatch
                semerror(out, node, 4);
            }
            else {
                if (r == 'v') {
                    //lhs wrong type
                    semerror(out, node, 2, "nonvoid");
                }
                if (tmp == 'v') {
                    //rhs wrong type
                    semerror(out, node, 3, "nonvoid");
                }
            }
            //one side is array and the other isn't
            if((r > 96 && tmp < 97) || (r < 97 && tmp > 96)) {
                int left, right;
                if (r > 96) {
                    left = 0;
                    right = 1;
                }
                else {
                    left = 1;
                    right = 0;
                }
                semerror(out, node, 1, "ERROR", 0, left, right);
            }
            break;

        case ADDASS:
        case SUBASS:
        case MULASS:
        case DIVASS:
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
            c[0] = 0;
            r = semantic(out, node->children[0]);
            c[1] = 0;
            tmp = semantic(out, node->children[1]);
            
            if (r != 'u')
            if (!(r == 'i' || r == 'I')) {
                //lhs wrong type
                semerror(out, node, 2, "int");
            }
            if (tmp != 'u')
            if (!(tmp == 'i' || tmp == 'I')) {
                //rhs wrong type
                semerror(out, node, 3, "int");
            }
            if (r < 97 || tmp < 97) {
                //no arrays allowed
                semerror(out, node, 12);
            }
            r = 'i';
            break;

        case INC:
        case DEC:
            c[0] = 0;
            r = semantic(out, node->children[0]);
            
            if (r != 'u')
            if (!(r == 'i' || r == 'I')) {
                //unary type wrong
                semerror(out, node, 14, "int");
            }
            if (r < 97) {
                //no arrays allowed
                semerror(out, node, 12);
            }
            r = 'i';
            break;

        case AND:
        case OR:
            c[0] = 0;
            r = semantic(out, node->children[0]);
            c[1] = 0;
            tmp = semantic(out, node->children[1]);
            
            if (r != 'u')
            if (!(r == 'b' || r == 'B')) {
                //lhs wrong type
                semerror(out, node, 2, "bool");
            }
            if (tmp != 'u')
            if (!(tmp == 'b' || tmp == 'B')) {
                //rhs wrong type
                semerror(out, node, 3, "bool");
            }
            if (r < 97 || tmp < 97) {
                //no arrays allowed
                semerror(out, node, 12);
            }
            r = 'b';
            break;

        case NOT:
            c[0] = 0;
            r = semantic(out, node->children[0]);
            
            if (r != 'u')
            if (!(r == 'b' || r == 'B')) {
                //unary type wrong
                semerror(out, node, 14, "bool");
            }
            if (r < 97) {
                //no arrays allowed
                semerror(out, node, 12);
            }
            r = 'b';
            break;
        
        case LEQ:
        case GEQ:
        case '<':
        case '>':
            c[0] = 0;
            r = semantic(out, node->children[0]);
            c[1] = 0;
            tmp = semantic(out, node->children[1]);

            
            if ((!areEqual(r, tmp)) && (r != 'v' && r != 'u' && tmp != 'v' && tmp != 'u')) {
                //type mismatch
                semerror(out, node, 4);
            }
            //one side is array and the other isn't
            if((r > 96 && tmp < 97) || (r < 97 && tmp > 96)) {
                int left, right;
                if (r > 96) {
                    left = 0;
                    right = 1;
                }
                else {
                    left = 1;
                    right = 0;
                }
                semerror(out, node, 1, "ERROR", 0, left, right);
            }
            r = 'b';
            break;

        case MIN:
        case MAX:
            c[0] = 0;
            r = semantic(out, node->children[0]);
            c[1] = 0;
            tmp = semantic(out, node->children[1]);
            
            if (r != 'u')
            if (!(r == 'i' || r == 'I')) {
                //lhs wrong
                semerror(out, node, 2, "int");
            }
            
            if (tmp != 'u')
            if (!(tmp == 'i' || tmp == 'I')) {
                //rhs wrong
                semerror(out, node, 3, "int");
            }
            break;

        case EQ:
        case NEQ:
            c[0] = 0;
            r = semantic(out, node->children[0]);
            c[1] = 0;
            tmp = semantic(out, node->children[1]);
            
            if ((!areEqual(r, tmp)) && (r != 'v' && r != 'u' && tmp != 'v' && tmp != 'u')) {
                //type mismatch
                semerror(out, node, 4);
            }
            if (r == 'v') {
                //lhs wrong
                semerror(out, node, 2, "nonvoid");
            }
            if (tmp == 'v') {
                //rhs wrong
                semerror(out, node, 3, "nonvoid");
            }
            //one side is array and the other isn't
            if((r > 96 && tmp < 97) || (r < 97 && tmp > 96)) {
                int left, right;
                if (r > 96) {
                    left = 0;
                    right = 1;
                }
                else {
                    left = 1;
                    right = 0;
                }
                semerror(out, node, 1, "ERROR", 0, left, right);
            }
            r = 'b';
            break;

        case CHSIGN: //unary
            c[0] = 0;
            r = semantic(out, node->children[0]);
            node->token->tokenstr = strdup("chsign");
            
            if (r != 'u')
            if (!(r == 'i' || r == 'I')) { 
                //wrong type
                semerror(out, node, 14, "int");
            }
            if (r < 97) {
                //no arrays allowed
                semerror(out, node, 12);
            }
            r = 'i';
            break;

        case SIZEOF: //unary
            c[0] = 0;
            r = semantic(out, node->children[0]);
            node->token->tokenstr = strdup("sizeof");
            
            if (r > 96 && r != 'u') {
                //arrays only
                semerror(out, node, 13);
            }
            r = 'i';
            break;

        case '?':
            c[0] = 0;
            r = semantic(out, node->children[0]);
            
            if (r != 'u')
            if (!(r == 'i' || r == 'I')) {
                //wrong type
                semerror(out, node, 14, "int");
            }
            if (r < 97) {
                //no arrays
                semerror(out, node, 12);
            }
            r = 'i';
            break;

        case '[':
            c[0] = 0;
            r = semantic(out, node->children[0]); //ID
            c[1] = 0;
            tmp = semantic(out, node->children[1]); //Index
            if (r > 96) {
                if (node->children[0]->token->tokenclass == ID) {
                    //indexing nonarray
                    semerror(out, node, 7);
                }
            }
            else {
                r += 32;
            }
            if (!(tmp == 'i' || tmp == 'I')) {
                //indexed by non-int
                semerror(out, node, 5);
            }
            if (tmp < 97) {
                if (node->children[1]->token->tokenclass == ID) {
                    //indexing with unindexed array
                    semerror(out, node, 6);
                }
            }
            //keep r
            break;

        case '.':
            c[0] = 0;
            semantic(out, node->children[0]);
            c[1] = 1;
            r = semantic(out, node->children[1]);
            break;

        case CHARCONST:
            r = 'c';
            break;

        case STRINGCONST:
            r = 'C';
            break;

        case NUMCONST:
            if (node->isArray) {
                r = 'I';
            }
            else {
                r = 'i';
            }
            break;

        case BOOLCONST:
            if (node->isArray) {
                r = 'B';
            }
            else {
                r = 'b';
            }
            break;

        default: //should never happen
            fprintf(out, "Something horrible happened analyzing line %i, token %s\n", node->lineno, node->token->tokenstr);
            break;
    }

    switch(r) {
        case 'v':
        case 'V':
            node->expType = Void;
            node->expstr = strdup("void");
            break;
        case 'i':
        case 'I':
            node->expType = Integer;
            node->expstr = strdup("int");
            break;
        case 'b':
        case 'B':
            node->expType = Boolean;
            node->expstr = strdup("bool");
            break;
        case 'c':
        case 'C':
            node->expType = Char;
            node->expstr = strdup("char");
            break;
        case 'u':
        case 'U':
            node->expType = UndefinedType;
            node->expstr = strdup("undefined");
            break;
        case 'n':
        case 'N':
            break;
        default:
            r = 'u';
            node->expType = UndefinedType;
            node->expstr = strdup("undefined");
            break;
    }
    
    //traversal
    for (int i = 0; i < 3; i++) {
        if (c[i]) {
            semantic(out, node->children[i]);
        }
    }

    if (scoped) {
        declarations.leave();
    }
    if (looped) {
        scopedepth--;
    }

    if (sib) {
        semantic(out, node->sibling);
    }

    return r;
}

//traverse the tree to determine what VarK never get used as IdK, to print a warning later in semantic().
char setIsUsed (FILE *out, TreeNode *node) {
    if (node == NULL) {
        return 'n';
    }
    char r = 'u'; //return type of treenode
    int c[] = {1, 1, 1}; //indicates checked children with semantic(), so we don't check them again later
    int sib = sibcheck; //both used in passing() to not check siblings more than once
    char tmp;
    TreeNode *p, *g; //temporary nodes
    bool scoped = false; //entered scope
    bool looped = false; //entered loop
    int b = 1;
    int i = 0;

    switch(node->token->tokenclass){
        case ID:
            switch(node->nodekind) {
                case DeclK:
                    switch(node->subkind.decl) {
                        case ParamK:
                        case VarK:
                            if (!temp.insert(node->token->tokenstr, node) && node->isForIdDecl != true) {

                            }
                            if (!(!temp.lookupGlobal(node->token->tokenstr))) { //if global scope
                                node->isUsed = true;
                            }
                            break;

                        case FuncK:
                            if ((node->children[1] != NULL) && (node->children[1]->token->tokenclass == '{')) {
                                justInFunc = true;
                            }

                            if (!temp.insertGlobal(node->token->tokenstr, node)) {

                            }
                            temp.enter(node->token->tokenstr);
                            scoped = true;
                            break;

                        default:
                            break;
                    }
                    break;

                case ExpK:
                    switch(node->subkind.exp) {
                        case IdK:
                            p = (TreeNode*)temp.lookup(node->token->tokenstr);
                            g = (TreeNode*)temp.lookupGlobal(node->token->tokenstr);

                            node->isUsed = true;
                            if (p != NULL) {
                                p->isUsed = true;
                            }
                            if (g != NULL /*&& p == NULL*/) { //we can only assume the global var is being used if a local doesn't exist //EDIT: Never mind, doesn't match test output?
                                g->isUsed = true;
                            }
                            break;

                        case CallK:
                            p = (TreeNode*)declarations.lookup(node->token->tokenstr); //FuncK

                            if (p == NULL) {
                                //function not declared
                            }
                            else {
                                //tried to call a variable
                                if (p->subkind.decl == VarK) {
                                }
                                else { //continue
                                    c[0] = 0;
                                    //check passed variables
                                    passingUsed(out, node->children[0], p->children[0], 1, node, p);
                                }
                            }
                            break;

                        default:
                            break;
                    }
                    break;
            }
            break;
        case '{':
            if (hitfor) { //immediately follows a "do", so don't enter new scope as one was already previously entered at "for" - that's how for scopes work!
                hitfor = 0;
                break;
            }
            if (!justInFunc){
                temp.enter("compound");
                scoped = true;
            }
            else {
                justInFunc = false;
            }
            break;

        case FOR:
            node->children[0]->isForIdDecl = true;
            if (!justInFunc){
                temp.enter("for");
                scoped = true;
            }
            else {
                justInFunc = false;
            }
            c[0] = 0;
            setIsUsed(out, node->children[0]);
            c[1] = 0;
            setIsUsed(out, node->children[1]);
            if (strcmp(node->children[2]->token->tokenstr, "{") == 0) {
                hitfor = 1;
            }
            c[2] = 0;
            setIsUsed(out, node->children[2]); //could be compound
            break;

        case WHILE:
            scopedepth++;
            looped = true;
        case IF:
            c[0] = 0;
            r = setIsUsed(out, node->children[0]);
            r = 'v';
            break;

        default:
            break;
    }

    //traversal
    for (int i = 0; i < 3; i++) {
        if (c[i]) {
            setIsUsed(out, node->children[i]);
        }
    }

    if (scoped) {
        temp.leave();
    }
    if (looped) {
        scopedepth--;
    }

    if (sib) {
        setIsUsed(out, node->sibling);
    }

}

//version of passing() for setIsUsed()
void passingUsed(FILE *out, TreeNode *call, TreeNode *def, int num, TreeNode *node, TreeNode *p) {
    //more fun errors go here in the future
    if (call == NULL && def == NULL) { //reached the last passed variable
        return;
    }
    if (call == NULL) {
        //too few arguments
        //errors go here in the future
        return;
    }
    if (def == NULL) {
        //too many args
        //errors go here in the future
        int sbak = sibcheck;
        sibcheck = 1;
        setIsUsed(out, call);
        sibcheck = sbak;
        return;
    }

    int sbak = sibcheck;
    sibcheck = 0; //do this to not check siblings more than once
    char r = setIsUsed(out, call);
    sibcheck = sbak;
    passingUsed(out, call->sibling, def->sibling, num+1, node, p);
}