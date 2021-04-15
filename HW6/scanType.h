// Greyson Biggs
// CS 445
// 2/18/2021
// scanType.h
// Adapted from Professor Heckendorn's examples

#ifndef _SCANTYPE_H_
#define _SCANTYPE_H_
// 
//  SCANNER TOKENDATA
// 
struct TokenData {
    int  tokenclass;        // token class
    int  linenum;           // line where found
    char *tokenstr;         // what string was actually read
    char cvalue;            // any character value
    int  nvalue;            // any numeric value or Boolean value
    char *svalue;           // any string value e.g. an id
};

//
//  PARSER TREENODE
//

// the exact type of the token or node involved.  These are divided into
// various "kinds" in the enums that follow

// Kinds of Operators
// these are the token numbers for the operators same as in flex
typedef int OpKind;  

// Kinds of Statements
//typedef enum {DeclK, StmtK, ExpK} NodeKind;
enum NodeKind {DeclK, StmtK, ExpK};

// Subkinds of Declarations
enum DeclKind {VarK, FuncK, ParamK};

// Subkinds of Statements
enum StmtKind {NullK, IfK, WhileK, ForK, CompoundK, ReturnK, BreakK, RangeK};

// Subkinds of Expressions
enum ExpKind {OpK, ConstantK, IdK, AssignK, InitK, CallK};

// ExpType is used for type checking (Void means no type or value, UndefinedType means undefined)
enum ExpType {Void, Integer, Boolean, Char, CharInt, Equal, UndefinedType};

// What kind of scoping is used?  (decided during typing)
enum VarKind {None, Local, Global, Parameter, LocalStatic};

#ifndef TREENODE_H
#define TREENODE_H

typedef struct treeNode
{
    // connectivity in the tree
    struct treeNode *children[3];           // children of the node
    struct treeNode *sibling;               // siblings for the node
    struct TokenData *token;

    // what kind of node
    int lineno;                             // linenum relevant to this node
    NodeKind nodekind;                      // type of this node
    union                                   // subtype of type
    {
	    DeclKind decl;                      // used when DeclK
    	StmtKind stmt;                      // used when StmtK
	    ExpKind exp;                        // used when ExpK
    } subkind;
    
    // extra properties about the node depending on type of the node
    union                                   // relevant data to type -> attr
    {
        OpKind op;                          // type of token (same as in bison)
	    int value;                          // used when an integer constant or boolean
        unsigned char cvalue;               // used when a character
	    char *string;                       // used when a string constant
	    char *name;                         // used when IdK
    } attr;                                 
    ExpType expType;		                // used when ExpK for type checking
    VarKind varKind;                        // see enum just above
    char *expstr;                           // stores expType as a string, for printTree
    bool isArray;                           // is this an array
    bool isStatic;                          // is staticly allocated?
    bool isUsed;                            // shows up as ExpK?
    bool isInitialized;                     // can safely assume a value is assigned?
    bool isForIdDecl;                       // is this an int declared in a For loop?
    bool isConst;                           // is this a constant variable?
    int offset;                             // node offset
    int size;                               // node size
    int arrlen;                             // length of (array) node when declared
} TreeNode;

#endif
#endif