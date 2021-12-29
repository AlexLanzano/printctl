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

typedef struct command_result {
    command_t command;
    error_t error;
    char message[COMMAND_RESULT_MESSAGE_MAX];
} command_result_t;

void command_exit();
void command_status(command_result_t *result);
void command_connect(char *command_args, command_result_t *result);
void command_load(char *command_args, command_result_t *result);
void command_print(char *command_args, command_result_t *result);

#endif
