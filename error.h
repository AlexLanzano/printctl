#ifndef ERROR_H
#define ERROR_H

#include <errno.h>

typedef enum {
    SUCCESS,
    ERROR
} error_t;

void print_error(const char *msg);

#endif
