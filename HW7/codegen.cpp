#include "emitcode.h"
#include "scanType.h"
#include "parser.tab.h"
#include "symbolTable.h"
#include <cstring>
#include <string>
#include <iostream>

using namespace std;

extern SymbolTable declarations;
extern int goffset;
int breakpoint;
bool inparam;

void gencode(TreeNode *node, int toff, bool folSib);

void emitBinary(TreeNode *node, int toff) {
    gencode(node->children[0], toff, true);
    emitRM((char*)"ST", 3, toff, 1, (char*)"store result of left");
    emitComment((char*)"TOFF dec:", toff-1);
    gencode(node->children[1], toff-1, true);
    emitRM((char*)"LD", 4, toff, 1, (char*)"load result of left");
}

void emitAss(TreeNode *node, int toff, char *op, char *comment) {
    emitComment((char*)"EXPRESSION");
    if (node->children[0]->token->tokenclass == '[') { //array
        gencode(node->children[0]->children[1], toff-1, true);
        emitRM((char*)"ST", 3, toff, 1, (char*)"Push index");
        emitComment((char*)"TOFF dec:", toff-1);
        gencode(node->children[1], toff-1, true);
        emitComment((char*)"TOFF inc:", toff);
        emitRM((char*)"LD", 4, toff, 1, (char*)"Pop index");
        if (node->children[0]->children[0]->declaredAsParm) {
            emitRM((char*)"LD", 5, node->children[0]->children[0]->offset, ((node->children[0]->children[0]->varKind == Local) || (node->children[0]->children[0]->varKind == Parameter)), (char*)"Load address of base of array", node->children[0]->children[0]->token->tokenstr);
        }
        else {
            emitRM((char*)"LDA", 5, node->children[0]->children[0]->offset, ((node->children[0]->children[0]->varKind == Local) || (node->children[0]->children[0]->varKind == Parameter)), (char*)"Load address of base of array", node->children[0]->children[0]->token->tokenstr);
        }
        emitRO((char*)"SUB", 5, 5, 4, (char*)"Compute offset of value");
        emitRM((char*)"LD", 4, 0, 5, (char*)"load lhs variable", node->children[0]->children[0]->token->tokenstr);
        emitRO(op, 3, 4, 3, (char*)"op", op);
        emitRM((char*)"ST", 3, 0, 5, (char*)"Store variable", node->children[0]->children[0]->token->tokenstr);
    }
    else { //nonarray
        gencode(node->children[1], toff-1, true);
        emitRM((char*)"LD", 4, node->children[0]->offset, node->children[0]->varKind != Global, (char*)"load lhs variable", node->children[0]->token->tokenstr);
        emitRO(op, 3, 4, 3, (char*)"op", op);
        emitRM((char*)"ST", 3, node->children[0]->offset, 0, (char*)"Store variable", node->children[0]->token->tokenstr);
    }
}

void emitReturn(char *name) {
    emitRM((char*)"LD", 3, -1, 1, (char*)"Load return address");
    emitRM((char*)"LD", 1, 0, 1, (char*)"Adjust fp");
    emitRM((char*)"JMP", 7, 0, 3, (char*)"Return");
    emitComment((char*)"END FUNCTION", name);
    emitComment();
}

int builtIn(TreeNode *node, int toff, bool emit) { //generates code for built-in functions like outnl
    if (!strcmp(node->token->tokenstr, "input")) {
        if (emit) {
            emitRO((char*)"IN", 3, 3, 3, (char*)"int input()");
        }
        return 1;
    }
    if (!strcmp(node->token->tokenstr, "output")) {
        if (emit) {
            gencode(node->children[0], toff, true);
            emitRO((char*)"OUT", 3, 3, 3, (char*)"output(int)");
        }
        return 1;
    }
    if (!strcmp(node->token->tokenstr, "inputb")) {
        if (emit) {
            emitRO((char*)"OUTB", 3, 3, 3, (char*)"bool input()");
        }
        return 1;
    }
    if (!strcmp(node->token->tokenstr, "outputb")) {
        if (emit) {
            gencode(node->children[0], toff, true);
            emitRO((char*)"OUTB", 3, 3, 3, (char*)"output(bool)");
        }
        return 1;
    }
    if (!strcmp(node->token->tokenstr, "inputc")) {
        if (emit) {
            emitRO((char*)"INC", 3, 3, 3, (char*)"char input()");
        }
        return 1;
    }
    if (!strcmp(node->token->tokenstr, "outputc")) {
        if (emit) {
            gencode(node->children[0], toff, true);
            emitRO((char*)"OUTC", 3, 3, 3, (char*)"output(char)");
        }
        return 1;
    }
    if (!strcmp(node->token->tokenstr, "outnl")) {
        if (emit) {
            emitRO((char*)"OUTNL", 3, 3, 3, (char*)"outnl()");
        }
        return 1;
    }
    return 0;
}

