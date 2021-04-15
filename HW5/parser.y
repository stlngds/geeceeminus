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
#include "yyerror.h"

#define YYERROR_VERBOSE

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
    if (s == NULL && numErrors == 0) {
        printf("ERROR(SYSTEM): never add a NULL to a sibling list.\n");
        fflush(stdout);
        printf("%s\n", t->token->tokenstr);
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

%type <tokenData> assignop relop sumop mulop unaryop minmaxop
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
    |   error                                   {$$ = NULL;}
;


varDecl
    :   typeSpec varDeclList ';'                {$$ = $2; setType($2, $1, false); yyerrok;}
    |   error varDeclList ';'                   {$$ = NULL; yyerrok;}
    |   typeSpec error ';'                      {$$ = NULL; yyerrok; yyerrok;}
;
scopedVarDecl
    :   STATIC typeSpec varDeclList ';'         {$$ = $3; setType($3, $2, true); yyerrok;}
    |   typeSpec varDeclList ';'                {$$ = $2; setType($2, $1, false); yyerrok;}
;
varDeclList
    :   varDeclList ',' varDeclInit             {addSibling($1, $3); $$ = $1; yyerrok;}
    |   varDeclInit                             {$$ = $1;}
    |   varDeclList ',' error                   {$$ = NULL;}
    |   error                                   {$$ = NULL;}
;
varDeclInit
    :   varDeclId                               {$$ = $1;}
    |   varDeclId ':' simpleExp                 {$$ = $1; if ($$ != NULL) {$$->children[0] = $3; $1->isInitialized = true;}}
    |   error ':' simpleExp                     {$$ = NULL; yyerrok;}
;
varDeclId
    :   ID                                      {$$ = newDeclNode(VarK, UndefinedType, $1);}
    |   ID '[' NUMCONST ']'                     {$$ = newDeclNode(VarK, UndefinedType, $1); $$->isArray = true;}
    |   ID '[' error                            {$$ = NULL;}
    |   error ']'                               {$$ = NULL; yyerrok;}
;
typeSpec
    :   INT                                     {$$ = Integer;}
    |   BOOL                                    {$$ = Boolean;}
    |   CHAR                                    {$$ = Char;}
;


funDecl
    :   typeSpec ID '(' parms ')' stmt          {$$ = newDeclNode(FuncK, $1, $2, $4, $6);}
    |   ID '(' parms ')' stmt                   {$$ = newDeclNode(FuncK, Void, $1, $3, $5);}
    |   typeSpec error                          {$$ = NULL;}
    |   typeSpec ID '(' error                   {$$ = NULL;}
    |   ID '(' error                            {$$ = NULL;}
    |   ID '(' parms ')' error                  {$$ = NULL;}
;
parms
    :   parmList                                {$$ = $1;}
    |                                           {$$ = NULL;}
;
parmList
    :   parmList ';' parmTypeList               {$$ = $1; addSibling($1, $3);}
    |   parmTypeList                            {$$ = $1;}
    |   parmList ';' error                      {$$ = NULL;}
    |   error                                   {$$ = NULL;}
;
parmTypeList
    :   typeSpec parmIdList                     {$$ = $2; setType($2, $1, false);}
    |   typeSpec error                          {$$ = NULL;}
;
parmIdList
    :   parmIdList ',' parmId                   {$$ = $1; addSibling($1, $3); yyerrok;}
    |   parmId                                  {$$ = $1;}
    |   parmIdList ',' error                    {$$ = NULL;}
    |   error                                   {$$ = NULL;}
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
    |   error ';'                               {$$ = NULL; yyerrok;}
;
compoundStmt
    :   '{' localDecls stmtList '}'             {$$ = newStmtNode(CompoundK, $1, $2, $3); yyerrok;}
;
localDecls
    :   localDecls scopedVarDecl                {if ($1 == NULL) {$$ = $2;} else {$$ = $1;} addSibling($1, $2);}
    |                                           {$$ = NULL;}
;
stmtList
    :   stmtList stmt                           {if ($1 == NULL) {$$ = $2;} else {$$ = $1;} addSibling($1, $2);}
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
    |   IF error                                {$$ = NULL;}
    |   IF error ELSE matched                   {$$ = NULL; yyerrok;}
    |   IF error THEN matched ELSE matched      {$$ = NULL; yyerrok;}
    |   WHILE error DO matched                  {$$ = NULL; yyerrok;}
    |   WHILE error                             {$$ = NULL;}
    |   FOR ID '=' error DO matched             {$$ = NULL; yyerrok;}
    |   FOR error                               {$$ = NULL;}
;
unmatched
    :   IF simpleExp THEN matched                   {$$ = newStmtNode(IfK, $1, $2, $4);}
    |   IF simpleExp THEN unmatched                 {$$ = newStmtNode(IfK, $1, $2, $4);}
    |   IF simpleExp THEN matched ELSE unmatched    {$$ = newStmtNode(IfK, $1, $2, $4, $6);}
    |   WHILE simpleExp DO unmatched                {$$ = newStmtNode(WhileK, $1, $2, $4);}
    |   FOR ID '=' iterRange DO unmatched           {$$ = newStmtNode(ForK, $1, newDeclNode(VarK, Integer, $2), $4, $6);}
    |   IF error THEN stmt                          {$$ = NULL; yyerrok;}
    |   IF error THEN matched ELSE unmatched        {$$ = NULL; yyerrok;}
;
iterRange
    :   simpleExp TO simpleExp                  {$$ = newStmtNode(RangeK, $2, $1, $3);}
    |   simpleExp TO simpleExp BY simpleExp     {$$ = newStmtNode(RangeK, $2, $1, $3, $5);}
    |   simpleExp TO error                      {$$ = NULL;}
    |   error BY error                          {$$ = NULL; yyerrok;}
    |   simpleExp TO simpleExp BY error         {$$ = NULL;}
;
returnStmt
    :   RETURN ';'                              {$$ = newStmtNode(ReturnK, $1);}
    |   RETURN exp ';'                          {$$ = newStmtNode(ReturnK, $1, $2); yyerrok;}
    |   RETURN error ';'                        {$$ = NULL; yyerrok;}
;
breakStmt
    :   BREAK ';'                               {$$ = newStmtNode(BreakK, $1);}
;


exp
    :   mutable assignop exp                    {$$ = newExpNode(AssignK, $2, $1, $3);}
    |   mutable INC                             {$$ = newExpNode(AssignK, $2, $1);}
    |   mutable DEC                             {$$ = newExpNode(AssignK, $2, $1);}
    |   simpleExp                               {$$ = $1;}
    |   error assignop exp                      {$$ = NULL; yyerrok;}
    |   mutable assignop error                  {$$ = NULL;}
    |   error INC                               {$$ = NULL; yyerrok;}
    |   error DEC                               {$$ = NULL; yyerrok;}
;
assignop
    :   '='                                     {$$ = $1;}
    |   ADDASS                                  {$$ = $1;}
    |   SUBASS                                  {$$ = $1;}
    |   MULASS                                  {$$ = $1;}
    |   DIVASS                                  {$$ = $1;}
;
simpleExp
    :   simpleExp OR andExp                     {$$ = newExpNode(OpK, $2, $1, $3);}
    |   andExp                                  {$$ = $1;}
    |   simpleExp OR error                      {$$ = NULL;}
;
andExp
    :   andExp AND unaryRelExp                  {$$ = newExpNode(OpK, $2, $1, $3);}
    |   unaryRelExp                             {$$ = $1;}
    |   andExp AND error                        {$$ = NULL;}
;
unaryRelExp
    :   NOT unaryRelExp                         {$$ = newExpNode(OpK, $1, $2);}
    |   relExp                                  {$$ = $1;}
    |   NOT error                               {$$ = NULL;}
;
relExp
    :   minmaxExp relop minmaxExp               {$$ = newExpNode(OpK, $2, $1, $3);}
    |   minmaxExp                               {$$ = $1;}
    |   minmaxExp relop error                   {$$ = NULL;}
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
    |   sumExp sumop error                      {$$ = NULL;}
;
sumop
    :   '+'                                     {$$ = $1;}
    |   '-'                                     {$$ = $1;}
;
mulExp
    :   mulExp mulop unaryExp                   {$$ = newExpNode(OpK, $2, $1, $3);}
    |   unaryExp                                {$$ = $1;}
    |   mulExp mulop error                      {$$ = NULL;}
;
mulop
    :   '*'                                     {$$ = $1;}
    |   '/'                                     {$$ = $1;}
    |   '%'                                     {$$ = $1;}
;
unaryExp
    :   unaryop unaryExp                        {$$ = newExpNode(OpK, $1, $2);}
    |   factor                                  {$$ = $1;}
    |   unaryop error                           {$$ = NULL;}
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
    :   '(' exp ')'                             {$$ = $2; yyerrok;}
    |   call                                    {$$ = $1;}
    |   constant                                {$$ = $1;}
    |   '(' error                               {$$ = NULL;}
;
call
    :   ID '(' args ')'                         {$$ = newExpNode(CallK, $1, $3);}
    |   error '('                               {$$ = NULL; yyerrok;}
;
args
    :   argList                                 {$$ = $1;}
    |                                           {$$ = NULL;}
;
argList
    :   argList ',' exp                         {$$ = $1; addSibling($1, $3); yyerrok;}
    |   exp                                     {$$ = $1;}
    |   argList ',' error                       {$$ = NULL;}
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
    initErrorProcessing();
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
        insertIO();
        setIsUsed(stdout, syntaxTree); //mark variables that aren't used for future warnings
        semantic(stdout, syntaxTree);
        if (declarations.lookupGlobal("main") == NULL) {
            printf("ERROR(LINKER): A function named 'main' with no parameters must be defined.\n");
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