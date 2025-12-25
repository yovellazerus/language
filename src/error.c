
#include "../include/error.h"

void run_time_error(const char* fmt, ...){
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, RED "RUN-TIME-ERROR: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, ".\n" RESET);

    va_end(args);
    exit(1);
}