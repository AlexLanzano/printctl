#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stddef.h>
#include <common/error.h>

error_t serial_init(const char *serial_device_path, const uint64_t baud, int *serial_fd);
error_t serial_deinit();
error_t serial_read(const int serial_fd, char *buffer, const size_t buffer_size, size_t *bytes_read);
error_t serial_readline(const int serial_fd, char *buffer, const size_t buffer_size, size_t *bytes_read);
error_t serial_write(const int serial_fd, char *data);
#endif
