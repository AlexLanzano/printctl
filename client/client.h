#ifndef CLIENT_H
#define CLIENT_H

#include <common/error.h>

error_t client_connect(const char *profile);
error_t client_disconnect();
error_t client_print(const char *file_path);

#endif
