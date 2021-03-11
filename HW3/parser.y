%{
// Greyson Biggs
// CS 445
// 2/18/2021
// parser.y

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include "scanType.h"   //defines TreeNode
#include "ourgetopt.h"
#include "printtree.h"
#include "semantic.h"

extern int yylex();
extern FILE *yyin;
extern int yydebug;
extern int line;
extern int numErrors, numWarnings;
extern SymbolTable declarations;
extern SymbolTable temp;

extern int ourGetopt(int, char**, char*);
extern int optind;
extern char *optarg;

TreeNode *syntaxTree;

#define YYERROR_VERBOSE

//Prototypes

TreeNode *newDeclNode(DeclKind kind,
                      ExpType type,
                      TokenData *token=NULL,
                      TreeNode *c0=NULL,
                      TreeNode *c1=NULL,
                      TreeNode *c2=NULL);  // save TokenData block!!
TreeNode *newStmtNode(StmtKind kind,
                      TokenData *token,
                      TreeNode *c0=NULL,
                      TreeNode *c1=NULL,
                      TreeNode *c2=NULL);
TreeNode *newExpNode(ExpKind kind,
                     TokenData *token,
                     TreeNode *c0=NULL,
                     TreeNode *c1=NULL,
                     TreeNode *c2=NULL);


//Tree stuff

//TreeNode Constructors
//DeclNode Constructor
TreeNode *newDeclNode(DeclKind kind, ExpType type, TokenData *token, TreeNode *c0, TreeNode *c1, TreeNode *c2) {
    TreeNode *temp = (TreeNode*)malloc(sizeof(TreeNode));
    temp->children[0] = c0;
    temp->children[1] = c1;
    temp->children[2] = c2;
    temp->sibling = NULL;
    temp->token = token;
    if (token != NULL) temp->lineno = token->linenum;
    temp->nodekind = DeclK;
    temp->subkind.decl = kind;
    temp->expType = type;
    switch (temp->expType) {
        case Void:
            temp->expstr = strdup("void");
            break;
        case Integer:
            temp->expstr = strdup("int");
            break;
        case Boolean:
            temp->expstr = strdup("bool");
            break;
        case Char:
            temp->expstr = strdup("char");
            break;
        case UndefinedType:
            temp->expstr = strdup("undefined");
            break;
        default:
            temp->expstr = strdup("undefined");
            break;
    }
    //isArray
    //isStatic
    return temp;
}
//StmtNode Constructor
TreeNode *newStmtNode(StmtKind kind, TokenData *token, TreeNode *c0, TreeNode *c1, TreeNode *c2) {
    TreeNode *temp = (TreeNode*)malloc(sizeof(TreeNode));
    temp->children[0] = c0;
    temp->children[1] = c1;
    temp->children[2] = c2;
    temp->sibling = NULL;
    temp->token = token;
    if (token != NULL) temp->lineno = token->linenum;
    temp->nodekind = StmtK;
    temp->subkind.stmt = kind;
    temp->expType = UndefinedType;
    //isArray
    //isStatic
    return temp;
}
//ExpNode Constructor
TreeNode *newExpNode(ExpKind kind, TokenData *token, TreeNode *c0, TreeNode *c1, TreeNode *c2) {
    TreeNode *temp = (TreeNode*)malloc(sizeof(TreeNode));
    temp->children[0] = c0;
    temp->children[1] = c1;
    temp->children[2] = c2;
    temp->sibling = NULL;
    temp->token = token;
    if (token != NULL) temp->lineno = token->linenum;
    temp->nodekind = ExpK;
    temp->subkind.exp = kind;
    temp->expType = UndefinedType;
    //isArray
    //isStatic
    return temp;
}

//Classed TokenData Constructor
//unused atm
TokenData *newData(int tclass) {
    TokenData *temp = (TokenData*)malloc(sizeof(TokenData));
    temp->linenum = -1;
    temp->cvalue = 0;
    temp->nvalue = 0;
    temp->svalue = strdup("");
    switch(tclass){
        case 0:
            temp->tokenclass = INT;
            temp->tokenstr = strdup("int");
            break;
        case 1:
            temp->tokenclass = BOOL;
            temp->tokenstr = strdup("bool");
            break;
        case 2:
            temp->tokenclass = CHAR;
            temp->tokenstr = strdup("char");
            break;
        default:
            printf("monkaS in newData constructor. tclass not 0, 1, or 2.\n");
            break;
    }
}

// add a TreeNode s to a list of t's siblings.
// returns t on success
// Adding a NULL to the list is probably a programming error!
TreeNode *addSibling(TreeNode *t, TreeNode *s)
{
    if (s==NULL) {
        printf("ERROR(SYSTEM): never add a NULL to a sibling list.\n");
        exit(1);
    }
    if (t!=NULL) { 
        TreeNode *tmp;

        tmp = t;
        while (tmp->sibling!=NULL) tmp = tmp->sibling;
        tmp->sibling = s; 
        return t;
    }
    return s;
}

// pass the static and type attribute down the sibling list
void setType(TreeNode *t, ExpType type, bool isStatic = false)
{
    while (t) {
        t->expType = type;
        t->isStatic = isStatic;
        switch (t->expType) {
            case Void:
                t->expstr = strdup("void");
                break;
            case Integer:
                t->expstr = strdup("int");
                break;
            case Boolean:
                t->expstr = strdup("bool");
                break;
            case Char:
                t->expstr = strdup("char");
                break;
            case UndefinedType:
                t->expstr = strdup("undefined");
                break;
            default:
                t->expstr = strdup("undefined");
                break;
        }
        t = t->sibling;
    }
}

void yyerror(const char *msg)
{
    printf("ERROR(%d): %s\n", line, msg);
    numErrors++;
}


%}


