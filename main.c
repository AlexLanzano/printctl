#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <command.h>

#define INPUT_LENGTH 256

extern char g_command[COMMAND_COUNT][COMMAND_STRING_MAX];

static command_t parse_command(char *command)
{
    // TODO: Optimize
    for (size_t i = 0; i< COMMAND_COUNT; i++) {
        if (strncmp(command, g_command[i], strlen(g_command[i])) == 0) {
            return (command_t)i+1;
        }
    }

    return COMMAND_INVALID;
}

static char *get_command_args(char *input, char *command)
{
    if (!input || !command) {
        return NULL;
    }

    size_t input_length = strlen(input);
    size_t command_length = strlen(command);

    if (input_length == command_length ||
        input_length > command_length+1) {
        return NULL;
    }

    return &input[command_length+1];

}

static error_t process_input(char *input)
{
    if (!input) {
        return ERROR;
    }

    error_t error;
    char *command = strtok(input, " ");
    char *command_args = get_command_args(input, command);

    switch (parse_command(command)) {
    case COMMAND_INVALID:
        print_error("Invalid command.");
        break;

    case COMMAND_QUIT:
        command_exit();
        break;

    case COMMAND_STATUS:
        error = command_status();
        break;

    case COMMAND_CONNECT:
        error = command_connect(command_args);
        break;

    case COMMAND_LOAD:
        error = command_load(command_args);
        break;

    case COMMAND_PRINT:
        error = command_print(command_args);
        break;
    }

    return error;
}

int32_t main(int32_t argc, char **argv)
{
    char input[INPUT_LENGTH] = {0};

    while (1) {
        printf("> ");
        fgets(input, INPUT_LENGTH, stdin);

        // Don't do anything if the user enters an empty input
        if (input[0] == '\n') {
            continue;
        }

        // Check if the input is too big
        if (input[strlen(input)-1] != '\n') {
            printf("Invalid input!\n");
            // Clear out the remaining characters on the input stream
            while (input[strlen(input)-1] != '\n') {
                fgets(input, INPUT_LENGTH, stdin);
            }
            continue;
        }

        // Clear the newline character at the end of the input
        input[strlen(input)-1] = 0;

        // Process the input
        process_input(input);
    }

    return 0;
}
