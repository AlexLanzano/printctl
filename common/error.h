#ifndef ERROR_H
#define ERROR_H

#include <errno.h>

#define SUCCESS 0

typedef int error_t;

void print_error(const char *msg);

#endif
