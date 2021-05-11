// Greyson Biggs
// CS 445
// 3/9/2021
// semantic.cpp

#include "semantic.h"

extern int numWarnings, numErrors;
extern TreeNode *newDeclNode(DeclKind kind,
                      ExpType type,
                      TokenData *token=NULL,
                      TreeNode *c0=NULL,
                      TreeNode *c1=NULL,
                      TreeNode *c2=NULL);  // save TokenData block!!
extern TreeNode *addSibling(TreeNode *t, TreeNode *s);

SymbolTable declarations;
SymbolTable temp;
bool justInFunc = false; //used to help determine scope

int doesreturn = 0;
int scopedepth = 0;
int sibcheck = 1; //have semantic() go down the sibling list or not
int hitfor = 0; //used to determine For loop scoping
TreeNode *currFunc = NULL;


int goffset = 0;
int foffset;

void semerror(FILE *out, TreeNode *node, int m, const char *types = NULL, int pline = 0, int lhs = 0, int rhs = 0, int pline2 = 0) {
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

        //Assignment 4
        case 15: //////
            fprintf(out, "ERROR(%d): Cannot have a break statement outside of loop.\n", node->lineno);
            break;
        case 16: //////
            fprintf(out, "ERROR(%d): Cannot use array as test condition in %s statement.\n", node->lineno, node->token->tokenstr);
            break;
        case 17: //////
            fprintf(out, "ERROR(%d): Cannot use array in position %d in range of for statement.\n", node->lineno, pline);
            break;
        case 19: //////
            fprintf(out, "ERROR(%d): Expecting type %s in position %d in range of for statement but got type %s.\n", node->lineno, "int", pline, node->expstr);
            break;
        case 20: //////
            fprintf(out, "ERROR(%d): Expecting Boolean test condition in %s statement but got type %s.\n", node->lineno, node->token->tokenstr, node->children[0]->expstr);
            break;

        //call type mismatches
        //handled in passing() instead of here
        case 18: //////
            //fprintf(out, "ERROR(%d): Expecting type %s in parameter %i of call to '%s' declared on line %d but got type %s.\n", node->lineno, types, pline2, tmp->token->tokenstr, tmp->lineno, ?);
            break;
        case 21: //////
            //lookup function being called, get the lineno
            //fprintf(out, "ERROR(%d): Expecting array in parameter %i of call to '%s' declared on line %d.\n", node->lineno, pline2, tmp->token->tokenstr, tmp->lineno);
            break;
        case 27: //////
            //fprintf(out, "ERROR(%d): Not expecting array in parameter %i of call to '%s' declared on line %d.\n", node->lineno, pline2, ?, pline);
            break;

        //function errors
        case 22: //////
            fprintf(out, "ERROR(%d): Function '%s' at line %d is expecting no return value, but return has a value.\n", node->lineno, currFunc->token->tokenstr, currFunc->lineno);
            break;
        case 23: //////
            fprintf(out, "ERROR(%d): Function '%s' at line %d is expecting to return type %s but return has no value.\n", node->lineno, currFunc->token->tokenstr, currFunc->lineno, currFunc->expstr);
            break;
        case 24: //////
            fprintf(out, "ERROR(%d): Function '%s' at line %d is expecting to return type %s but returns type %s.\n", node->lineno, currFunc->token->tokenstr, currFunc->lineno, currFunc->expstr, node->children[0]->expstr);
            break;

        case 25: //////
            fprintf(out, "ERROR(%d): Initializer for variable '%s' is not a constant expression.\n", node->lineno, node->token->tokenstr);
            break;

        case 26: //////
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
            fprintf(out, "ERROR(%d): Initializer for variable '%s' requires both operands be arrays or not but variable is%s an array and rhs is%s an array.\n", node->lineno, node->token->tokenstr, lhss, rhss);
            break;
        case 28: //////
            fprintf(out, "ERROR(%d): Too few parameters passed for function '%s' declared on line %d.\n", node->lineno, node->token->tokenstr, pline);
            break;
        case 29: //////
            fprintf(out, "ERROR(%d): Too many parameters passed for function '%s' declared on line %d.\n", node->lineno, node->token->tokenstr, pline);
            break;
        case 30:
            fprintf(out, "ERROR(%d): Initializer for variable '%s' of type %s is of type %s\n", node->lineno, node->token->tokenstr, node->expstr, types);
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

        //Assignment 4
        case 4:
            fprintf(out, "WARNING(%d): Expecting to return type %s but function '%s' has no return statement.\n", node->lineno, node->expstr, node->token->tokenstr);
            break;
        case 5: //semi-dupe of 0
            fprintf(out, "WARNING(%d): The function '%s' seems not to be used.\n", node->lineno, node->token->tokenstr);
            break;
        case 6: //semi-dupe of 0
            fprintf(out, "WARNING(%d): The parameter '%s' seems not to be used.\n", node->lineno, node->token->tokenstr);
            break;
    }
    numWarnings++;
}

