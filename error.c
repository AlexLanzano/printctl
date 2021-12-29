#include <stdio.h>
#include <error.h>

void print_error(const char *msg)
{
    if (!msg) {
        return;
    }

    fprintf(stderr, "ERROR %i: %s\n", errno, msg);
}
