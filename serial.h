#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <error.h>

error_t serial_init(char *serial_device_path, uint64_t baud, int *serial_fd);

#endif