bool areEqual(char a, char b) { //are equal, regardless of capitalization
    return a == 'u' || b == 'u' || a == b || a == b - 32 || a == b + 32;
}

//check if any child isn't isConst
bool checkconst(TreeNode *node[]) {
    for (int i = 0; i < 3; i++) {
        if (node[i] != NULL)
            if (!node[i]->isConst) {
                return false;
                }
    }
    return true;
}

//recursive sibling traversal, used when analyzing variables passed in function calls
void passing(FILE *out, TreeNode *call, TreeNode *def, int num, TreeNode *node, TreeNode *p) {
    //more fun errors go here in the future
    if (call == NULL && def == NULL) { //reached the last passed variable
        return;
    }
    if (call == NULL) {
        //too few arguments
        semerror(out, node, 28, "ERROR", p->lineno);
        return;
    }
    if (def == NULL) {
        //too many args
        semerror(out, node, 29, "ERROR", p->lineno);
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

    //check for mismatches between call and def
    //type mismatch
    if (call->expstr[0] != def->expstr[0] && call->expstr[0] != 'u') {
        fprintf(out, "ERROR(%d): Expecting type %s in parameter %i of call to '%s' declared on line %d but got type %s.\n", node->lineno, def->expstr, num, p->token->tokenstr, p->lineno, call->expstr);
        numErrors++;
    }
    //not expecting array
    if (call->isArray && !def->isArray) {
        fprintf(out, "ERROR(%d): Not expecting array in parameter %i of call to '%s' declared on line %d.\n", node->lineno, num, p->token->tokenstr, def->lineno);
        numErrors++;
    }
    //expecting array
    if (!call->isArray && def->isArray) {
        fprintf(out, "ERROR(%d): Expecting array in parameter %i of call to '%s' declared on line %d.\n", node->lineno, num, p->token->tokenstr, def->lineno);
        numErrors++;
    }

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
    bool justInFunctmp;
    int numsibs = 0;
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
                            node->isConst = true;
                        case VarK:
                            //if var is initialized
                            if (node->isArray && node->subkind.decl == VarK) {
                                node->size = node->arrlen + 1;
                            }
                            if (node->children[0] != NULL) {
                                c[0] = 0;
                                r = semantic(out, node->children[0]);
                                if ((r > 96 && node->isArray) || (r < 97 && !node->isArray)) {
                                    //array initializer mismatch
                                    int left, right;
                                    if (r > 96) {
                                        left = 1;
                                        right = 0;
                                    }
                                    else {
                                        left = 0;
                                        right = 1;
                                    }
                                    semerror(out, node, 26, "ERROR", 0, left, right, 0);
                                }
                                if (node->subkind.decl == VarK) {
                                    if (!node->children[0]->isConst) {
                                        //nonconstant initializer
                                        semerror(out, node, 25);
                                    }
                                }
                                if (!areEqual(node->expstr[0], r)) {
                                    semerror(out, node, 30, node->children[0]->expstr);
                                }
                            }

                            r = node->expstr[0];

                            //if already declared
                            if (!declarations.insert(node->token->tokenstr, node) && node->isForIdDecl != true) {
                                semerror(out, node, 10, "ERROR", ((TreeNode*)(declarations.lookup(node->token->tokenstr)))->lineno);
                                node->varKind = Local;
                            }
                            else {
                                if (node->isUsed != true) {
                                    //seems not to be used
                                    if (node->subkind.decl == ParamK)
                                        semwarning(out, node, 6);
                                    else //VarK
                                        semwarning(out, node, 0);
                                }
                                //Global
                                if (declarations.depth() <= 1){
                                    node->offset = goffset;
                                    goffset -= node->size;
                                    //printf("Global var %s, goffset: %i\n", node->token->tokenstr, goffset);
                                    node->varKind = Global;
                                }
                                //LocalStatic
                                else if (node->isStatic == true){
                                    node->offset = goffset;
                                    goffset -= node->size;
                                    node->varKind = LocalStatic;
                                }
                                //Parameter
                                else if (node->subkind.decl == ParamK) {
                                node->offset = foffset;
                                node->varKind = Parameter;
                                foffset-= node->size;
                                }
                                //Local
                                else {
                                    node->offset = foffset;
                                    foffset -= node->size;
                                    node->varKind = Local;
                                }
                            }

                            if (node->isArray && node->subkind.decl != ParamK) {
                                node->offset--;
                                node->size = node->arrlen + 1;
                            }
                            
                            //fprintf(out, "VarK or ParamK %s has r %c with expstr %s\n", node->token->tokenstr, r, node->expstr);
                            
                            break;

                        case FuncK:
                            foffset = -2;
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
                                printf("ERROR(LINKER): A function named 'main' with no parameters must be defined.\n");
                                numErrors++;
                            }
                            //seems not to be used
                            //fprintf(out, "%s %i\n", node->token->tokenstr, node->isUsed);
                            p = (TreeNode*)temp.lookup(node->token->tokenstr);
                            if (p->isUsed != true) { //for some reason, isUsed doesn't always work, but p does.
                                semwarning(out, node, 5);
                            }
                            declarations.enter(node->token->tokenstr);
                            scoped = true;
                            currFunc = node;
                            if (node->children[0] != NULL)
                            if (node->children[0]->token->tokenclass == ID)
                            if (node->children[0]->nodekind == DeclK)
                            if (node->children[0]->subkind.decl == ParamK) { 
                                TreeNode *tmp;
                                tmp = node->children[0];
                                numsibs++;
                                while (tmp->sibling != NULL) {
                                    tmp = tmp->sibling;
                                    numsibs++;
                                }
                            }
                            node->size = -2 - numsibs;
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
                                node->varKind = Local;
                            }
                            else {
                                //used function name as though it were a variable
                                if (p->subkind.decl == FuncK) {
                                    semerror(out, node, 9);
                                    //
                                    if (p == currFunc) {
                                        node->varKind = Local;
                                        node->size = -3;
                                    }
                                    else {
                                        node->varKind = Global;
                                        node->size = p->size;
                                    }
                                }
                                else {
                                    r = p->expstr[0];
                                    node->isStatic = p->isStatic;
                                    node->size = p->size;
                                    node->varKind = p->varKind;
                                    if (p->isArray) {
                                        r -= 32;
                                        node->isArray = true;
                                    }
                                    if (node->isInitialized)
                                        p->isInitialized = true;
                                    if (p != g) //global scoped vars may be initialized elsewhere in the code, so we can't make any hard and fast claims about them
                                    if (p->isInitialized != true && p->isForIdDecl != true && b == 1 && node->isStatic != true) {
                                            //variable may be uninitialized when used here
                                            semwarning(out, node, 3);
                                            node->isInitialized = true; //prevent further warnings about the same variable later on
                                            p->isInitialized = true; //?
                                    }
                                }
                                
                                node->isConst = false;
                                node->offset = p->offset;
                                if (p->subkind.decl == ParamK) { // used in codegen
                                    node->declaredAsParm = true;
                                }
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
                                node->isConst = false;
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
            node->size = foffset;
            if (hitfor) { //immediately follows a "do", so don't enter new scope as one was already previously entered at "for" - that's how for scopes work!
                hitfor = 0;
                break;
            }
            if (!justInFunc){
                declarations.enter("compound");
                scoped = true;
            }
            if (justInFunc) {
                justInFunc = false;
                justInFunctmp = true;
                node->size = foffset;
                node->isConst = false;
                break;
            }
            node->isConst = false;
            node->size = foffset;
            break;

        case FOR:
            scopedepth++;
            looped = true;
            b = foffset;
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
            
            foffset -= 2;
            node->size = foffset;
            c[1] = 0;
            semantic(out, node->children[1]);
            if (node->children[2] != NULL) //otherwise segfaults if nothing follows "do"
            if (strcmp(node->children[2]->token->tokenstr, "{") == 0) {
                hitfor = 1;
            }
            c[2] = 0;
            semantic(out, node->children[2]); //could be compound
            //node->size = foffset;
            break;

        case TO:
            c[0] = 0;
            r = semantic(out, node->children[0]);
            if (r < 97)
                //can't put arrays in ranges
                semerror(out, node->children[0], 17, "ERROR", 1);
            if (!(r == 'i' || r == 'I' || r == 'u' || r == 'U'))
                //can't put non-ints(?) in ranges
                semerror(out, node->children[0], 19, "ERROR", 1);
            c[1] = 0;
            r = semantic(out, node->children[1]);
            if (r < 97)
                semerror(out, node->children[1], 17, "ERROR", 2);
            if (!(r == 'i' || r == 'I' || r == 'u' || r == 'U'))
                semerror(out, node->children[1], 19, "ERROR", 2);
            c[2] = 0;
            if (node->children[2] != NULL) {
                r = semantic(out, node->children[2]);
                if(r < 97)
                    semerror(out, node->children[2], 17, "ERROR", 3);
                if (!(r == 'i' || r == 'I' || r == 'u' || r == 'U'))
                    semerror(out, node->children[2], 19, "ERROR", 3);
            }
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
            if (!(r == 'b' || r == 'B')) {
                //can't test a non-boolean
                semerror(out, node, 20);
            }
            if (r < 97) {
                //can't test an array
                semerror(out, node, 16);
            }
            r = 'v';
            node->isConst = false;
            break;

        case BREAK: //need to check how deep we are in scope
            if (scopedepth == 0) {
                semerror(out, node, 15);
            }
            r = 'v';
            node->isConst = false;
            break;

        case RETURN:
            doesreturn = 1;
            c[0] = 0;
            r = semantic(out, node->children[0]);
            if (currFunc->expstr[0] == 'v' && r != 'n') {
                //returned value on void function
                semerror(out, node, 22);
            }
            if (currFunc->expstr[0] != 'v' && r == 'n') {
                //returned nothing
                semerror(out, node, 23);
            }
            if (currFunc->expstr[0] != 'v' && r != 'n' && !areEqual(currFunc->expstr[0], r)) {
                //return type mismatch
                semerror(out, node, 24);
            }
            if (r < 97) {
                semerror(out, node, 8);
            }
            r = 'v';
            node->isConst = checkconst(node->children);
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

            
            if (r < 97) {
                node->isArray = true;
            }
            
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
                semerror(out, node, 1, "ERROR", 0, left, right, 0);
            }
            break;

        case ADDASS:
        case SUBASS:
        case MULASS:
        case DIVASS:
        node->children[0]->isInitialized = true;
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
            node->isConst = checkconst(node->children);
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
            node->isConst = checkconst(node->children);
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
            node->isConst = checkconst(node->children);
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
            node->isConst = checkconst(node->children);
            break;
        
        case LEQ:
        case GEQ:
        case '<':
        case '>':
            c[0] = 0;
            r = semantic(out, node->children[0]);
            c[1] = 0;
            tmp = semantic(out, node->children[1]);

            
            if ((!areEqual(r, tmp)) && (r != 'u' && tmp != 'u')) {
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
                semerror(out, node, 1, "ERROR", 0, left, right, 0);
            }
            r = 'b';
            node->isConst = checkconst(node->children);
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
            node->isConst = checkconst(node->children);
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
                semerror(out, node, 1, "ERROR", 0, left, right, 0);
            }
            r = 'b';
            node->isConst = checkconst(node->children);
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
            node->isConst = checkconst(node->children);
            break;

        case SIZEOF: //unary
            c[0] = 0;
            //node->children[0]->isInitialized = true; //test output demands this
            r = semantic(out, node->children[0]);
            node->token->tokenstr = strdup("sizeof");
            
            if (r > 96 && r != 'u') {
                //arrays only
                semerror(out, node, 13);
            }
            r = 'i';
            node->isConst = checkconst(node->children);
            break;

        case '?':
            c[0] = 0;
            node->children[0]->isInitialized = true; //test output demands this
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
            node->isConst = false;
            break;

        case '[':
            //foffset -= node->size;
            c[0] = 0;
            r = semantic(out, node->children[0]); //ID
            node->children[0]->isArray = true;
            c[1] = 0;
            node->children[1]->isInitialized = true; //test output demands that uninit warnings not be thrown at array indices. This has been acknowledged as a bug by Professor Heckendorn but he decided to leave it as-is.
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
            if (!(tmp == 'i' || tmp == 'I' || tmp == 'u' || tmp == 'U')) {
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
            node->isConst = checkconst(node->children);
            break;

        case '.':
            c[0] = 0;
            semantic(out, node->children[0]);
            c[1] = 1;
            r = semantic(out, node->children[1]);
            break;

        case CHARCONST:
            r = 'c';
            node->isConst = true;
            break;

        case STRINGCONST:
            r = 'C';
            node->isConst = true;
            node->isArray = true;
            node->size = strlen(node->token->tokenstr) - 1;
            node->varKind = Global;
            node->offset = goffset - 1;
            goffset -= node->size;
            break;

        case NUMCONST:
            if (node->isArray) {
                r = 'I';
            }
            else {
                r = 'i';
            }
            node->isConst = true;
            break;

        case BOOLCONST:
            if (node->isArray) {
                r = 'B';
            }
            else {
                r = 'b';
            }
            node->isConst = true;
            break;

        default: //should never happen
            fprintf(out, "Something horrible happened analyzing line %i, token %s\n", node->lineno, node->token->tokenstr);
            node->isConst = false;
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
    
    //if (r < 97)
    //    node->isArray = true;
    if (node->varKind == Local && node->isStatic == true) {
        node->varKind = LocalStatic;
    }
    
    //traversal
    for (int i = 0; i < 3; i++) {
        if (c[i]) {
            semantic(out, node->children[i]);
        }
    }
    //if (justInFunctmp) {
    //    node->size = -3;
    //}

    if (scoped == true) { //hit FuncK
        //expected return but didn't get one
        if (node->token->tokenclass == ID) 
        if (node->nodekind == DeclK) 
        if (node->subkind.decl = FuncK) {
            node->varKind = Global;
            if (!(node->expstr[0] == 'v' || node->expstr[0] == 'u') && doesreturn < 1) {
                semwarning(out, node, 4);
            }
            doesreturn = 0;
        }
        declarations.leave();
    }
    if (looped) {
        scopedepth--;
    }

    if (sib) {
        semantic(out, node->sibling);
    }

    if (node->token->tokenclass == '{') {
        node->size = foffset;
    }

    if (node->token->tokenclass == FOR) {
        foffset = b;
    }

    return r;
}

