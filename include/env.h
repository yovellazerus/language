#ifndef ENV_H
#define ENV_H

#include "object.h"
#include "common.h"

#define MAX_VARS 1024

typedef struct Env_t Env_t;

Env_t*      Env_create(Env_t *parent);
void        Env_insert(Env_t *Env_t, const char *name, Object_t *val);
Object_t*   Env_search(Env_t *Env_t, const char *name);
void        Env_destroy(Env_t *Env_t);

#endif