%union {
    ExpType type;
    TokenData *tokenData;
    TreeNode *tNode;
    double value;
}

%type <tNode> program declList decl
%type <tNode> varDecl scopedVarDecl varDeclList varDeclInit varDeclId
%type <tNode> funDecl parms parmList parmTypeList parmIdList parmId
%type <tNode> stmt matched unmatched expStmt compoundStmt iterRange localDecls stmtList returnStmt breakStmt
%type <tNode> exp simpleExp andExp unaryRelExp relExp minmaxExp sumExp mulExp
%type <tNode> unaryExp factor mutable immutable call args argList constant

%type <type> typeSpec

%type <tokenData> relop sumop mulop unaryop minmaxop
%token <tokenData> DEC INC MIN MAX NOT AND OR TRUE FALSE
%token <tokenData> IF THEN ELSE BY TO FOR DO WHILE RETURN BREAK
%token <tokenData> BOOL CHAR INT STATIC EQ GEQ LEQ NEQ
%token <tokenData> ID NUMCONST CHARCONST STRINGCONST BOOLCONST ADDASS SUBASS MULASS DIVASS
%token <tokenData> SINGLEOP '[' ']' '{' '}' '(' ')' '<' '>' '=' '+' '-' '*' '/' '%' '?' ':' ';' ',' SIZEOF CHSIGN


///////////////////////////////////////////////////
///////////////////////////////////////////////////

%%

program
    :   declList                                {syntaxTree = $1;}
;
declList
    :   declList decl                           {addSibling($1, $2); $$ = $1;}
    |   decl                                    {$$ = $1;}
;
decl
    :   varDecl                                 {$$ = $1;}
    |   funDecl                                 {$$ = $1;}
;


varDecl
    :   typeSpec varDeclList ';'                {$$ = $2; setType($2, $1, FALSE);}
;
scopedVarDecl
    :   STATIC typeSpec varDeclList ';'         {$$ = $3; setType($3, $2, TRUE);}
    |   typeSpec varDeclList ';'                {$$ = $2; setType($2, $1, FALSE);}
;
varDeclList
    :   varDeclList ',' varDeclInit             {addSibling($1, $3); $$ = $1;}
    |   varDeclInit                             {$$ = $1;}
;
varDeclInit
    :   varDeclId                               {$$ = $1;}
    |   varDeclId ':' simpleExp                 {$$ = $1; $$->children[0] = $3; $1->isInitialized = true;}
;
varDeclId
    :   ID                                      {$$ = newDeclNode(VarK, UndefinedType, $1);}
    |   ID '[' NUMCONST ']'                     {$$ = newDeclNode(VarK, UndefinedType, $1); $$->isArray = true;}
;
typeSpec
    :   INT                                     {$$ = Integer;}
    |   BOOL                                    {$$ = Boolean;}
    |   CHAR                                    {$$ = Char;}
;


funDecl
    :   typeSpec ID '(' parms ')' stmt          {$$ = newDeclNode(FuncK, $1, $2, $4, $6);}
    |   ID '(' parms ')' stmt                   {$$ = newDeclNode(FuncK, Void, $1, $3, $5);}
;
parms
    :   parmList                                {$$ = $1;}
    |                                           {$$ = NULL;}
;
parmList
    :   parmList ';' parmTypeList               {$$ = $1; addSibling($1, $3);}
    |   parmTypeList                            {$$ = $1;}
;
parmTypeList
    :   typeSpec parmIdList                     {$$ = $2; setType($2, $1, FALSE);}
;
parmIdList
    :   parmIdList ',' parmId                   {$$ = $1; addSibling($1, $3);}
    |   parmId                                  {$$ = $1;}
