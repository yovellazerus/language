#ifndef AST_H
#define AST_H

#include "common.h"
#include "object.h"
#include "env.h"
#include "error.h"

typedef enum {
    NodeType_literal = 0,
    NodeType_var,
    NodeType_assign,
    NodeType_add,
    NodeType_sub,
    NodeType_mul,       
    NodeType_div,
    NodeType_mod,
    NodeType_block,
    NodeType_if,
    NodeType_while,
    NodeType_func_def,
    NodeType_func_call,
    NodeType_dump,
} NodeType;

typedef struct Node_t Node_t;

/* forward declarations */
typedef struct Literal_t    Literal_t;
typedef struct Var_t        Var_t;
typedef struct Assign_t     Assign_t;
typedef struct Add_t        Add_t;
typedef struct Block_t      Block_t;
typedef struct If_t         If_t;
typedef struct While_t      While_t;

/* constructors */
Node_t* Literal(Object_t *obj);
Node_t* Var(const char *name);
Node_t* Assign(Node_t *lhs, Node_t *rhs);
Node_t* Add(Node_t *lhs, Node_t *rhs);
Node_t* Block(Node_t **stmts, size_t count);
Node_t* Dump(Node_t* exr);

Object_t* AST_assign(Node_t* var, Object_t* x, Env_t* env);
void      AST_destroy(Node_t *node);
Object_t* AST_eval(Node_t *node, Env_t* env);
void      AST_to_dot_format(const Node_t *root, const char* path);  // for debug

#endif
