#ifndef COMMAND_H
#define COMMAND_H

#include <error.h>

#define COMMAND_RESULT_MESSAGE_MAX 256
#define COMMAND_STRING_MAX 32
#define COMMAND_COUNT 5

typedef enum command {
    COMMAND_INVALID,
    COMMAND_QUIT,
    COMMAND_STATUS,
    COMMAND_CONNECT,
    COMMAND_LOAD,
    COMMAND_PRINT,
} command_t;

void command_exit();
error_t command_status();
error_t command_connect(char *command_args);
error_t command_load(char *command_args);
error_t command_print(char *command_args);

#endif
