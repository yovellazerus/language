
#include "../include/ast.h"

Env_t* global_env = NULL;

// virtual AST node struct
struct Node_t {
    NodeType type;
};

// 3.14159265359, "Hello World!", False ...
typedef struct Literal_t {
    Node_t base;
    Object_t* obj;
} Literal_t;

// +
typedef struct Add_t {
    Node_t base;
    Node_t* left;
    Node_t* right;
} Add_t;

// dump
typedef struct Dump_t {
    Node_t base;
    Node_t* exr;
} Dump_t;

// =
typedef struct Assign_t {
    Node_t base;
    Node_t* left;
    Node_t* right;
} Assign_t;

// _var_foo42
typedef struct Var_t {
    Node_t base;
    const char* name;
} Var_t;
Node_t* Var(const char* name);

typedef struct Block_t {
    Node_t base;
    Node_t** statements;
    size_t count;          
} Block_t;

Node_t* Literal(Object_t* obj){
    Literal_t* res = malloc(sizeof(*res));
    if(!res) run_time_error("no system memory");
    res->base.type = NodeType_literal;
    res->obj = obj;
    return (Node_t*)res;
}

Node_t* Block(Node_t** statements, size_t count) {
    Block_t* res = malloc(sizeof(*res));
    if(!res) run_time_error("no system memory");
    res->base.type = NodeType_block;
    res->statements = statements;
    res->count = count;
    return (Node_t*)res;
}

Node_t* Dump(Node_t* exr){
    Dump_t* res = malloc(sizeof(*res));
    if(!res) run_time_error("no system memory");
    res->base.type = NodeType_dump;
    res->exr = exr;
    return (Node_t*)res; 
}

Node_t* Var(const char* name){
    Var_t* res = malloc(sizeof(*res));
    if(!res) run_time_error("no system memory");
    res->base.type = NodeType_var;
    res->name = name;
    return (Node_t*)res;
}

Node_t* Add(Node_t *lhs, Node_t *rhs){
    Add_t* res = malloc(sizeof(*res));
    if(!res) run_time_error("no system memory");
    res->base.type = NodeType_add;
    res->left = lhs;
    res->right = rhs;
    return (Node_t*)res;    
}

Node_t* Assign(Node_t *lhs, Node_t *rhs){
    Assign_t* res = malloc(sizeof(*res));
    if(!res) run_time_error("no system memory");
    res->base.type = NodeType_assign;
    res->left = lhs;
    res->right = rhs;
    return (Node_t*)res;
}

Object_t* AST_eval(Node_t* node, Env_t* env){
    if(!node) run_time_error("bad AST node");
    Block_t* p = NULL;
    Object_t* last = NULL;
    
    switch (node->type) {
        
        case NodeType_literal:
            return ((Literal_t*)node)->obj;
        
        case NodeType_add:
            return _(AST_eval(((Add_t*)node)->left, env), add, AST_eval(((Add_t*)node)->right, env));
            
        case NodeType_assign:
            return AST_assign(((Assign_t*)node)->left, AST_eval(((Assign_t*)node)->right, env), env);
            
        case NodeType_dump:
            return _(AST_eval(((Dump_t*)node)->exr, env), dump);
            
        case NodeType_var:
            return Env_search(env, ((Var_t*)node)->name);
        
        // to make a forest in to a tree
        case NodeType_block:
            p = (Block_t*)node;
            last = NULL;
            for(size_t i = 0; i < p->count; i++) {
                last = AST_eval(p->statements[i], env);
            }
            return last;
            break;

        default:
           run_time_error("unkown node type in AST eval, type-code: %d", node->type);
           break;
    }
    return NULL;
}

// only one global env for now...
Object_t* AST_assign(Node_t* var, Object_t* x, Env_t* env){
    if(!var || var->type != NodeType_var){
        run_time_error("must assign to a var");
    }
    Env_insert(env, ((Var_t*)var)->name, x);
    return x;
}

void Object_print_dot(const Object_t* obj, FILE* f, int* my_id);

static int dot_node_id = 0;

