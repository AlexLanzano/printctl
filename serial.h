#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <error.h>
#include <stddef.h>

error_t serial_init(char *serial_device_path, uint64_t baud, int *serial_fd);
error_t serial_read(const int serial_fd, char *buffer, const size_t buffer_size);
error_t serial_write(const int serial_fd, char *data);
#endif