;
parmId
    :   ID                                      {$$ = newDeclNode(ParamK, UndefinedType, $1);}
    |   ID '[' ']'                              {$$ = newDeclNode(ParamK, UndefinedType, $1); $$->isArray = true;}
;


stmt
    :   matched                                 {$$ = $1;}
    |   unmatched                               {$$ = $1;}
;
expStmt
    :   exp ';'                                 {$$ = $1;}
    |   ';'                                     {$$ = NULL;}
;
compoundStmt
    :   '{' localDecls stmtList '}'             {$$ = newStmtNode(CompoundK, $1, $2, $3);}
;
localDecls
    :   localDecls scopedVarDecl                {if ($1 == NULL) {$$ = $2;} else {$$ = $1;} addSibling($1, $2);}
    |                                           {$$ = NULL;}
;
stmtList
    :   stmtList stmt                           {if ($1 == NULL) {$$ = $2;} else {$$ = $1; addSibling($1, $2);}}
    |                                           {$$ = NULL;}
;
matched
    :   IF simpleExp THEN matched ELSE matched  {$$ = newStmtNode(IfK, $1, $2, $4, $6);}
    |   WHILE simpleExp DO matched              {$$ = newStmtNode(WhileK, $1, $2, $4);}
    |   FOR ID '=' iterRange DO matched         {$$ = newStmtNode(ForK, $1, newDeclNode(VarK, Integer, $2), $4, $6);}
    |   expStmt                                 {$$ = $1;}
    |   compoundStmt                            {$$ = $1;}
    |   returnStmt                              {$$ = $1;}
    |   breakStmt                               {$$ = $1;}
;
unmatched
    :   IF simpleExp THEN matched                   {$$ = newStmtNode(IfK, $1, $2, $4);}
    |   IF simpleExp THEN unmatched                 {$$ = newStmtNode(IfK, $1, $2, $4);}
    |   IF simpleExp THEN matched ELSE unmatched    {$$ = newStmtNode(IfK, $1, $2, $4, $6);}
    |   WHILE simpleExp DO unmatched                {$$ = newStmtNode(WhileK, $1, $2, $4);}
    |   FOR ID '=' iterRange DO unmatched           {$$ = newStmtNode(ForK, $1, newDeclNode(VarK, Integer, $2), $4, $6);}
;
iterRange
    :   simpleExp TO simpleExp                  {$$ = newStmtNode(RangeK, $2, $1, $3);}
    |   simpleExp TO simpleExp BY simpleExp     {$$ = newStmtNode(RangeK, $2, $1, $3, $5);}
;
returnStmt
    :   RETURN ';'                              {$$ = newStmtNode(ReturnK, $1);}
    |   RETURN exp ';'                          {$$ = newStmtNode(ReturnK, $1, $2);}
;
breakStmt
    :   BREAK ';'                               {$$ = newStmtNode(BreakK, $1);}
;


exp
    :   mutable '=' exp                         {$$ = newExpNode(AssignK, $2, $1, $3);}
    |   mutable ADDASS exp                      {$$ = newExpNode(AssignK, $2, $1, $3);}
    |   mutable SUBASS exp                      {$$ = newExpNode(AssignK, $2, $1, $3);}
    |   mutable MULASS exp                      {$$ = newExpNode(AssignK, $2, $1, $3);}
    |   mutable DIVASS exp                      {$$ = newExpNode(AssignK, $2, $1, $3);}
    |   mutable INC                             {$$ = newExpNode(AssignK, $2, $1);}
    |   mutable DEC                             {$$ = newExpNode(AssignK, $2, $1);}
    |   simpleExp                               {$$ = $1;}
;
simpleExp
    :   simpleExp OR andExp                     {$$ = newExpNode(OpK, $2, $1, $3);}
    |   andExp                                  {$$ = $1;}
;
andExp
    :   andExp AND unaryRelExp                  {$$ = newExpNode(OpK, $2, $1, $3);}
    |   unaryRelExp                             {$$ = $1;}
;
unaryRelExp
    :   NOT unaryRelExp                         {$$ = newExpNode(OpK, $1, $2);}
    |   relExp                                  {$$ = $1;}
;
relExp
    :   minmaxExp relop minmaxExp               {$$ = newExpNode(OpK, $2, $1, $3);}
    |   minmaxExp                               {$$ = $1;}
;
relop
    :   LEQ                                     {$$ = $1;}
    |   '<'                                     {$$ = $1;}
    |   '>'                                     {$$ = $1;}
    |   GEQ                                     {$$ = $1;}
    |   EQ                                      {$$ = $1;}
    |   NEQ                                     {$$ = $1;}
;
minmaxExp
    :   minmaxExp minmaxop sumExp               {$$ = newExpNode(OpK, $2, $1, $3);}
    |   sumExp                                  {$$ = $1;}
