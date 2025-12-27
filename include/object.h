#ifndef OBJECT_H
#define OBJECT_H

#include "common.h"
#include "error.h"

typedef enum {
    ObjectType_none = 0,
    ObjectType_real,
    ObjectType_bool,
    ObjectType_string,
    ObjectType_function,
    ObjectType_namespace,
} ObjectType;

struct Object_t;
struct Object_VT_t;

typedef struct Object_t {
    ObjectType type;
    struct Object_VT_t *vt;
    void *data;
} Object_t;

typedef struct Object_VT_t {
    Object_t* (*add)(Object_t*, Object_t*);
    Object_t* (*sub)(Object_t*, Object_t*);
    Object_t* (*mul)(Object_t*, Object_t*);
    Object_t* (*div)(Object_t*, Object_t*);
    Object_t* (*cmp)(Object_t*, Object_t*);
    Object_t* (*to_bool)(Object_t*);
    Object_t* (*dump)(Object_t*);
    Object_t* (*destroy)(Object_t*);
} Object_VT_t;

/* forward declarations */
typedef struct Real_t       Real_t_t;
typedef struct String_t     String_t;
typedef struct Bool_t       Bool_t;
typedef struct Function_t   Function_t;
typedef struct Namespace_t  Namespace_t;

/* constructor */
Object_t* Object_ctor(ObjectType type, void *data);

/* dispatch macro */
#define _(obj, func, ...) ((obj)->vt->func ?                                        \
    (obj)->vt->func((obj), ##__VA_ARGS__)                                           \
    : (run_time_error("object of type %d has no method %s", (obj)->type, #func),    \
    (Object_t*)NULL))                                                               \

#endif