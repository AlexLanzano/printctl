#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <daemon/printer.h>
#include <daemon/serial.h>
#include <daemon/gcode.h>
#include <common/error.h>

#define MAX_LINE_LENGTH 1024

static int g_serial_fd;
static bool g_is_connected = false;

error_t printer_connect(const char *serial_device, const uint64_t baud)
{
    error_t error;
    bool ok = false;
    char response[1024] = {0};

    if (!serial_device) {
        print_error("No serial device provided.");
        return EINVAL;
    }

    error = serial_init(serial_device, baud, &g_serial_fd);
    if (error) {
        print_error("Unable to connect to the serial device.");
        return error;
    }

    do {
        error = gcode_get_response(g_serial_fd, response, 1024, &ok);
        if (error) {
            return error;
        }

        printf("%s", response);
    } while(strncmp(response, "LCD status changed\n", 1024) != 0);

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
        return EINVAL;
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
        return EINVAL;
    }

    error_t error;

    if (!filename) {
        print_error("Please specify a file name.");
        return EINVAL;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        print_error("Unable to open file.");
        return errno;
    }

    // Send file data
    error = gcode_send_file(g_serial_fd, file);
    fclose(file);
    if (error) {
        print_error("Unable to send file to printer.");
        return EINVAL;
    }

    return SUCCESS;
}

error_t printer_send(const char *gcode)
{
    if (!g_is_connected) {
        print_error("Please connect to a serial device.");
        return ECOMM;
    }


    return gcode_send(g_serial_fd, gcode);
}
