
#include "../include/object.h"

// Real
//==========================================================================================

Object_t* Real_add(Object_t* a,  Object_t* b) {
    if (b->type != ObjectType_real) run_time_error("cannot add real + type-code: %d", b->type);
    double* result = malloc(sizeof(double));
    *result = *(double*)a->data + *(double*)b->data;
    return Object_ctor(ObjectType_real, result);
}
Object_t* Real_dump( Object_t* obj) {
    printf("%lf\n", *(double*)obj->data);
    return NULL;
}
bool Real_to_bool( Object_t* obj) {
    return *(double*)obj->data != 0.0;
}
void Real_dtor(Object_t* obj) {
    free(obj->data);
    free(obj);
}

Object_VT_t Real_vt = {
    .add = Real_add,
    .sub = NULL,
    .mul = NULL,
    .div = NULL,
    .dump = Real_dump,
    .to_bool = Real_to_bool,
    .destroy = Real_dtor
};

// String
//==========================================================================================

Object_t* String_add( Object_t* a,  Object_t* b) {
    if (b->type != ObjectType_string)
        run_time_error("cannot add string + type-code: %d", b->type);

    const char* s1 = (const char*)a->data;
    const char* s2 = (const char*)b->data;
    size_t len = strlen(s1) + strlen(s2) + 1;

    char* result = malloc(len);
    if (!result) run_time_error("no system memory");

    strcpy(result, s1);
    strcat(result, s2);

    return Object_ctor(ObjectType_string, result);
}
Object_t* String_dump( Object_t* obj) {
    printf("%s\n", (const char*)obj->data);
    return NULL;
}
bool String_to_bool( Object_t* obj) {
    return ((const char*)obj->data)[0] != '\0';
}
void String_dtor(Object_t* obj) {
    free(obj->data);
    free(obj);
}

Object_VT_t String_vt = {
    .add = String_add,
    .sub = NULL,        
    .mul = NULL,        
    .div = NULL,        
    .dump = String_dump,
    .to_bool = String_to_bool,
    .destroy = String_dtor
};

// Bool
//==========================================================================================

Object_t* Bool_add( Object_t* a,  Object_t* b) {
    if (b->type != ObjectType_bool)
        run_time_error("cannot add bool + type-code: %d", b->type);
    bool* result = malloc(sizeof(bool));
    *result = (*(bool*)a->data) || (*(bool*)b->data); // 'or' is '+' for bool
    return Object_ctor(ObjectType_bool, result);
}

Object_t* Bool_mul( Object_t* a,  Object_t* b) {
    if (b->type != ObjectType_bool)
        run_time_error("cannot mul bool * type-code: %d", b->type);
    bool* result = malloc(sizeof(bool));
    *result = (*(bool*)a->data) && !(*(bool*)b->data); // 'and' is '*' for bool
    return Object_ctor(ObjectType_bool, result);
}

Object_t* Bool_dump( Object_t* obj) {
    printf("%s\n", (*(bool*)obj->data) ? "True" : "False");
    return NULL;
}

bool Bool_to_bool( Object_t* obj) {
    return *(bool*)obj->data;
}

void Bool_dtor(Object_t* obj) {
    free(obj->data);
    free(obj);
}

Object_VT_t Bool_vt = {
    .add = Bool_add,
    .sub = NULL,
    .mul = Bool_mul,      
    .div = NULL,      
    .dump = Bool_dump,
    .to_bool = Bool_to_bool,
    .destroy = Bool_dtor
};

// general
//==========================================================================================

Object_t* Object_ctor(ObjectType type, void* data) {
    Object_t* obj = malloc(sizeof(*obj));
    if (!obj) run_time_error("no system memory");
    
    obj->type = type;
    obj->data = data;

    switch(type) {
        case ObjectType_real:
            obj->vt = &Real_vt;
            break;
        case ObjectType_string:
            obj->vt = &String_vt;
            break;
        case ObjectType_bool:
            obj->vt = &Bool_vt;
            break;
        default:
            run_time_error("unknown object type code: %d", type);
    }
    return obj;
}