//generate all the tm code for the built-in functions
void genBuiltIn() {
    //input()
    emitComment((char*)"FUNCTION", (char*)"input");
    emitRM((char*)"ST", 3, -1, 1, (char*)"Store return address");
    emitRO((char*)"IN", 2, 2, 2, (char*)"Grab int input");
    emitReturn((char*)"input");
    
    //output()
    emitComment((char*)"FUNCTION", (char*)"output");
    emitRM((char*)"ST", 3, -1, 1, (char*)"Store return address");
    emitRM((char*)"LD", 3, -2, 1, (char*)"Load parameter");
    emitRO((char*)"OUT", 3, 3, 3, (char*)"Output integer");
    emitReturn((char*)"output");
    
    //inputb()
    emitComment((char*)"FUNCTION", (char*)"inputb");
    emitRM((char*)"ST", 3, -1, 1, (char*)"Store return address");
    emitRO((char*)"INB", 2, 2, 2, (char*)"Grab bool input");
    emitReturn((char*)"inputb");
    
    //outputb()
    emitComment((char*)"FUNCTION", (char*)"outputb");
    emitRM((char*)"ST", 3, -1, 1, (char*)"Store return address");
    emitRM((char*)"LD", 3, -2, 1, (char*)"Load parameter");
    emitRO((char*)"OUTB", 3, 3, 3, (char*)"Output bool");
    emitReturn((char*)"outputb");
    
    //inputc()
    emitComment((char*)"FUNCTION", (char*)"inputc");
    emitRM((char*)"ST", 3, -1, 1, (char*)"Store return address");
    emitRO((char*)"INC", 2, 2, 2, (char*)"Grab char input");
    emitReturn((char*)"inputc");
    
    //outputc()
    emitComment((char*)"FUNCTION", (char*)"outputc");
    emitRM((char*)"ST", 3, -1, 1, (char*)"Store return address");
    emitRM((char*)"LD", 3, -2, 1, (char*)"Load parameter");
    emitRO((char*)"OUTC", 3, 3, 3, (char*)"Output char");
    emitReturn((char*)"outputc");
    
    //outnl()
    emitComment((char*)"FUNCTION", (char*)"outnl");
    emitRM((char*)"ST", 3, -1, 1, (char*)"Store return address");
    emitRO((char*)"OUTNL", 3, 3, 3, (char*)"Output a newline");
    emitReturn((char*)"outnl");

    return;
}

void initGlobals(string, void *m) {
    TreeNode *node = (TreeNode*)m;
    if (node->subkind.decl == VarK && node->children[0]) {
        gencode(node->children[0], -2, true);
        emitRM((char*)"ST", 3, node->offset, 0, (char*)"initialize", node->token->tokenstr);
    }
    if (node->subkind.decl == VarK && node->isArray) {
        emitRM((char*)"LDC", 3, node->size-1, 0, (char*)"load size of array", node->token->tokenstr);
        emitRM((char*)"ST", 3, node->offset+1, 0, (char*)"store size of array", node->token->tokenstr);
    }
}

int parameterize(TreeNode *node, int offset) {
    emitComment((char*)"TOFF dec:", offset - 1);
    if (node == NULL) {
        return -1;
    }
    emitComment((char*)"Param");
    gencode(node, offset, false);
    emitRM((char*)"ST", 3, offset, 1, (char*)"Push parameter");
    emitComment((char*)"Param end");
    return parameterize(node->sibling, offset-1) - 1;
}

