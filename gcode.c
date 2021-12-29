#include <stdarg.h>
#include <error.h>
#include <serial.h>
#include <string.h>
#include <stdio.h>

#define MAX_GCODE_LENGTH 512

error_t gcode_send(const int serial_fd, const char *format, ...)
{
    if (!format) {
        errno = EINVAL;
        return ERROR;
    }

    error_t error;
    char gcode[MAX_GCODE_LENGTH];
    va_list ap;

    va_start(ap, format);

    vsnprintf(gcode, MAX_GCODE_LENGTH, format, ap);
    error = serial_write(serial_fd, gcode);

    va_end(ap);

    return error;
}