//insert IO routine prototypes into the symbol table
void insertIO () {
    //void output(int)
    //construct TokenData of FuncK node
    TokenData *outputtd = (TokenData*)malloc(sizeof(TokenData));
    outputtd->linenum = -1;
    outputtd->tokenstr = strdup("output");
    outputtd->tokenclass = ID;
    //construct TokenData of integer parameter
    TokenData *outputparmtd = (TokenData*)malloc(sizeof(TokenData));
    outputparmtd->linenum = -1;
    outputparmtd->tokenstr = strdup("*dummy*");
    outputparmtd->tokenclass = NUMCONST;
    //construct ParamK TreeNode
    TreeNode *outputparmtn = newDeclNode(ParamK, Integer, outputparmtd);
    //construct FuncK TreeNode, with parameter node as child
    TreeNode *outputtn = newDeclNode(FuncK, Void, outputtd, outputparmtn);
    declarations.insert("output", outputtn);
    //set location in TM code
    outputtn->loc = 5;

    //void outputb(bool)
    //construct TokenData of FuncK node
    TokenData *outputbtd = (TokenData*)malloc(sizeof(TokenData));
    outputbtd->linenum = -1;
    outputbtd->tokenstr = strdup("outputb");
    outputbtd->tokenclass = ID;
    //construct TokenData of boolean parameter
    TokenData *outputbparmtd = (TokenData*)malloc(sizeof(TokenData));
    outputbparmtd->linenum = -1;
    outputbparmtd->tokenstr = strdup("*dummy*");
    outputbparmtd->tokenclass = BOOLCONST;
    //construct parameter TreeNode
    TreeNode *outputbparmtn = newDeclNode(ParamK, Boolean, outputbparmtd);
    //construct FuncK TreeNode, with parameter node as child
    TreeNode *outputbtn = newDeclNode(FuncK, Void, outputbtd, outputbparmtn);
    declarations.insert("outputb", outputbtn);
    //set location in TM code
    outputbtn->loc = 16;

    //void outputc(char)
    //construct TokenData of FuncK node
    TokenData *outputctd = (TokenData*)malloc(sizeof(TokenData));
    outputctd->linenum = -1;
    outputctd->tokenstr = strdup("outputc");
    outputctd->tokenclass = ID;
    //construct TokenData of char parameter
    TokenData *outputcparmtd = (TokenData*)malloc(sizeof(TokenData));
    outputcparmtd->linenum = -1;
    outputcparmtd->tokenstr = strdup("*dummy*");
    outputcparmtd->tokenclass = CHARCONST;
    //construct parameter TreeNode
    TreeNode *outputcparmtn = newDeclNode(ParamK, Char, outputcparmtd);
    //construct FuncK TreeNode, with parameter node as child
    TreeNode *outputctn = newDeclNode(FuncK, Void, outputctd, outputcparmtn);
    declarations.insert("outputc", outputctn);
    //set location in TM code
    outputctn->loc = 27;

    //int input()
    //construct TokenData of FuncK node
    TokenData *inputtd = (TokenData*)malloc(sizeof(TokenData));
    inputtd->linenum = -1;
    inputtd->tokenstr = strdup("input");
    inputtd->tokenclass = ID;
    //construct FuncK TreeNode
    TreeNode *inputtn = newDeclNode(FuncK, Integer, inputtd);
    declarations.insert("input", inputtn);
    //set location in TM code
    inputtn->loc = 0;

    //bool inputb()
    //construct TokenData of FuncK node
    TokenData *inputbtd = (TokenData*)malloc(sizeof(TokenData));
    inputbtd->linenum = -1;
    inputbtd->tokenstr = strdup("inputb");
    inputbtd->tokenclass = ID;
    //construct FuncK TreeNode
    TreeNode *inputbtn = newDeclNode(FuncK, Boolean, inputbtd);
    declarations.insert("inputb", inputbtn);
    //set location in TM code
    inputbtn->loc = 11;

    //char inputc()
    //construct TokenData of FuncK node
    TokenData *inputctd = (TokenData*)malloc(sizeof(TokenData));
    inputctd->linenum = -1;
    inputctd->tokenstr = strdup("inputc");
    inputctd->tokenclass = ID;
    //construct FuncK TreeNode
    TreeNode *inputctn = newDeclNode(FuncK, Char, inputctd);
    declarations.insert("inputc", inputctn);
    //set location in TM code
    inputctn->loc = 22;

    //void outnl()
    //construct TokenData of FuncK node
    TokenData *outnltd = (TokenData*)malloc(sizeof(TokenData));
    outnltd->linenum = -1;
    outnltd->tokenstr = strdup("outnl");
    outnltd->tokenclass = ID;
    //construct FuncK TreeNode
    TreeNode *outnltn = newDeclNode(FuncK, Void, outnltd);
    declarations.insert("outnl", outnltn);
    //set location in TM code
    outnltn->loc = 33;

    //create a tree from these
    addSibling(inputtn, inputbtn);
    addSibling(inputtn, inputctn);
    addSibling(inputtn, outputtn);
    addSibling(inputtn, outputbtn);
    addSibling(inputtn, outputctn);
    addSibling(inputtn, outnltn);
}

