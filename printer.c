#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <printer.h>
#include <serial.h>
#include <error.h>
#include <gcode.h>

#define MAX_LINE_LENGTH 1024

static int g_serial_fd;
static bool g_is_connected = false;

error_t printer_connect(const char *serial_device, const uint64_t baud)
{
    error_t error;

    if (!serial_device) {
        errno = EINVAL;
        print_error("No serial device provided.");
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

error_t printer_status()
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

error_t printer_load(const char *filename)
{
    if (!g_is_connected) {
        errno = ECOMM;
        print_error("Please connect to a serial device.");
        return ERROR;
    }

    error_t error;
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

error_t printer_print(const char *filename)
{
    if (!g_is_connected) {
        errno = ECOMM;
        print_error("Please connect to a serial device.");
        return ERROR;
    }

    if (!filename) {
        errno = EINVAL;
        print_error("Please provide a filename to print.");
        return ERROR;
    }

    error_t error;

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
