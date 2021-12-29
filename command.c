#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <command.h>
#include <serial.h>
#include <error.h>

#define MAX_LINE_LENGTH 1024

static int g_serial_fd;
static bool g_is_connected = false;
const char g_command[COMMAND_COUNT][COMMAND_STRING_MAX] = {{"quit"},{"status"}, {"connect"}, {"load"},
                                                           {"print"}};

void command_exit()
{
    exit(0);
}

void command_status(command_result_t *result)
{
    if (!g_is_connected) {
        strncpy(result->message, "Please connect to a serial device.", COMMAND_RESULT_MESSAGE_MAX-1);
        return;
    }

    write(g_serial_fd, "M27", 3);
    return;
}

void command_connect(char *command_args, command_result_t *result)
{
    error_t error;
    char serial_device[256] = {0};
    uint64_t baud;

    char *token = strtok(command_args, " ");
    if (!token) {
        result->error = ERROR_INVALID;
        strncpy(result->message, "No serial device provided.", COMMAND_RESULT_MESSAGE_MAX-1);
        return;
    }
    strncpy(serial_device, token, 255);

    token = strtok(NULL, " ");
    if (!token) {
        result->error = ERROR_INVALID;
        strncpy(result->message, "No baud rate provided.", COMMAND_RESULT_MESSAGE_MAX-1);
        return;
    }

    baud = atoi(token);
    if (baud == 0) {
        strncpy(result->message, "Invalid baud rate provided.", COMMAND_RESULT_MESSAGE_MAX-1);
        return;
    }

    error = serial_init(serial_device, baud, &g_serial_fd);
    if (error != SUCCESS) {
        strncpy(result->message, "Unable to connect to the serial device.", COMMAND_RESULT_MESSAGE_MAX-1);
        g_is_connected = false;
    } else {
        g_is_connected = true;
    }
}

void command_load(char *command_args, command_result_t *result)
{
    if (!g_is_connected) {
        strncpy(result->message, "Please connect to a serial device.", COMMAND_RESULT_MESSAGE_MAX-1);
        return;
    }

    char *filename = strtok(command_args, " ");
    char code[512] = {0};
    char line[MAX_LINE_LENGTH] = {0};

    FILE *file = fopen(filename, "rb");
    if (!file) {
        strncpy(result->message, "Unable to open file.", COMMAND_RESULT_MESSAGE_MAX-1);
        return;
    }

    // Send g code to start writing to the a file on the SD card
    strncpy(code, "M28 ", 511);
    strncat(code, filename, 507);
    write(g_serial_fd, code, strlen(code));

    // Write file data to the SD card
    while (fgets(line, MAX_LINE_LENGTH, file)) {
        printf("%s", line);
        write(g_serial_fd, line, strlen(line));
    }

    // Send g code to stop writing to the a file on the SD card
    strncpy(code, "M29 ", 511);
    strncat(code, filename, 507);
    write(g_serial_fd, code, strlen(code));
}

void command_print(char *command_args, command_result_t *result)
{
    if (!g_is_connected) {
        strncpy(result->message, "Please connect to a serial device.", COMMAND_RESULT_MESSAGE_MAX-1);
        return;
    }

    char code[512] = {0};
    char *filename = strtok(command_args, " ");

    // Set the file that will be printed from the SD card
    strncpy(code, "M23 ", 511);
    strncat(code, filename, 507);
    write(g_serial_fd, code, strlen(code));

    // Start the print
    write(g_serial_fd, "M24", 3);
}
