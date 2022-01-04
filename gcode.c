#include <stdarg.h>
#include <error.h>
#include <serial.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_GCODE_LENGTH 512

static error_t gcode_clean(char *gcode)
{
    if (!gcode) {
        return ERROR;
    }

    // Skip comments
    if (gcode[0] == ';') {
        gcode[0] = 0;
    }

    size_t gcode_length = strlen(gcode);
    for (size_t i = 0; i < gcode_length; i++) {
        if (gcode[i] == ';') {
            gcode[i] = '\n';
            gcode[i+1] = 0;
            break;
        }
    }

    return SUCCESS;
}

static bool gcode_ok(char *response)
{
    if (!response) {
        return false;
    }

    size_t length = strlen(response);
    if (length == 0 || length != 3) {
        return false;
    }


    if (length >= 3) {
        if (strncmp(&response[length-3], "ok\n", 3) == 0) {
            return true;
        }
    }

    return false;
}

error_t gcode_get_response(const int serial_fd, char *response, size_t response_length, bool *ok)
{
    if (!response || !ok || !response_length) {
        return ERROR;
    }

    error_t error;
    size_t bytes_read = 0;

    error = serial_readline(serial_fd, response, response_length, &bytes_read);
    if (error) {
        return ERROR;
    }

    *ok = gcode_ok(response);

    return SUCCESS;
}

error_t gcode_send(const int serial_fd, const char *format, ...)
{
    if (!format) {
        errno = EINVAL;
        return ERROR;
    }

    error_t error;
    char gcode[MAX_GCODE_LENGTH];
    size_t gcode_length = 0;
    char response[1024] = {0};
    bool ok = false;
    va_list ap;

    memset(response, 0, 1024);

    va_start(ap, format);
    vsnprintf(gcode, MAX_GCODE_LENGTH, format, ap);
    va_end(ap);

    gcode_length = strlen(gcode);
    if (gcode[gcode_length-1] != '\n') {
        if (gcode_length+2 >= MAX_GCODE_LENGTH) {
            return ERROR;
        }
        gcode[gcode_length] = '\n';
        gcode[gcode_length+1] = 0;
    }

    printf("SENDING %s", gcode);
    error = serial_write(serial_fd, gcode);
    if (error) {
        return ERROR;
    }

    do {
        error = gcode_get_response(serial_fd, response, 1024, &ok);
        if (error) {
            printf("ERROR\n");
            return ERROR;
        }

        printf("%s", response);
    } while (!ok);
    return error;
}

error_t gcode_send_file(const int serial_fd, FILE *file)
{
    if (!file) {
        errno = EINVAL;
        return ERROR;
    }

    error_t error;
    char line[1024] = {0};

    while (fgets(line, 1024, file)) {
        gcode_clean(line);
        if (line[0] == 0 ||
            line[0] == '\n') {
            continue;
        }

        error = gcode_send(serial_fd, line);
        if (error) {
            return error;
        }
    }

    return SUCCESS;
}