static int AST_to_dot_rec(FILE* f,const Node_t* node) {
    if (!node) return -1;
    
    int my_id = dot_node_id++;
    
    switch(node->type) {
        case NodeType_literal:
            Object_print_dot(((Literal_t*)node)->obj, f, &my_id);
            break;
        case NodeType_var:
            fprintf(f, "  node%d [label=\"Var: %s\"];\n", my_id, ((Var_t*)node)->name);
            break;
        case NodeType_add:
            fprintf(f, "  node%d [label=\"+\"];\n", my_id);
            if(((Add_t*)node)->left) {
                int l = AST_to_dot_rec(f, ((Add_t*)node)->left);
                fprintf(f, "  node%d -> node%d;\n", my_id, l);
            }
            if(((Add_t*)node)->right) {
                int r = AST_to_dot_rec(f, ((Add_t*)node)->right);
                fprintf(f, "  node%d -> node%d;\n", my_id, r);
            }
            break;
        case NodeType_assign:
            fprintf(f, "  node%d [label=\"=\"];\n", my_id);
            if(((Assign_t*)node)->left) {
                int l = AST_to_dot_rec(f, ((Assign_t*)node)->left);
                fprintf(f, "  node%d -> node%d;\n", my_id, l);
            }
            if(((Assign_t*)node)->right) {
                int r = AST_to_dot_rec(f, ((Assign_t*)node)->right);
                fprintf(f, "  node%d -> node%d;\n", my_id, r);
            }
            break;
        case NodeType_dump:
            fprintf(f, "  node%d [label=\"Dump\"];\n", my_id);
            if(((Dump_t*)node)->exr) {
                int e = AST_to_dot_rec(f, ((Dump_t*)node)->exr);
                fprintf(f, "  node%d -> node%d;\n", my_id, e);
            }
            break;
            
        case NodeType_block: {
            fprintf(f, "  node%d [label=\"Block\"];\n", my_id);
            Block_t* p = (Block_t*)node;
            for(size_t i = 0; i < p->count; i++) {
                int child_id = AST_to_dot_rec(f, p->statements[i]);
                if(child_id >= 0)
                    fprintf(f, "  node%d -> node%d;\n", my_id, child_id);
            }
            break;
}
        default:
            fprintf(f, "  node%d [label=\"Unknown\"];\n", my_id);
            break;
    }
    
    return my_id;
}

void AST_to_dot_format(const Node_t* node, const char* path) {
    FILE* f = fopen(path, "w");
    if(!f) {
        fprintf(stderr, "Cannot open `%s` for writing\n", path);
        return;
    }
    
    dot_node_id = 0;
    fprintf(f, "digraph AST {\n");
    AST_to_dot_rec(f, node);
    fprintf(f, "}\n");
    fclose(f);
}

void Object_print_dot(const Object_t* obj, FILE* f, int* my_id){
    switch (obj->type){
        case ObjectType_real:
            fprintf(f, "  node%d [label=\"Real: %lf\"];\n", *my_id, *(double*)obj->data);
            break;
        case ObjectType_string:
            fprintf(f, "  node%d [label=\"String: %s\"];\n", *my_id, (const char*)obj->data);
            break;
        case ObjectType_bool:
            fprintf(f, "  node%d [label=\"Bool: %s\"];\n", *my_id, (*(bool*)obj->data) ? "True" : "False");
            break;
        case ObjectType_none:
            fprintf(f, "  node%d [label=\"Nil\"];\n", *my_id);
            break;
        default:
            run_time_error("unknown type of object in print dot format, type-code: %d", obj->type);
            break;
    }
}

void AST_destroy(Node_t* node){
    if(!node) return;
    Block_t* p = NULL;
    switch (node->type){
        
        case NodeType_literal:
            _(((Literal_t*)node)->obj, destroy);
            break;
            
        case NodeType_add:
            AST_destroy(((Add_t*)node)->left);
            AST_destroy(((Add_t*)node)->right);
            break;
            
        case NodeType_dump:
            AST_destroy(((Dump_t*)node)->exr);
            break;
            
        case NodeType_assign:
            AST_destroy(((Assign_t*)node)->left);
            AST_destroy(((Assign_t*)node)->right);
            break;
            
        case NodeType_block:
            p = (Block_t*)node;
            for(size_t i = 0; i < p->count; i++)
                AST_destroy(p->statements[i]);
            free(p->statements);
            break;
            
        case NodeType_var:
            break;
        
        default:
           run_time_error("unkown node type in `%s`, type-code: %d", __func__, node->type);
           break;
    }
    free(node);
    return;
}

