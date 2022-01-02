#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <printer.h>

#define INPUT_LENGTH 256
#define COMMAND_STRING_MAX 32
#define COMMAND_COUNT 6

typedef enum command {
    COMMAND_INVALID,
    COMMAND_QUIT,
    COMMAND_CONNECT,
    COMMAND_DISCONNECT,
    COMMAND_STATUS,
    COMMAND_PRINT,
    COMMAND_SEND,
} command_t;

static const char g_command[COMMAND_COUNT][COMMAND_STRING_MAX] = {{"quit"}, {"connect"}, {"disconnect"}, {"status"},
                                                                  {"print"}, {"send"}};

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

static error_t process_connect_command()
{
    char *token;
    char serial_device[256] = {0};
    int baud;

    token = strtok(NULL, " ");
    if (!token) {
        errno = EINVAL;
        print_error("No serial device provided.");
        return ERROR;
    }
    strncpy(serial_device, token, 255);

    token = strtok(NULL, " ");
    if (!token) {
        errno = EINVAL;
        print_error("No baud rate provided.");
        return ERROR;
    }

    baud = atoi(token);
    if (baud == 0) {
        errno = EINVAL;
        print_error("Invalid baud rate provided.");
        return ERROR;
    }

    return printer_connect(serial_device, baud);
}

static error_t process_print_command()
{
    char *token;
    char filename[256] = {0};

    token = strtok(NULL, " ");
    if (!token) {
        errno = EINVAL;
        print_error("Please specify a file to print.");
        return ERROR;
    }
    strncpy(filename, token, 255);

    return printer_print(filename);
}

/* static error_t process_send_command() */
/* { */
/*     char *token; */
/*     char gcode[512] = {0}; */

/*     token = strtok(NULL, " "); */
/*     while (token) { */
/*         char arg[256]; */
/*         snprintf(arg, 256, "%s ") */
/*         token = strtok(NULL, " "); */
/*     } */

/*     if (strlen(gcode) == 0) { */
/*         return ERROR; */
/*     } */

/*     return printer_send(gcode); */
/* } */

static error_t process_input(char *input)
{
    if (!input) {
        return ERROR;
    }
    error_t error;
    char *command = strtok(input, " ");

    switch (parse_command(command)) {
    case COMMAND_INVALID:
        print_error("Invalid command.");
        break;

    case COMMAND_QUIT:
        printer_disconnect();
        exit(0);
        break;

    case COMMAND_CONNECT:
        error = process_connect_command();
        break;

    case COMMAND_DISCONNECT:
        error = printer_disconnect();
        break;

    case COMMAND_STATUS:
        error = printer_status();
        break;

    case COMMAND_PRINT:
        error = process_print_command();
        break;

    case COMMAND_SEND:
        error = printer_send(&input[5]);
        break;

    }
    return error;
}

error_t run_script(int32_t argc, char **argv)
{
    error_t error;
    char input[INPUT_LENGTH] = {0};
    size_t line_count = 0;

    FILE *script = fopen(argv[1], "r");
    if (!script) {
        print_error("Failed to open script.");
        return ERROR;
    }

    while (fgets(input, INPUT_LENGTH, script)) {
        if (input[strlen(input)-1] == '\n') {
            input[strlen(input)-1] = 0;
        }
        error = process_input(input);
        if (error) {
            printf("Failed to execute command on line %li.\n", line_count);
            return ERROR;
        }
        line_count++;
    }

    return SUCCESS;
}

int32_t main(int32_t argc, char **argv)
{
    char input[INPUT_LENGTH] = {0};

    if (argc > 1) {
        run_script(argc, argv);
    } else {
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
}
