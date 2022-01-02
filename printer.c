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

error_t printer_disconnect()
{
    if (!g_is_connected) {
        return SUCCESS;
    }

    error_t error;

    error = serial_deinit(g_serial_fd);
    if (error) {
        print_error("Unable to disconnect from serial device.");
        return error;
    }

    g_is_connected = false;

    return SUCCESS;
}

error_t printer_status()
{
    error_t error;

    if (!g_is_connected) {
        print_error("Please connect to a serial device.");
        return ERROR;
    }

    error = gcode_send(g_serial_fd, "M27\n");
    if (error) {
        print_error("Failed to send status code.");
        return error;
    }

    return SUCCESS;;
}

error_t printer_print(const char *filename)
{
    if (!g_is_connected) {
        errno = ECOMM;
        print_error("Please connect to a serial device.");
        return ERROR;
    }

    error_t error;

    if (!filename) {
        errno = EINVAL;
        print_error("Please specify a file name.");
        return ERROR;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
            print_error("Unable to open file.");
        return ERROR;
    }

    // Write file data to the SD card
    error = gcode_send_file(g_serial_fd, file);
    fclose(file);
    if (error) {
        print_error("Unable to send file to printer.");
        return ERROR;
    }

    return SUCCESS;
}

error_t printer_send(const char *gcode)
{
    if (!g_is_connected) {
        errno = ECOMM;
        print_error("Please connect to a serial device.");
        return ERROR;
    }


    return gcode_send(g_serial_fd, gcode);
}
