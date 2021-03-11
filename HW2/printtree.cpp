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