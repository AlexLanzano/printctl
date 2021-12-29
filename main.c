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

static uint32_t process_input(char *input, command_result_t *result)
{
    if (!input || !result) {
        return 1;
    }

    char *command = strtok(input, " ");
    char *command_args = get_command_args(input, command);

    switch (parse_command(command)) {
    case COMMAND_INVALID:
        result->command = COMMAND_INVALID;
        strncpy(result->message, "Invalid command.", COMMAND_RESULT_MESSAGE_MAX-1);
        break;

    case COMMAND_QUIT:
        result->command = COMMAND_QUIT;
        command_exit();
        break;

    case COMMAND_STATUS:
        result->command = COMMAND_STATUS;
        command_status(result);
        break;

    case COMMAND_CONNECT:
        result->command = COMMAND_CONNECT;
        command_connect(command_args, result);
        break;

    case COMMAND_LOAD:
        result->command = COMMAND_LOAD;
        command_load(command_args, result);
        break;

    case COMMAND_PRINT:
        result->command = COMMAND_PRINT;
        command_print(command_args, result);
        break;
    }

    return 0;
}

int32_t main(int32_t argc, char **argv)
{
    char input[INPUT_LENGTH] = {0};
    bool is_running = true;

    while (is_running) {
        command_result_t command_result = {0};

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
        process_input(input, &command_result);

        // Print result message if there is one
        if (strlen(command_result.message) > 0) {
            printf("%s\n", command_result.message);
        }
    }

    return 0;
}