//traverses tree similarly to printtree and semantic
void gencode(TreeNode *node, int toff, bool folSib) {
    if (node == NULL) {
        return;
    }
    int bak, bak2, bak3, breakBack;
    char *p;

    switch(node->token->tokenclass){
        case ID:
            switch(node->nodekind) {
                case DeclK:
                    switch(node->subkind.decl) {
                        case ParamK:
                            //handled in CallK and parameterize()
                            break;
                        case VarK:
                            if (node->varKind != Global && node->varKind != LocalStatic) { //is local
                                if (node->children[0] != NULL) { //variable initialized on declaration
                                    toff--;
                                    emitComment((char*)"TOFF set:", toff);
                                    if (node->children[0]->token->tokenclass != STRINGCONST) {
                                        gencode(node->children[0], toff, true);
                                        emitRM((char*)"ST", 3, node->offset, 1, (char*)"initialize", node->token->tokenstr);
                                    }
                                }
                                if (node->isArray) {
                                    emitRM((char*)"LDC", 3, node->size-1, 6, (char*)"load size of array", node->token->tokenstr);
                                    emitRM((char*)"ST", 3, node->offset+1, 1, (char*)"store size of array", node->token->tokenstr);
                                }
                                if (node->children[0] != NULL) {
                                    if (node->children[0]->token->tokenclass == STRINGCONST) {
                                        gencode(node->children[0], toff, true);
                                    }
                                }

                            }
                            break;

                        case FuncK:
                            //if not built-in IO function
                            if (!builtIn(node, toff, false)) {
                                node->loc = emitWhereAmI() - 1;
                                emitComment((char*)"FUNCTION", node->token->tokenstr);
                                node->offset = emitSkip(0);
                                toff += node->size;
                                emitComment((char*)"TOFF set:", toff);
                                emitRM((char*)"ST", 3, -1, 1, (char*)"Store return address");
                                
                                gencode(node->children[1], node->size, true); //don't traverse param list!
                                emitComment((char*)"Add standard closing in case there is no return statement");
                                emitRM((char*)"LDC", 2, 0, 6, (char*)"Set return value to 0");
                                emitReturn(node->token->tokenstr);
                            }
                            break;

                        default:
                            break;
                    }
                    break;

                case ExpK:
                    switch(node->subkind.exp) {
                        case IdK:
                            if (node->varKind != Global) { //is local
                                if (node->isArray /*&& !node->isParam*/) {
                                    if (node->declaredAsParm) {
                                        emitRM((char*)"LD", 3, node->offset, ((node->varKind == Local) || (node->varKind == Parameter)), (char*)"Load address of base of array", node->token->tokenstr);
                                    }
                                    else {
                                        emitRM((char*)"LDA", 3, node->offset, ((node->varKind == Local) || (node->varKind == Parameter)), (char*)"Load address of base of array", node->token->tokenstr);
                                    }
                                    //printf("%i line %i\n", node->varKind, node->lineno);
                                }
                                else {
                                    emitRM((char*)"LD", 3, node->offset, ((node->varKind == Local) || (node->varKind == Parameter)), (char*)"retrieve local variable", node->token->tokenstr);
                                }
                            }
                            else { //global
                                if (node->isArray) {
                                    emitRM((char*)"LDA", 3, node->offset, 0, (char*)"Load address of base of array", node->token->tokenstr);
                                }
                                else {
                                    emitRM((char*)"LD", 3, node->offset, 0, (char*)"retrieve global variable", node->token->tokenstr);
                                }
                            }
                            break;

                        case CallK:
                            emitComment((char*)"CALL", node->token->tokenstr);
                            emitRM((char*)"ST", 1, toff, 1, (char*)"Store fp in ghost frame for output");
                            emitComment((char*)"TOFF dec:", toff - 1);
                            parameterize(node->children[0], toff - 2);
                            //emitRM((char*)"LDC", 2, 3+emitSkip(0), 0, (char*)"compute return address");
                            emitRM((char*)"LDA", 1, toff, 1, (char*)"Ghost frame becomes new active frame");
                            emitRM((char*)"LDA", 3, 1, 7, (char*)"Return address in ac");
                            emitComment((char*)"TOFF:", toff);
                            emitRM((char*)"JMP", 7, ((TreeNode*)(declarations.lookup(node->token->tokenstr)))->loc - emitWhereAmI(), 7, (char*)"CALL", node->token->tokenstr);
                            emitRM((char*)"LDA", 3, 0, 2, (char*)"Save the result in ac");
                            emitComment((char*)"Call end", node->token->tokenstr);
                            break;

                        default:
                            break;
                    }
                    break;
            }
            break;
        case '{': 
            bak = toff;
            emitComment((char*)"COMPOUND");
            toff = node->size;
            emitComment((char*)"TOFF set:", toff);
            gencode(node->children[0], toff, true);
            emitComment((char*)"Compound Body");
            gencode(node->children[1], toff, true);
            toff = bak;
            emitComment((char*)"TOFF set:", toff);
            emitComment((char*)"END COMPOUND");
            break;

        case FOR:
            emitComment((char*)"TOFF set:", toff);
            emitComment((char*)"FOR");
            gencode(node->children[0], toff, true); //should be a variable
            //toff += -2;
            gencode(node->children[1], toff, true); //should be TO
            bak = emitWhereAmI();
            gencode(node->children[2], toff-2, true); //should be a compound or something
            emitRM((char*)"LD", 3, toff, 1, (char*)"Load index");
            emitRM((char*)"LD", 5, toff-2, 1, (char*)"Load step");
            emitRO((char*)"ADD", 3, 3, 5, (char*)"increment");
            emitRM((char*)"ST", 3, toff, 1, (char*)"store back to index");
            emitRM((char*)"JMP", 7, bak - (emitWhereAmI() + 7), 7, (char*)"go to beginning of loop");
            backPatchAJumpToHere(bak - 1, (char*)"Jump past loop [backpatch]");
            break;

        case TO:
            gencode(node->children[0], toff-3, true);
            emitRM((char*)"ST", 3, toff, 1, (char*)"save starting value in index variable");
            gencode(node->children[1], toff-3, true);
            emitRM((char*)"ST", 3, toff-1, 1, (char*)"save stop value");
            if (node->children[2] != NULL) {
                gencode(node->children[2], toff-3, true); //step value
            }
            else {
                emitRM((char*)"LDC", 3, 1, 6, (char*)"default increment by 1");
            }
            emitRM((char*)"ST", 3, toff-2, 1, (char*)"save step value");
            emitRM((char*)"LD", 4, toff, 1, (char*)"loop index");
            emitRM((char*)"LD", 5, toff-1, 1, (char*)"stop value");
            emitRM((char*)"LD", 3, toff-2, 1, (char*)"step value");
            emitRO((char*)"SLT", 3, 4, 5, (char*)"Op <");
            emitRM((char*)"JNZ", 3, 1, 7, (char*)"Jump to loop body");
            emitSkip(1);
            break;

        case BY:
            //this case probably doesn't need to be here but I'm not touching it just in case.
            break;

        case WHILE:
            breakBack = breakpoint;
            emitComment((char*)"WHILE");
            bak3 = emitSkip(0);
            gencode(node->children[0], toff, true);
            bak = emitSkip(2);
            breakpoint = bak+1;
            backPatchAJumpToHere((char*)"JNZ", 3, bak, (char*)"skip jump to end of while on success [backpatched]");
            emitComment((char*)"DO");
            gencode(node->children[1], toff, true);
            emitRM((char*)"LDC", 7, bak3, 0, (char*)"go to beginning of while");
            backPatchAJumpToHere(bak+1, (char*)"jump to end of while [backpatched]");
            emitComment((char*)"END WHILE");
            breakpoint = breakBack;
            break;
        case IF:
            emitComment((char*)"IF");
            gencode(node->children[0], toff, true);
            bak = emitSkip(1);
            emitComment((char*)"THEN");
            gencode(node->children[1], toff, true);
            if(node->children[2] != NULL) {
                bak2 = emitSkip(1);
            }
            backPatchAJumpToHere((char*)"JZR", 3, bak, (char*)"jump on conditional failure [backpatched]");
            if(node->children[2] != NULL) {
                emitComment((char*)"ELSE");
                gencode(node->children[2], toff, true);
                backPatchAJumpToHere(bak2, (char*)"skip else [backpatched]");
            }
            emitComment((char*)"ENDIF");
            break;

        case BREAK:
            emitRM((char*)"LDC", 7, breakpoint, 0, (char*)"break");
            break;

        case RETURN:
            emitComment((char*)"RETURN");
            if (node->children[0] != NULL) {
                gencode(node->children[0], toff, true);
            }
            emitRM((char*)"LDA", 2, 0, 3, (char*)"Copy result to return register");
            emitRM((char*)"LD", 3, -1, 1, (char*)"Load return address");
            emitRM((char*)"LD", 1, 0, 1, (char*)"Adjust fp");
            emitRM((char*)"JMP", 7, 0, 3, (char*)"Return");
            emitComment((char*)"END RETURN");
            folSib = false;
            break;

        case '=':
            emitComment((char*)"EXPRESSION");
            if (node->children[0]->token->tokenclass == '[') {
                gencode(node->children[0]->children[1], toff-1, true);
                emitRM((char*)"ST", 3, toff, 1, (char*)"Push index");
                emitComment((char*)"TOFF dec:", toff-1);
                gencode(node->children[1], toff-1, true);
                emitComment((char*)"TOFF inc:", toff);
                emitRM((char*)"LD", 4, toff, 1, (char*)"Pop index");
                if (node->children[0]->children[0]->declaredAsParm) {
                    emitRM((char*)"LD", 5, node->children[0]->children[0]->offset, ((node->children[0]->children[0]->varKind == Local) || (node->children[0]->children[0]->varKind == Parameter)), (char*)"Load address of base of array", node->children[0]->children[0]->token->tokenstr);
                }
                else {
                    emitRM((char*)"LDA", 5, node->children[0]->children[0]->offset, ((node->children[0]->children[0]->varKind == Local) || (node->children[0]->children[0]->varKind == Parameter)), (char*)"Load address of base of array", node->children[0]->children[0]->token->tokenstr);
                }
                emitRO((char*)"SUB", 5, 5, 4, (char*)"Compute offset of value");
                emitRM((char*)"ST", 3, 0, 5, (char*)"Store variable", node->children[0]->children[0]->token->tokenstr);
            }
            else if (node->children[0]->expType == Char && node->children[0]->isArray) {
                gencode(node->children[1], toff-1, true);
                emitRM((char*)"LDA", 4, node->children[0]->offset, 1, (char*)"address of lhs");
                emitRM((char*)"LD", 5, 1, 3, (char*)"size of rhs");
                emitRM((char*)"LD", 6, 1, 4, (char*)"size of lhs");
                emitRO((char*)"SWP", 5, 6, 6, (char*)"pick smallest size");
                emitRO((char*)"MOV", 4, 3, 5, (char*)"array op =");
            }
            else { //nonarray
                gencode(node->children[1], toff, true);
                emitRM((char*)"ST", 3, node->children[0]->offset, ((node->children[0]->varKind == Local) || (node->children[0]->varKind == Parameter)), (char*)"Store variable", node->children[0]->token->tokenstr);
            }
            break;

        case ADDASS:
            emitAss(node, toff, (char*)"ADD", (char*)"add for ADDASS");
            break;
        case SUBASS:
            emitAss(node, toff, (char*)"SUB", (char*)"subtract for SUBASS");
            break;
        case MULASS:
            emitAss(node, toff, (char*)"MUL", (char*)"multiply for MULASS");
            break;
        case DIVASS:
            emitAss(node, toff, (char*)"DIV", (char*)"divide for DIVASS");
            break;
        
        case '+':
            emitBinary(node, toff);
            emitRO((char*)"ADD", 3, 3, 4, (char*)"+");
            break;
        case '-':
            emitBinary(node, toff);
            emitRO((char*)"SUB", 3, 4, 3, (char*)"-");
            break;
        case '*':
            emitBinary(node, toff);
            emitRO((char*)"MUL", 3, 4, 3, (char*)"*");
            break;
        case '/':
            emitBinary(node, toff);
            emitRO((char*)"DIV", 3, 4, 3, (char*)"/");
            break;
        case '%':
            emitBinary(node, toff);
            emitRO((char*)"MOD", 3, 4, 3, (char*)"Op %");
            break;

        case INC:
            if (node->children[0]->token->tokenclass == '['){
                //emitComment((char*)"TOFF dec:", toff-1);
                //emitBinary(node->children[0], toff-1);
                emitRM((char*)"LDC", 3, 3, 6, (char*)"Load integer constant");
                if (node->children[0]->children[0]->declaredAsParm) {
                    emitRM((char*)"LD", 5, node->children[0]->children[0]->offset, ((node->children[0]->children[0]->varKind == Local) || (node->children[0]->children[0]->varKind == Parameter)), (char*)"Load address of base of array", node->children[0]->children[0]->token->tokenstr);
                }
                else {
                    emitRM((char*)"LDA", 5, node->children[0]->children[0]->offset, ((node->children[0]->children[0]->varKind == Local) || (node->children[0]->children[0]->varKind == Parameter)), (char*)"Load address of base of array", node->children[0]->children[0]->token->tokenstr);
                }
                //printf("%i line %i\n", node->children[0]->children[0]->varKind, node->children[0]->children[0]->lineno);
                emitRO((char*)"SUB", 5, 5, 3, (char*)"Compute offset of value");
                emitRM((char*)"LD", 3, 0, 5, (char*)"load lhs variable", node->children[0]->children[0]->token->tokenstr);
                emitRO((char*)"LDA", 3, 1, 3, (char*)"increment value of", node->children[0]->children[0]->token->tokenstr);
                emitRM((char*)"ST", 3, 0, 5, (char*)"store variable", node->children[0]->children[0]->token->tokenstr);
            }
            else {
                emitRM((char*)"LD", 3, node->children[0]->offset, ((node->children[0]->varKind == Local) || (node->children[0]->varKind == Parameter)), (char*)"retrieve old value of", node->children[0]->token->tokenstr);
                emitRO((char*)"LDA", 3, 1, 3, (char*)"increment");
                emitRM((char*)"ST", 3, node->children[0]->offset, node->children[0]->varKind != Global, (char*)"Store variable", node->children[0]->token->tokenstr);
            }
            break;
        case DEC:
            if (node->children[0]->token->tokenclass == '['){
                emitRM((char*)"LDC", 3, 3, 6, (char*)"Load integer constant");
                if (node->children[0]->children[0]->declaredAsParm) {
                    emitRM((char*)"LD", 5, node->children[0]->children[0]->offset, ((node->children[0]->children[0]->varKind == Local) || (node->children[0]->children[0]->varKind == Parameter)), (char*)"Load address of base of array", node->children[0]->children[0]->token->tokenstr);
                }
                else {
                    emitRM((char*)"LDA", 5, node->children[0]->children[0]->offset, ((node->children[0]->children[0]->varKind == Local) || (node->children[0]->children[0]->varKind == Parameter)), (char*)"Load address of base of array", node->children[0]->children[0]->token->tokenstr);
                }
                //printf("%i line %i\n", node->children[0]->children[0]->varKind, node->children[0]->children[0]->lineno);
                emitRO((char*)"SUB", 5, 5, 3, (char*)"Compute offset of value");
                emitRM((char*)"LD", 3, 0, 5, (char*)"load lhs variable", node->children[0]->children[0]->token->tokenstr);
                emitRO((char*)"LDA", 3,-1, 3, (char*)"decrement value of", node->children[0]->children[0]->token->tokenstr);
                emitRM((char*)"ST", 3, 0, 5, (char*)"store variable", node->children[0]->children[0]->token->tokenstr);
            }
            else {
                emitRM((char*)"LD", 3, node->children[0]->offset, ((node->varKind == Local) || (node->varKind == Parameter)), (char*)"retrieve old value of", node->children[0]->token->tokenstr);
                emitRO((char*)"LDA", 3, -1, 3, (char*)"decrement");
                emitRM((char*)"ST", 3, node->children[0]->offset, node->children[0]->varKind != Global, (char*)"Store variable", node->children[0]->token->tokenstr);
            }
            break;

        case AND:
            emitBinary(node, toff);
            emitRO((char*)"AND", 3, 4, 3, (char*)"AND");
            break;
        case OR:
            emitBinary(node, toff);
            emitRO((char*)"OR", 3, 4, 3, (char*)"OR");
            break;

        case NOT:
            gencode(node->children[0], toff, true);
            emitRM((char*)"LDC", 4, 0, 0, (char*)"load 0");
            emitRO((char*)"TEQ", 3, 3, 4, (char*)"test equal to zero");
            break;
        
        case LEQ:
            emitBinary(node, toff);
            emitRO((char*)"TLE", 3, 4, 3, (char*)"<=");
            break;
        case GEQ:
            emitBinary(node, toff);
            emitRO((char*)"TGE", 3, 4, 3, (char*)">=");
            break;
        case '<':
            emitBinary(node, toff);
            emitRO((char*)"TLT", 3, 4, 3, (char*)"<");
            break;
        case '>':
            emitBinary(node, toff);
            emitRO((char*)"TGT", 3, 4, 3, (char*)">");
            break;

        case MIN:
        case MAX:
            
            break;

        case EQ:
            emitBinary(node, toff);
            emitRO((char*)"TEQ", 3, 4, 3, (char*)"==");
            break;
        case NEQ:
            emitBinary(node, toff);
            emitRO((char*)"TNE", 3, 4, 3, (char*)"!=");
            break;

        case CHSIGN: //unary
            gencode(node->children[0], toff, true);
            emitRM((char*)"LDC", 4, -1, 0, (char*)"load -1");
            emitRO((char*)"MUL", 3, 3, 4, (char*)"* by -1");
            break;

        case SIZEOF: //unary
            gencode(node->children[0], toff, true);
            emitRM((char*)"LD", 3, 1, 3, (char*)"get length of array");
            break;

        case '?':
            gencode(node->children[0], toff, true);
            emitRO((char*)"RND", 3, 3, 0, (char*)"get random number");
            break;

        case '[':
            emitBinary(node, toff);
            emitRO((char*)"SUB", 3, 4, 3, (char*)"compute address");
            emitRM((char*)"LD", 3, 0, 3, (char*)"get value");
            break;

        case '.':
            //nothing
            break;

        case CHARCONST:
            emitRM((char*)"LDC", 3, node->token->cvalue, 6, (char*)"Load char constant");
            break;

        case STRINGCONST:
            p = node->token->svalue;
            p++;
            p[strlen(p)-1] = 0;
            emitStrLit(node->offset, p);
            emitRM((char*)"LDA", 3, node->offset, 0, (char*)"Load address of char array");
            emitRM((char*)"LDA", 4, node->offset - 2, 1, (char*)"address of lhs"); //middle var is wrong
            emitRM((char*)"LD", 5, 1, 3, (char*)"size of rhs");
            emitRM((char*)"LD", 6, 1, 4, (char*)"size of lhs");
            emitRO((char*)"SWP", 5, 6, 6, (char*)"pick smallest size");
            emitRO((char*)"MOV", 4, 3, 5, (char*)"array op =");
            break;

        case NUMCONST:
        case BOOLCONST:
            emitRM((char*)"LDC", 3, node->token->nvalue, 6, (char*)"Load constant");
            break;

        default: //should never happen
            printf("monkaS in codegen, got node->token->tokenstr of %s\n", node->token->tokenstr);
            break;
    }
    if (folSib) {
        gencode(node->sibling, toff, true);
    }
}

void emitAll(TreeNode *node) {
    emitSkip(1);
    genBuiltIn();
    gencode(node, 0, true);
    backPatchAJumpToHere(0, (char*)"Jump to init [backpatch]");
    emitComment((char*)"INIT");
    //emitRM((char*)"LD", 0, 0, 0, (char*)"set global pointer");
    emitRM((char*)"LDA", 1, goffset, 0, (char*)"set first frame at end of globals");
    emitRM((char*)"ST", 1, 0, 1, (char*)"store old fp (point to self)");
    emitComment((char*)"INIT GLOBALS AND STATICS");
    declarations.applyToAllGlobal(initGlobals);
    emitComment((char*)"END INIT GLOBALS AND STATICS");

    emitRM((char*)"LDA", 3, 1, 7, (char*)"Return address in ac");
    emitGotoAbs(((TreeNode*)(declarations.lookup((char*)"main")))->offset, (char*)"Jump to main");

    emitRO((char*)"HALT", 0, 0, 0, (char*)"DONE!");
    emitComment((char*)"END INIT");
}