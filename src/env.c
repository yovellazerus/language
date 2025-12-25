
#include "../include/env.h"

typedef struct Env_entry_t {
    const char* name;
    Object_t* value;
} Env_entry_t;

struct Env_t {
    struct Env_t* perent;
    Env_entry_t sym_table[MAX_VARS_IN_SCOPE];
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
        if(env->sym_table[i].value) _(env->sym_table[i].value, destroy);
    }
    free(env);
}

// inserting only to LOCAL env
void Env_insert(Env_t* env, const char* name, Object_t* value){
    if(!env || !name){
        run_time_error("bad env or bad var name in `%s`", __func__);
    }
    // search and appdate
    for(size_t i = 0; i < env->var_count; i++){
        if (strcmp(env->sym_table[i].name, name) == 0) {
            if(env->sym_table[i].value) _(env->sym_table[i].value, destroy);
            env->sym_table[i].value = value;
            return;
        }
    }
    // new
    env->sym_table[env->var_count].name = name;
    env->sym_table[env->var_count].value = value;
    env->var_count++;
    if(env->var_count >= MAX_VARS_IN_SCOPE) run_time_error("to many vars in the in scope, sorry...");
}

// search in local and going upp to the GLOBAL (when the perent is NULL)
Object_t* Env_search(Env_t* env, const char* name){
    if(!env || !name){
        run_time_error("bad env or bad var name in function: `%s`", __func__);
    }
    while (env)
    {
        for(size_t i = 0; i < env->var_count; i++){
            if(strcmp(env->sym_table[i].name, name) == 0){
                return env->sym_table[i].value;
            }
        }
        env = env->perent;
    }
    run_time_error("undefined variable: '%s'", name);  // not found!
    return NULL;
}
