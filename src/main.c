
#include "../include/common.h"
#include "../include/ast.h"
#include "../include/object.h"

// Helper functions
double* malloc_double(double x) {
    double* p = malloc(sizeof(*p));
    *p = x;
    return p;
}

bool* malloc_bool(bool b) {
    bool* p = malloc(sizeof(*p));
    *p = b;
    return p;
}

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

void test1(void) {
    Env_t* global_env = Env_create(NULL);

    // ---------- Reals ----------
    Node_t* a_assign = Assign(Var("a"), Literal(Object_ctor(ObjectType_real, malloc_double(10.0))));
    Node_t* b_assign = Assign(Var("b"), Literal(Object_ctor(ObjectType_real, malloc_double(20.0))));
    Node_t* c_assign = Assign(
        Var("c"),
        Add(Var("a"), Var("b"))
    );
    Node_t* dump_c = Dump(Var("c"));

    // ---------- Booleans ----------
    Node_t* flag_assign = Assign(
        Var("flag"),
        Literal(Object_ctor(ObjectType_bool, malloc_bool(true)))
    );

    // not_flag = flag OR flag (represented as Add in your AST)
    Node_t* not_flag_assign = Assign(
        Var("not_flag"),
        Add(Var("flag"), Var("flag"))
    );
    Node_t* dump_flag = Dump(Var("not_flag"));

    // ---------- Strings ----------
    Node_t* s1_assign = Assign(Var("s1"), Literal(Object_ctor(ObjectType_string, strdup("Hello "))));
    Node_t* s2_assign = Assign(Var("s2"), Literal(Object_ctor(ObjectType_string, strdup("World!"))));
    Node_t* s3_assign = Assign(
        Var("s3"),
        Add(Var("s1"), Var("s2"))
    );
    Node_t* dump_s3 = Dump(Var("s3"));

    // ---------- Collect statements ----------
    Node_t* stmts[] = {
        a_assign, b_assign, c_assign, dump_c,
        flag_assign, not_flag_assign, dump_flag,
        s1_assign, s2_assign, s3_assign, dump_s3
    };
    Node_t* root = Block(stmts, sizeof(stmts)/sizeof(stmts[0]));

    // dump AST to dot format
    AST_to_dot_format(root, "output/test1.dot");

    // evaluate
    AST_eval(root, global_env);

    // cleanup
    //AST_destroy(root);
    Env_destroy(global_env);
}

int main(int argc, char* argv[]){
    (void) argc;
    (void) argv;
    test0();
    test1();
    return 0;
}