#ifndef PRINTER_H
#define PRINTER_H

#include <error.h>

error_t printer_connect(const char *serial_device, const uint64_t baud);
error_t printer_disconnect();
error_t printer_status();
error_t printer_print(const char *filename);
error_t printer_send(const char *gcode);

#endif
