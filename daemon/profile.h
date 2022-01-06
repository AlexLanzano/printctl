#ifndef PROFILE_H
#define PROFILE_H

#include <stdint.h>
#include <common/error.h>

typedef struct profile {
    char name[32];
    char serial_device[256];
    uint64_t baud;
} profile_t;

error_t profile_init(const char *name, profile_t *profile);

#endif
