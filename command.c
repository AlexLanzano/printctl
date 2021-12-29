#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <command.h>
#include <serial.h>
#include <error.h>
#include <gcode.h>

#define MAX_LINE_LENGTH 1024

static int g_serial_fd;
static bool g_is_connected = false;
const char g_command[COMMAND_COUNT][COMMAND_STRING_MAX] = {{"quit"},{"status"}, {"connect"}, {"load"},
                                                           {"print"}};

void command_exit()
{
    exit(0);
}

error_t command_status()
{
    error_t error;

    if (!g_is_connected) {
        print_error("Please connect to a serial device.");
        return ERROR;
    }

    error = gcode_send(g_serial_fd, "M27");
    if (error) {
        print_error("Failed to send status code.");
        return error;
    }
    return SUCCESS;;
}

error_t command_connect(char *command_args)
{
    error_t error;
    char serial_device[256] = {0};
    uint64_t baud;

    char *token = strtok(command_args, " ");
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

    error = serial_init(serial_device, baud, &g_serial_fd);
    if (error) {
        print_error("Unable to connect to the serial device.");
        return ERROR;
    }

    g_is_connected = true;
    return SUCCESS;
}

error_t command_load(char *command_args)
{
    if (!g_is_connected) {
        errno = ECOMM;
        print_error("Please connect to a serial device.");
        return ERROR;
    }

    error_t error;
    char *filename = strtok(command_args, " ");
    if (!filename) {
        errno = EINVAL;
        print_error("Please specify a file to load.");
        return ERROR;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        print_error("Unable to open file.");
        return ERROR;
    }

    // Send gcode to start writing to the a file on the SD card
    error = gcode_send(g_serial_fd, "M28 %s", filename);
    if (error) {
        print_error("Unable to send gcode M28.");
        return ERROR;
    }

    // Write file data to the SD card
    char line[MAX_LINE_LENGTH] = {0};
    while (fgets(line, MAX_LINE_LENGTH, file)) {
       serial_write(g_serial_fd, line);
    }
    fclose(file);

    // Send gcode to stop writing to the a file on the SD card
    error = gcode_send(g_serial_fd, "M29 %s", filename);
    if (error) {
        print_error("Unable to send gcode M29.");
    }

    return error;
}

error_t command_print(char *command_args)
{
    if (!g_is_connected) {
        errno = ECOMM;
        print_error("Please connect to a serial device.");
        return ERROR;
    }

    error_t error;
    char *filename = strtok(command_args, " ");
    if (!filename) {
        errno = EINVAL;
        print_error("Please provide a filename to print.");
        return ERROR;
    }

    // Set the file that will be printed from the SD card
    error = gcode_send(g_serial_fd, "M23 %s", filename);
    if (error) {
        print_error("Failed to set the file to print.");
        return ERROR;
    }

    // Start the print
    error = gcode_send(g_serial_fd, "M24");
    if (error) {
        print_error("Failed to start the print.");
    }

    return error;
}