;
minmaxop
    :   MIN                                     {$$ = $1;}
    |   MAX                                     {$$ = $1;}
;
sumExp
    :   sumExp sumop mulExp                     {$$ = newExpNode(OpK, $2, $1, $3);}
    |   mulExp                                  {$$ = $1;}
;
sumop
    :   '+'                                     {$$ = $1;}
    |   '-'                                     {$$ = $1;}
;
mulExp
    :   mulExp mulop unaryExp                   {$$ = newExpNode(OpK, $2, $1, $3);}
    |   unaryExp                                {$$ = $1;}
;
mulop
    :   '*'                                     {$$ = $1;}
    |   '/'                                     {$$ = $1;}
    |   '%'                                     {$$ = $1;}
;
unaryExp
    :   unaryop unaryExp                        {$$ = newExpNode(OpK, $1, $2);}
    |   factor                                  {$$ = $1;}
;
unaryop
    :   '-'                                     {$$ = $1; $$->tokenclass = CHSIGN;}
    |   '*'                                     {$$ = $1; $$->tokenclass = SIZEOF;}
    |   '?'                                     {$$ = $1;}
;
factor
    :   immutable                               {$$ = $1;}
    |   mutable                                 {$$ = $1;}
;
mutable
    :   ID                                      {$$ = newExpNode(IdK, $1);}
    |   mutable '[' exp ']'                     {$$ = newExpNode(OpK, $2, $1, $3);}
;
immutable
    :   '(' exp ')'                             {$$ = $2;}
    |   call                                    {$$ = $1;}
    |   constant                                {$$ = $1;}
;
call
    :   ID '(' args ')'                         {$$ = newExpNode(CallK, $1, $3);}
;
args
    :   argList                                 {$$ = $1;}
    |                                           {$$ = NULL;}
;
argList
    :   argList ',' exp                         {$$ = $1; addSibling($1, $3);}
    |   exp                                     {$$ = $1;}
;
constant
    :   NUMCONST                                {$$ = newExpNode(ConstantK, $1);}
    |   CHARCONST                               {$$ = newExpNode(ConstantK, $1);}
    |   STRINGCONST                             {$$ = newExpNode(ConstantK, $1);}
    |   BOOLCONST                               {$$ = newExpNode(ConstantK, $1);}
;
%%

///////////////////////////////////////////////////
///////////////////////////////////////////////////

int main(int argc, char *argv[]) 
{
    //init
    int c;
    int pflag, Pflag, errflag;
    char *ofile;
    pflag = 0;
    errflag = 0;

    //Command-line options handling
    while (1) {
        while ((c = ourGetopt(argc, argv, (char *)"dDpP")) != EOF)
            switch (c) {
                case 'd':
                    //printf("Detected yydebug.\n");
                    yydebug = 1;
                    break;
                case 'D':
                    declarations.debug(true);
                    temp.debug(true);
                    break;
                case 'p':
                    //printf("Detected pflag.\n");
                    pflag = 1;
                    break;
                case 'P':
                    Pflag = 1;
                    break;
                case '?':
                    //printf("Detected errflag.\n");
                    errflag = 1;
                    break;
                default:
                    break;
            }
        
/*
    if (argc > 1) {
        if ((yyin = fopen(argv[1], "r"))) {
            // file open successful
        }
        else {
            // failed to open file
            printf("ERROR: failed to open \'%s\'\n", argv[1]);
            exit(1);
        }
    }
*/
        if (errflag == 1) {
            printf("usage: c- [options] [sourceFile]\noptions:\n-d\t- turn on parser debugging\n-D\t- turn on symbol table debugging\n-h\t- this usage message\n-p\t- print the abstract syntax tree\n-P\t- print the abstract syntax tree plus type information\n");
            exit(2);
        }

        if (optind < argc) {
            //(void)printf("file: %s\n", argv[optind]);
            yyin = fopen(argv[optind], "r");
            optind++;
        }
        else {
            break;
        }
    }

    yyparse();
    //printf("yyparse complete!\n");

    if (pflag == 1)
        {
            printTree(stdout, syntaxTree, 0, 0);
        }
        
    if (numErrors == 0) {
        setIsUsed(stdout, syntaxTree); //mark variables that aren't used for future warnings
        semantic(stdout, syntaxTree);
        if (declarations.lookupGlobal("main") == NULL) {
            printf("ERROR(LINKER): A function named 'main()' must be defined.\n");
            fflush(stdout);
            numErrors++;
        }
        if (Pflag == 1 && numErrors == 0) {
            printTreeTypes(stdout, syntaxTree, 0, 0);
        }
        

    }

    printf("Number of warnings: %d\n", numWarnings);
    printf("Number of errors: %d\n", numErrors);


    return 0;
}