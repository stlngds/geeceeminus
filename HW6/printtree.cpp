// Greyson Biggs
// CS 445
// printtree.cpp

#include "printtree.h"

void printTree(FILE *out, TreeNode *node, int level, int sibling) {
    int i, j;
    switch(node->token->tokenclass){
        case ID:
            switch(node->nodekind) {
                case DeclK:
                    switch(node->subkind.decl) {
                        case VarK:
                            fprintf(out, "Var: %s ", node->token->tokenstr);
                            if (node->isArray) fprintf(out, "is array ");
                            fprintf(out, "of type %s [line: %d]\n", node->expstr, node->lineno);
                            break;
                        case FuncK:
                            fprintf(out, "Func: %s returns type %s [line: %d]\n", node->token->tokenstr, node->expstr, node->lineno);
                            break;
                        case ParamK:
                            fprintf(out, "Parm: %s ", node->token->tokenstr);
                            if(node->isArray) fprintf(out, "is array ");
                            fprintf(out, "of type %s [line: %d]\n", node->expstr, node->lineno);
                            break;
                        default:
                            fprintf(out, "Reached DeclK with no nodekind!\n");
                            break;
                    }
                    break;
                case ExpK:
                    switch(node->subkind.exp) {
                        case IdK:
                            fprintf(out, "Id: %s ", node->token->tokenstr);
                            if(node->isArray) fprintf(out, "is array ");
                            fprintf(out, "[line: %d]\n", node->lineno);
                            break;
                        case CallK:
                            fprintf(out, "Call: %s [line: %d]\n", node->token->tokenstr, node->lineno);
                            break;
                        default:
                            break;
                    }
                    break;
                case StmtK:
                    switch(node->subkind.stmt) {
                        case ForK: //redundant
                            fprintf(out, "For [line: %d]\n", node->lineno);
                            break;
                        case RangeK:
                            fprintf(out, "Range [line: %d]\n", node->lineno);
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            break;
        case FOR: 
            fprintf(out, "For [line: %d]\n", node->lineno);
            break;
        case TO:
            fprintf(out, "Range [line: %d]\n", node->lineno);
            break;
        case IF:
            fprintf(out, "If [line: %d]\n", node->lineno);
            break;
        case WHILE:
            fprintf(out, "While [line: %d]\n", node->lineno);
            break;
        case RETURN:
            fprintf(out, "Return [line: %d]\n", node->lineno);
            break;
        case BREAK:
            fprintf(out, "Break [line: %d]\n", node->lineno);
            break;
        case NUMCONST:
            fprintf(out, "Const of type int: %i [line: %d]\n", node->token->nvalue, node->lineno);
            break;
        case CHARCONST:
            fprintf(out, "Const of type char: '%c' [line: %d]\n", node->token->cvalue, node->lineno);
            break;
        case STRINGCONST:
            fprintf(out, "Const is array of type char: %s [line: %d]\n", node->token->tokenstr, node->lineno);
            break;
        case BOOLCONST:
            fprintf(out, "Const of type bool: %s [line: %d]\n", node->token->tokenstr, node->lineno);
            break;
        //ADDASS or SUBASS or MULASS or...
        case ADDASS:
        case SUBASS:
        case MULASS:
        case DIVASS:
        case INC:
        case DEC:
        case '=':
            fprintf(out, "Assign: %s [line: %d]\n", node->token->tokenstr, node->lineno);
            break;
        case '{':
            fprintf(out, "Compound [line: %d]\n", node->lineno);
            break;

        //edge cases testing is anal about output for
        case CHSIGN:
            fprintf(out, "Op: CHSIGN [line: %d]\n", node->lineno);
            break;
        case SIZEOF:
            fprintf(out, "Op: SIZEOF [line: %d]\n", node->lineno);
            break;
        case AND:
            fprintf(out, "Op: AND [line: %d]\n", node->lineno);
            break;
        case OR:
            fprintf(out, "Op: OR [line: %d]\n", node->lineno);
            break;
        case NOT:
            fprintf(out, "Op: NOT [line: %d]\n", node->lineno);
            break;

        default:
            fprintf(out, "Op: %s [line: %d]\n", node->token->tokenstr, node->lineno);
            break;
    }
    for (i = 0; i < 3; i++) {
        if (node->children[i] != NULL) {
            for (j = -1; j < level; j++) {
                fprintf(out, ".   ");
            }
            fprintf(out, "Child: %d  ", i);
            printTree(out, node->children[i], level+1, 0);
        }
    }

    if (node->sibling != NULL) {
        for (j = 0; j < level; j++) {
            fprintf(out, ".   ");
        }
        fprintf(out, "Sibling: %d  ", sibling+1);
        printTree(out, node->sibling, level, sibling+1);
    }
}

/////////////////////////////
/////////////////////////////

//same as above function, but prints node type information before line info
void printTreeTypes(FILE *out, TreeNode *node, int level, int sibling, int allowType) {
    int i, j;
    char *type;
    //handle/print type a little better
    if (node->expstr == NULL) { //stops segfaults on the strcmp in the next if, ugh
        type = new char[strlen("undefined type")];
        strcpy(type, "undefined type");
    }
    else if (!strcmp(node->expstr, "undefined")) {
        type = new char[strlen("undefined type")];
        strcpy(type, "undefined type");
    }
    else {
        type = new char[strlen(node->expstr)];
        strcpy(type, node->expstr);
    }
    //type is printed before each line
    switch(node->token->tokenclass){
        case ID:
            switch(node->nodekind) {
                case DeclK:
                    switch(node->subkind.decl) {
                        case VarK:
                            fprintf(out, "Var: %s ", node->token->tokenstr);
                            if (node->isArray) fprintf(out, "is array ");
                            fprintf(out, "of type %s [line: %d]\n", node->expstr, node->lineno);
                            break;
                        case FuncK:
                            fprintf(out, "Func: %s returns type %s [line: %d]\n", node->token->tokenstr, node->expstr, node->lineno);
                            break;
                        case ParamK:
                            fprintf(out, "Parm: %s ", node->token->tokenstr);
                            if(node->isArray) fprintf(out, "is array ");
                            fprintf(out, "of type %s [line: %d]\n", node->expstr, node->lineno);
                            break;
                        default:
                            fprintf(out, "Reached DeclK with no nodekind!\n");
                            break;
                    }
                    break;
                case ExpK:
                    switch(node->subkind.exp) {
                        case IdK:
                            fprintf(out, "Id: %s ", node->token->tokenstr);
                            //if(node->isArray) fprintf(out, "is array ");
                            //if (allowType)
                            fprintf(out, "of type %s [line: %d]\n", type, node->lineno);
                            //else
                            //fprintf(out, "of undefined type [line: %d]\n", node->lineno);
                            break;
                        case CallK:
                            fprintf(out, "Call: %s of type %s [line: %d]\n", node->token->tokenstr, type, node->lineno);
                            break;
                        default:
                            break;
                    }
                    break;
                case StmtK:
                    switch(node->subkind.stmt) {
                        case ForK: //redundant
                            fprintf(out, "For [line: %d]\n", node->lineno);
                            break;
                        case RangeK:
                            fprintf(out, "Range [line: %d]\n", node->lineno);
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            break;
        case FOR: 
            fprintf(out, "For [line: %d]\n", node->lineno);
            break;
        case TO:
            fprintf(out, "Range [line: %d]\n", node->lineno);
            break;
        case IF:
            fprintf(out, "If [line: %d]\n", node->lineno);
            break;
        case WHILE:
            fprintf(out, "While [line: %d]\n", node->lineno);
            break;
        case RETURN:
            fprintf(out, "Return [line: %d]\n", node->lineno);
            break;
        case BREAK:
            fprintf(out, "Break [line: %d]\n", node->lineno);
            break;
        case NUMCONST:
            fprintf(out, "Const %i of type int [line: %d]\n", node->token->nvalue, node->lineno);
            break;
        case CHARCONST:
            fprintf(out, "Const '%c' of type char [line: %d]\n", node->token->cvalue, node->lineno);
            break;
        case STRINGCONST:
            fprintf(out, "Const is array %s of type char [line: %d]\n", node->token->tokenstr, node->lineno);
            break;
        case BOOLCONST:
            fprintf(out, "Const %s of type bool [line: %d]\n", node->token->tokenstr, node->lineno);
            break;
        //ADDASS or SUBASS or MULASS or...
        case ADDASS:
        case SUBASS:
        case MULASS:
        case DIVASS:
        case INC:
        case DEC:
        case '=':
            fprintf(out, "Assign: %s of type %s [line: %d]\n", node->token->tokenstr, type, node->lineno);
            break;
        case '{':
            fprintf(out, "Compound [line: %d]\n", node->lineno);
            break;

        default:
            fprintf(out, "Op: %s ", node->token->tokenstr);
            //if (allowType)
            fprintf(out, "of type %s [line: %d]\n", type, node->lineno);
            //else
            //fprintf(out, "of undefined type [line: %d]\n", node->lineno);
            break;
            //fprintf(out, "Op: %s of type %s [line: %d]\n", node->token->tokenstr, type, node->lineno);
            //break;
    }
    delete[] type; //free memory
    for (i = 0; i < 3; i++) {
        if (node->children[i] != NULL) {
            for (j = -1; j < level; j++) {
                fprintf(out, ".   ");
            }
            fprintf(out, "Child: %d  ", i);
            if (node->token->tokenclass == TO)
            printTreeTypes(out, node->children[i], level+1, 0, 0);
            else
            printTreeTypes(out, node->children[i], level+1, 0, allowType);
        }
    }
    if (node->sibling != NULL) {
        for (j = 0; j < level; j++) {
            fprintf(out, ".   ");
        }
        fprintf(out, "Sibling: %d  ", sibling+1);
        fflush(out);
        printTreeTypes(out, node->sibling, level, sibling+1);
    }

}

/////////////////////////////
/////////////////////////////

void printTreeOffsets(FILE *out, TreeNode *node, int level, int sibling) {
    int i, j;
    char *type;
    char *memtype;

    if (node->expstr == NULL) { //stops segfaults on the strcmp in the next if, ugh
        type = new char[strlen("undefined type")];
        strcpy(type, "undefined type");
    }
    else if (!strcmp(node->expstr, "undefined")) {
        type = new char[strlen("undefined type")];
        strcpy(type, "undefined type");
    }
    else {
        type = new char[strlen(node->expstr)];
        strcpy(type, node->expstr);
    }

    //varKind/memtype
    if (node->varKind == None) {
        memtype = new char[strlen("None")];
        strcpy(memtype, "None");
    }
    else if (node->varKind == Global) {
        memtype = new char[strlen("Global")];
        strcpy(memtype, "Global");
    }
    else if (node->varKind == Local) {
        memtype = new char[strlen("Local")];
        strcpy(memtype, "Local");
    }
    else if (node->varKind == LocalStatic) {
        memtype = new char[strlen("LocalStatic")];
        strcpy(memtype, "LocalStatic");
    }
    else if (node->varKind == Parameter) {
        memtype = new char[strlen("Parameter")];
        strcpy(memtype, "Parameter");
    }
    else { //should never happen
        memtype = new char[strlen("NONE")];
        strcpy(memtype, "NONE");
    }

    switch(node->token->tokenclass){
        case ID:
            switch(node->nodekind) {
                case DeclK:
                    switch(node->subkind.decl) {
                        case VarK:
                            fprintf(out, "Var: %s of ", node->token->tokenstr);
                            if (node->isStatic) fprintf(out, "static ");
                            if (node->isArray) fprintf(out, "array of ");
                            fprintf(out, "type %s [mem: %s loc: %d size: %d] [line: %d]\n", node->expstr, memtype, node->offset, node->size, node->lineno);
                            break;
                        case FuncK:
                            fprintf(out, "Func: %s returns type %s [mem: %s loc: %d size: %d] [line: %d]\n", node->token->tokenstr, node->expstr, memtype, node->offset, node->size, node->lineno);
                            break;
                        case ParamK:
                            fprintf(out, "Parm: %s of ", node->token->tokenstr);
                            if (node->isArray) fprintf(out, "array of ");
                            fprintf(out, "type %s [mem: %s loc: %d size: %d] [line: %d]\n", node->expstr, memtype, node->offset, node->size, node->lineno);
                            break;
                        default:
                            fprintf(out, "Reached DeclK with no nodekind!\n");
                            break;
                    }
                    break;
                case ExpK:
                    switch(node->subkind.exp) {
                        case IdK:
                            fprintf(out, "Id: %s of ", node->token->tokenstr);
                            if (node->isStatic) fprintf(out, "static ");
                            if (node->isArray) fprintf(out, "array of ");
                            fprintf(out, "type %s [mem: %s loc: %d size: %d] [line: %d]\n", type, memtype, node->offset, node->size, node->lineno);
                            break;
                        case CallK:
                            fprintf(out, "Call: %s of type %s [line: %d]\n", node->token->tokenstr, type, node->lineno);
                            break;
                        default:
                            break;
                    }
                    break;
                case StmtK:
                    switch(node->subkind.stmt) {
                        case ForK: //redundant
                            fprintf(out, "For [line: %d]\n", node->lineno);
                            break;
                        case RangeK:
                            fprintf(out, "Range [line: %d]\n", node->lineno);
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            break;
        case FOR: 
            fprintf(out, "For [line: %d]\n", node->lineno);
            break;
        case TO:
            fprintf(out, "Range [line: %d]\n", node->lineno);
            break;
        case IF:
            fprintf(out, "If [line: %d]\n", node->lineno);
            break;
        case WHILE:
            fprintf(out, "While [line: %d]\n", node->lineno);
            break;
        case RETURN:
            fprintf(out, "Return [line: %d]\n", node->lineno);
            break;
        case BREAK:
            fprintf(out, "Break [line: %d]\n", node->lineno);
            break;
        case NUMCONST:
            fprintf(out, "Const %i of type int [line: %d]\n", node->token->nvalue, node->lineno);
            break;
        case CHARCONST:
            fprintf(out, "Const '%c' of type char [line: %d]\n", node->token->cvalue, node->lineno);
            break;
        case STRINGCONST:
            fprintf(out, "Const %s of array of type char [mem: %s loc: %d size: %d] [line: %d]\n", node->token->tokenstr, memtype, node->offset, node->size, node->lineno);
            break;
        case BOOLCONST:
            fprintf(out, "Const %s of type bool [line: %d]\n", node->token->tokenstr, node->lineno);
            break;
        //ADDASS or SUBASS or MULASS or...
        case ADDASS:
        case SUBASS:
        case MULASS:
        case DIVASS:
        case INC:
        case DEC:
        case '=':
            fprintf(out, "Assign: %s of ", node->token->tokenstr);
            if (node->isArray) fprintf(out, "array of ");
            fprintf(out, "type %s [line: %d]\n", type, node->lineno);
            break;
        case '{':
            fprintf(out, "Compound [mem: None loc: 0 size: %d] [line: %d]\n", node->size, node->lineno);
            break;

        default:
            fprintf(out, "Op: %s ", node->token->tokenstr);
            //if (allowType)
            fprintf(out, "of type %s [line: %d]\n", type, node->lineno);
            //else
            //fprintf(out, "of undefined type [line: %d]\n", node->lineno);
            break;
            //fprintf(out, "Op: %s of type %s [line: %d]\n", node->token->tokenstr, type, node->lineno);
            //break;
    }
    delete[] type; //free memory
    for (i = 0; i < 3; i++) {
        if (node->children[i] != NULL) {
            for (j = -1; j < level; j++) {
                fprintf(out, ".   ");
            }
            fprintf(out, "Child: %d  ", i);
            if (node->token->tokenclass == TO)
            printTreeOffsets(out, node->children[i], level+1, 0);
            else
            printTreeOffsets(out, node->children[i], level+1, 0);
        }
    }
    if (node->sibling != NULL) {
        for (j = 0; j < level; j++) {
            fprintf(out, ".   ");
        }
        fprintf(out, "Sibling: %d  ", sibling+1);
        fflush(out);
        printTreeOffsets(out, node->sibling, level, sibling+1);
    }

}