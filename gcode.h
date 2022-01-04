#ifndef GCODE_H
#define GCODE_H

#include <stdarg.h>
#include <error.h>
#include <stdbool.h>

error_t gcode_get_response(const int serial_fd, char *response, size_t response_length, bool *ok);
error_t gcode_send(const int serial_fd, const char *code, ...);
error_t gcode_send_file(const int serial_fd, FILE *file);

#endif
