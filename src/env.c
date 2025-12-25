
#include "../include/env.h"

typedef struct Env_entry_t {
    const char* name;
    Object_t* value;
} Env_entry_t;

struct Env_t {
    struct Env_t* perent;
    Env_entry_t sym_table[MAX_VARS];
    size_t var_count;
};

Env_t*  Env_create(Env_t *perent){
    Env_t* res = (Env_t*)malloc(sizeof(*res));
    if(!res) run_time_error("no system memory");
    res->perent = perent;
    memset(res->sym_table, 0, sizeof(res->sym_table));
    res->var_count = 0;
    return res;
}

void   Env_destroy(Env_t* env){
    for(size_t i =0; i < env->var_count; i++){
        _(env->sym_table[i].value, destroy);
    }
    free(env);
}

void Env_insert(Env_t* env, const char* name, Object_t* value){
    if(!env || !name){
        run_time_error("bad env or bad var name in `Env_insert`");
    }
    // search and appdate
    for(size_t i = 0; i < env->var_count; i++){
        if (strcmp(env->sym_table[i].name, name) == 0) {
            env->sym_table[i].value = value;
            return;
        }
    }
    // new
    env->sym_table[env->var_count].name = name;
    env->sym_table[env->var_count].value = value;
    env->var_count++;
    if(env->var_count >= MAX_VARS) run_time_error("to many vars in the program, sory...");
}

// searching only in local env
Object_t* Env_search(Env_t* env, const char* name){
    if(!env || !name){
        run_time_error("bad env or bad var name in function: `%s`", __func__);
    }
    for(size_t i = 0; i < env->var_count; i++){
        if(strcmp(env->sym_table[i].name, name) == 0){
            return env->sym_table[i].value;
        }
    }
    char buf[256];
    snprintf(buf, sizeof(buf), "undefined variable '%s'", name);
    run_time_error(buf);  // not found!
    return NULL;
}