//traverse the tree to determine what VarK never get used as IdK, to print a warning later in semantic().
char setIsUsed (FILE *out, TreeNode *node) {
    if (node == NULL) {
        return 'n';
    }
    //printf("%s\n", node->token->tokenstr);
    fflush(out);
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
                            if (scopedepth < 1) { //if global scope
                                c[0] = 0;
                                setIsUsed(out, node->children[0]);
                                if (node->children[0] != NULL)
                                if (node->children[0]->token != NULL)
                                if (node->children[0]->token->tokenstr != NULL)
                                if (strcmp(node->token->tokenstr, node->children[0]->token->tokenstr) == 0)
                                    //edge case where if a global is used to initialize itself, it does not count as used
                                    node->isUsed = false;
                            }
                            break;

                        case FuncK:
                            if ((node->children[1] != NULL) && (node->children[1]->token->tokenclass == '{')) {
                                justInFunc = true;
                            }
                            if (strcmp(node->token->tokenstr, "main") == 0) { //main is always used
                                node->isUsed = true;
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
                                break;
                            }
                            if (g != NULL /*&& p == NULL*/) { //we can only assume the global var is being used if a local doesn't exist //EDIT: Never mind, doesn't match test output?
                                g->isUsed = true;
                            }
                            break;

                        case CallK:
                            node->isUsed = true;
                            p = (TreeNode*)temp.lookup(node->token->tokenstr); //FuncK
                            if (p == NULL) {
                                //function not declared
                                //fprintf(out, "%s\n", node->token->tokenstr);
                            }
                            else {
                                //fprintf(out, "%s %s\n", node->token->tokenstr, p->token->tokenstr);
                                p->isUsed = true;
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
            if (node->children[2] != NULL) //otherwise segfaults if nothing follows "do"
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

        case TO:
            c[0] = 0;
            r = setIsUsed(out, node->children[0]);
            c[1] = 0;
            r = setIsUsed(out, node->children[1]);
            c[2] = 0;
            if (node->children[2] != NULL) {
                r = setIsUsed(out, node->children[2]);
            }
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