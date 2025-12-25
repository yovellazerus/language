
#include "../include/common.h"
#include "../include/ast.h"
#include "../include/object.h"

#include "object.h"
#include <stdlib.h>

void test0(void){

    Env_t* global_env = Env_create(NULL);

    // x = 3.14
    double* d1 = malloc(sizeof(*d1));
    *d1 = 3.14;
    Node_t* x_var = Var("x");
    Node_t* x_assign = Assign(x_var, Literal(Object_ctor(ObjectType_real, d1)));

    // y = 2.0
    double* d2 = malloc(sizeof(*d2));
    *d2 = 2.0;
    Node_t* y_var = Var("y");
    Node_t* y_assign = Assign(y_var, Literal(Object_ctor(ObjectType_real, d2)));

    // z = x + y
    Node_t* z_var = Var("z");
    Node_t* add_xy = Add(x_var, y_var);
    Node_t* z_assign = Assign(z_var, add_xy);

    // ?z
    Node_t* dump_z = Dump(z_var);

    // x = z + 1
    double* d3 = malloc(sizeof(*d3));
    *d3 = 1.0;
    Node_t* add_z1 = Add(z_var, Literal(Object_ctor(ObjectType_real, d3)));
    Node_t* x_assign2 = Assign(x_var, add_z1);

    // ?x
    Node_t* dump_x = Dump(x_var);

    // Collect all statements in a single program node
    Node_t* stmts[6] = {x_assign, y_assign, z_assign, dump_z, x_assign2, dump_x};
    Node_t* root = Block(stmts, 6);
    
    // Dump the AST to a dot file
    AST_to_dot_format(root, "output/test0.dot");

    // Run the entire program
    AST_eval(root, global_env);
    
    // Cleanup
    // AST_destroy(root); bugs....
    Env_destroy(global_env);
}


int main(int argc, char* argv[]){
    (void) argc;
    (void) argv;
    test0();
    return 0;
}